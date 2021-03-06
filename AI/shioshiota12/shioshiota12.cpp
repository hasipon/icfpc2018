#include "../../include/point.hpp"
#include "../../include/model.hpp"
#include "../../include/output.hpp"
#include "../../include/union_find.hpp"
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <unordered_map>
#include <time.h>

using namespace std;

const int INF = 1<<30;

typedef Point P;

typedef vector<uint64_t> Filled;

const P dir[6] = {
	P(0,-1,0),
	P(+1,0,0),
	P(-1,0,0),
	P(0,0,+1),
	P(0,0,-1),
	P(0,+1,0),
};

bool is_in(P p, int R) {
	return 0 <= p.x && p.x < R && 0 <= p.y && p.y < R && 0 <= p.z && p.z < R;
}

int uf_idx(P p, int R) {
	return (p.x * R + p.y) * R + p.z;
}

bool is_near(P p1, P p2) {
	int dx = abs(p1.x - p2.x);
	int dy = abs(p1.y - p2.y);
	int dz = abs(p1.z - p2.z);
	return max(max(dx,dy),dz) == 1 && dx+dy+dz <= 2;
}

struct CacheBox {
	const int R;
	tuple<int,int,int,int,int,int> box1, box2;
	bool dirty;
	CacheBox(int R) : R(R), dirty(true) {}
	void setPos(P p) {
		int K = 5;
		dirty = false;
		box1 = make_tuple(
			max(0,   p.x - K),
			min(R-1, p.x + K),
			max(0,   p.y - K),
			min(R-1, p.y + K),
			max(0,   p.z - K),
			min(R-1, p.z + K)
		);
		box2 = make_tuple(
			max(0,   p.x - K - 1),
			min(R-1, p.x + K + 1),
			max(0,   p.y - K - 1),
			min(R-1, p.y + K + 1),
			max(0,   p.z - K - 1),
			min(R-1, p.z + K + 1)
		);
	}
	void touch(P p) {
		if (dirty) return;
		if (!check(p)) dirty = true;
	}
	bool check(P p) {
		return
			get<0>(box1) <= p.x && p.x <= get<1>(box1) &&
			get<2>(box1) <= p.y && p.y <= get<3>(box1) &&
			get<4>(box1) <= p.z && p.z <= get<5>(box1);
	}
};

struct Main : OutputBase {
	const Model& Src, Tgt;
	const int R;
	Main(const Model& Src, const Model& Tgt, std::ofstream& ofs) : OutputBase(ofs), Src(Src), Tgt(Tgt), R(max(Src.R, Tgt.R)) {}

Filled newFilled() const { return Filled((R*R*R+63)/64); }
void Fset(Filled& a, P p) const {
	int x = (p.x * R + p.y) * R + p.z;
	a[x / 64] |= 1ULL << (x % 64);
}
void Freset(Filled& a, P p) const {
	int x = (p.x * R + p.y) * R + p.z;
	a[x / 64] &= ~(1ULL << (x % 64));
}
bool Fget(const Filled& a, P p) const {
	int x = (p.x * R + p.y) * R + p.z;
	return (a[x / 64] >> (x % 64)) & 1;
}

void move(P& pos, const P& to) {
	if (pos == to) return;

	auto d = to - pos;
	int cnt = 0;
	if (d.x != 0) ++cnt;
	if (d.y != 0) ++cnt;
	if (d.z != 0) ++cnt;
	if (cnt != 1) throw "move failure";

	while (d.x > 15) { SMove(P(15,0,0)); d.x -= 15; }
	while (d.x < -15) { SMove(P(-15,0,0)); d.x += 15; }
	while (d.y > 15) { SMove(P(0,15,0)); d.y -= 15; }
	while (d.y < -15) { SMove(P(0,-15,0)); d.y += 15; }
	while (d.z > 15) { SMove(P(0,0,15)); d.z -= 15; }
	while (d.z < -15) { SMove(P(0,0,-15)); d.z += 15; }
	if (d != P(0,0,0)) SMove(d);

	pos = to;
}

tuple<int,int,int,P> fission(tuple<int,int,int,P>& bot, int cap, P nd) {
	int bid = get<0>(bot);
	int s1 = get<1>(bot);
	int s2 = get<2>(bot);
	P pos = get<3>(bot);
	bot = make_tuple(bid, s2-cap+2, s2, pos);
	return make_tuple(s1, s1+1, s2-cap+1, pos+nd);
}

bool contains(const vector<P>& a, P n) {
	return find(a.begin(), a.end(), n) != a.end();
}

int sign(int n) {
	if (n < 0) return -1;
	if (n > 0) return +1;
	return 0;
}

// [ first : { true: Void, false: SMove }, second : arg ]
vector<pair<bool,P>> make_plan(P s, P t, Filled& filled) {
	vector<pair<bool,P>> res;
	int dx = sign(t.x - s.x);
	int dy = sign(t.y - s.y);
	int dz = sign(t.z - s.z);
	P d(dx,dy,dz);
	P pos = s, pos0 = s;
	int mlen = 0;
	do {
		auto prev = pos;
		pos += d;
		if (Fget(filled, pos)) {
			if (prev != pos0) res.push_back({false, prev-pos0});
			mlen = 0;
			pos0 = prev;
			res.push_back({true,d});
			Freset(filled, pos);
		} else if (mlen == 15) {
			res.push_back({false, prev-pos0});
			mlen = 0;
			pos0 = prev;
		}
		++ mlen;
	} while (pos != t);
	res.push_back({false, t-pos0});
	//cerr << "plan" << s << t << endl;
	//for (auto p : res) cerr << p.first << p.second << endl;
	return res;
}

void mmove(vector<P>& bpos, Filled& filled, const vector<int>& xs, const vector<int>& ys, const vector<int>& zs, bool gvoid) {
	const int n = bpos.size();
	vector<bool> moved(n);
	set<P> target_s;
	for (auto x : xs) for (auto y : ys) for (auto z : zs) {
		P p(x,y,z);
		auto it = find(bpos.begin(), bpos.end(), p);
		if (it == bpos.end()) target_s.insert(p);
		else moved[it - bpos.begin()] = true;
	}
	if (target_s.empty()) throw "[mmove] target_s empty";

	vector<vector<pair<bool,P>>> plan(n);
	for (int i = 0; i < n; ++ i) if (!moved[i]) {
		bool ok = false;
		for (auto t : target_s) {
			int cnt = 0;
			if (t.x != bpos[i].x) ++ cnt;
			if (t.y != bpos[i].y) ++ cnt;
			if (t.z != bpos[i].z) ++ cnt;
			if (cnt == 1) {
				target_s.erase(t);
				ok = true;
				plan[i] = make_plan(bpos[i], t, filled);
				bpos[i] = t;
				break;
			}
		}
		if (!ok) throw "[mmove] target not found";
	}
	if (!target_s.empty()) throw "[mmove] target_s must be empty";
	unsigned len_plan = 0;
	for (auto& a : plan) if (a.size() > len_plan) len_plan = a.size();
	for (unsigned t = 0; t < len_plan; ++ t) {
		for (int i = 0; i < n; ++ i) {
			if (t < plan[i].size()) {
				if (plan[i][t].first) {
					Void(plan[i][t].second);
				} else {
					SMove(plan[i][t].second);
				}
			} else Wait();
		}
	}

	if(gvoid) {
		map<P, int> a[2][2], b[2][2], c[2][2], d[2][2];
		for (int base = 0; base + 1 < int(zs.size()); base+=2) {
			for (int i = 0; i < 2; i++) {
			    int z = zs[base + (i + 1) %2] - zs[base + i%2];
				a[0][i].insert({{P(xs[0], ys[0], zs[base + i]), z}});
				b[0][i].insert({{P(xs[0], ys[1], zs[base + i]),z}});
				c[0][i].insert({{P(xs[1], ys[0], zs[base + i]),z}});
				d[0][i].insert({{P(xs[1], ys[1], zs[base + i]),z}});

				if (base + 1 + 1 < int(zs.size())) {
					int z = zs[base + 1 + (i + 1) %2] - zs[base + 1+ i%2];
					a[1][i].insert({{P(xs[0], ys[0], zs[base + i + 1]), z}});
					b[1][i].insert({{P(xs[0], ys[1], zs[base + i + 1]), z}});
					c[1][i].insert({{P(xs[1], ys[0], zs[base + i + 1]), z}});
					d[1][i].insert({{P(xs[1], ys[1], zs[base + i + 1]), z}});
				}
			}
		}
		for(int base = 0; base < 2; base++) {
			int cnt = 0;
			for (auto p : bpos) {
				bool commanded = false;
				for (int i = 0; i < 2; i++) {
					int x = abs(xs[0] - xs[1]) - 2;
					int y = abs(ys[0] - ys[1]);
					int local = 0;
					if (a[base][i].count(p)) {
						P nd = P(1, 0, 0);
						P fd = P(x, y, a[base][i][p]);
						GVoid(nd, fd);
						local++;
						cnt++;
					}
					if (b[base][i].count(p)){
						P nd = P(1, 0, 0);
						P fd = P(x, -y, b[base][i][p]);
						GVoid(nd, fd);
						local++;
						cnt++;
					}
					if (c[base][i].count(p)) {
						P nd = P(-1, 0, 0);
						P fd = P(-x, y, c[base][i][p]);
						GVoid(nd, fd);
						local++;
						cnt++;
					}
					if (d[base][i].count(p)) {
						P nd = P(-1, 0, 0);
						P fd = P(-x, -y, d[base][i][p]);
						GVoid(nd, fd);
						local++;
						cnt++;
					}
					if(local == 1){
						commanded = true;
						break;
					}
					if(local > 1){
						throw "INVALID GVOID";
					}
				}
				if (!commanded) {
					Wait();
				}
			}
			if (cnt%8 != 0)throw " error GVOID";
		}
	}
}

void disassemble() {
	auto filled = newFilled();
	for (int y = 0; y < R-1; ++ y) {
		for (int x = 1; x < R-1; ++ x) {
			for (int z = 1; z < R-1; ++ z) {
				if (Src(x,y,z)) {
					Fset(filled, P(x,y,z));
				}
			}
		}
	}

	vector<int> xs = {0, min(30, R-1)};
	vector<int> ys = {0, min(30, R-1)};
	vector<int> zs;
	for (int i = 0; i < R-1; i += 30) zs.push_back(i);
	zs.push_back(R-1);

	vector<tuple<int,int,int,P>> bots { make_tuple(0,1,39,P(0,0,0)) };
	for (unsigned i = 1; i < zs.size(); ++ i) {
		int nbot = bots.size();
		for (int j = 0; j < nbot; ++ j) {
			if (j == nbot-1) {
				auto nd = P(0,0,1);
				auto r = fission(bots[j], 4, nd);
				bots.push_back(r);
				Fission(nd, get<2>(r) - get<1>(r) + 1);
			}
			else Wait();
		}
	}
	for (;;) {
		int bidx = -1;
		int nbot = bots.size();
		for (int i = nbot-1; i >= 0; -- i) {
			if (get<3>(bots[i]).z != zs[i]) {
				bidx = i;
				break;
			}
		}
		if (bidx < 0) break;
		for (int i = 0; i < nbot; ++ i) {
			if (i == bidx) {
				auto& pos = get<3>(bots[i]);
				auto d = P(0,0,min(15, zs[i] - pos.z));
				SMove(d);
				pos += d;
			} else Wait();
		}
	}
	{
		int nbot = bots.size();
		for (int j = 0; j < nbot; ++ j) {
			auto nd = P(0,1,0);
			auto r = fission(bots[j], 2, nd);
			bots.push_back(r);
			Fission(nd, get<2>(r) - get<1>(r) + 1);
		}
		sort(bots.begin(), bots.end());
	}
	for (;;) {
		int nbot = bots.size();
		bool ok = true;
		for (int j = 0; j < nbot; ++ j) {
			auto& pos = get<3>(bots[j]);
			if (pos.y != 0 && pos.y != ys[1]) { ok = false; break; }
		}
		if (ok) break;
		for (int j = 0; j < nbot; ++ j) {
			auto& pos = get<3>(bots[j]);
			if (pos.y != 0) {
				auto d = P(0,min(15, ys[1] - pos.y),0);
				SMove(d);
				pos += d;
			} else Wait();
		}
	}
	{
		int nbot = bots.size();
		for (int j = 0; j < nbot; ++ j) {
			if (j == 0) Flip(); else Wait();
		}
	}
	// todo call Void
	{
		int nbot = bots.size();
		for (int j = 0; j < nbot; ++ j) {
			auto nd = P(1,0,0);
			auto r = fission(bots[j], 1, nd);
			bots.push_back(r);
			Fission(nd, get<2>(r) - get<1>(r) + 1);
		}
		sort(bots.begin(), bots.end());
	}
	vector<P> bpos;
	for (auto b : bots) bpos.push_back(get<3>(b));
	mmove(bpos, filled, xs, ys, zs, true);
	int state = 0;
	for (;;) {
		//cerr << "state = " << state << endl;
		if (state == 0) { // x plus
			if (xs[1] == R-1) {
				state = 2;
			} else {
				xs[1] = min(R-1, xs[1] + 29);
				mmove(bpos, filled, xs, ys, zs, false);
				xs[0] += 29;
				mmove(bpos, filled, xs, ys, zs, true);
			}
		} else if (state == 2) { // x reset
			if (ys[1] == R-1) break;
			if (xs[0] != 0) {
				xs[0] = 0;
				mmove(bpos, filled, xs, ys, zs, false);
			}
			if (xs[1] != min(30,R-1)) {
				xs[1] = min(30,R-1);
				mmove(bpos, filled, xs, ys, zs, false);
			}
			state = 3;
		} else if (state == 3) {
			ys[1] = min(R-1, ys[1] + 30);
			mmove(bpos, filled, xs, ys, zs, false);
			ys[0] += 30;
			mmove(bpos, filled, xs, ys, zs, true);
			state = 0;
		} else {
			throw "[disassemble] invalid state";
		}
	}
	{
		int nbot = bots.size();
		for (int j = 0; j < nbot; ++ j) {
			if (j == 0) Flip(); else Wait();
		}
	}
	auto emptyFilled = newFilled();
	if (xs[0] != 0) { xs[0] = 0; mmove(bpos, emptyFilled, xs, ys, zs, false); }
	if (xs[1] != 1) { xs[1] = 1; mmove(bpos, emptyFilled, xs, ys, zs, false); }
	if (ys[0] != 0) { ys[0] = 0; mmove(bpos, emptyFilled, xs, ys, zs, false); }
	if (ys[1] != 1) { ys[1] = 1; mmove(bpos, emptyFilled, xs, ys, zs, false); }
	for (int i = 0; i < (int)zs.size(); ++ i) {
		if (zs[i] != i) { zs[i] = i; mmove(bpos, emptyFilled, xs, ys, zs, false); }
	}
	{
		vector<P> a;
		for (auto p : bpos) {
			if (p.x == 0) {
				FusionP(P(1,0,0));
				a.push_back(p);
			} else {
				FusionS(P(-1,0,0));
			}
		}
		//for (auto p : a) cerr << p << endl;
		bpos.swap(a);
	}
	{
		vector<P> a;
		for (auto p : bpos) {
			if (p.y == 0) {
				FusionP(P(0,1,0));
				a.push_back(p);
			} else {
				FusionS(P(0,-1,0));
			}
		}
		//for (auto p : a) cerr << p << endl;
		bpos.swap(a);
	}
	{
		for (int n = bpos.size(); n > 1; -- n) {
			for (int i = 0; i < n; ++ i) {
				if (i == n-2) {
					FusionP(P(0,0,1));
				} else if (i == n-1) {
					FusionS(P(0,0,-1));
				} else Wait();
			}
		}
	}
}

void calc_dist(unordered_map<P, int>& dist, P pos, const Filled& filled, const set<P>& targets) const {
	int num_targets = targets.size();
	int cnt_targets = 0;
	dist[pos] = 0;
	if (targets.count(pos)) if (++ cnt_targets == num_targets) return;
	{
		queue<P> Q;
		Q.push(pos);
		while (!Q.empty()) {
			auto p = Q.front(); Q.pop();
			int dd = dist[p] + 1;
			for (int k = 0; k < 6; ++ k) {
				auto pp = p + dir[k];
				if (is_in(pp, R) && !Fget(filled, pp) && !dist.count(pp)) {
					dist[pp] = dd;
					if (targets.count(pp)) if (++ cnt_targets == num_targets) return;
					Q.push(pp);
				}
			}
		}
	}
	throw "calc_dist failure";
}

bool checkFilled(P p1, P p2, const Filled& filled){
    P d = p1 - p2;
    int cnt = 0;
    if (d.x != 0) ++cnt;
    if (d.y != 0) ++cnt;
    if (d.z != 0) ++cnt;
    if(cnt != 1)throw __LINE__;
    int xmin = min(p1.x, p2.x), xmax = max(p1.x, p2.x);
    int ymin = min(p1.y, p2.y), ymax = max(p1.y, p2.y);
    int zmin = min(p1.z, p2.z), zmax = max(p1.z, p2.z);
    for (int x = xmin; x <= xmax; ++x)
        for (int y = ymin; y <= ymax; ++y)
            for (int z = zmin; z <= zmax; ++z) {
                if (Fget(filled, P(x, y, z)))return true;
            }
    return false;
}

vector<P> calc_move(P p1, P p2, P d, const Filled& filled) {
    int cnt = 0;
    if (d.x != 0) ++cnt;
    if (d.y != 0) ++cnt;
    if (d.z != 0) ++cnt;

    // SMOVE
    if (cnt == 1
        && -15 <= d.x && d.x <= 15
        && -15 <= d.y && d.y <= 15
        && -15 <= d.z && d.z <= 15) {
        if(checkFilled(p1, p2, filled))return {};
        return {d};
    }

    // LMOVE
    if(cnt == 2
       && -5 <= d.x && d.x <= 5
       && -5 <= d.y && d.y <= 5
       && -5 <= d.z && d.z <= 5) {
        P mid1, mid2;
        if(d.x == 0){
            mid1 = P(p1.x, p1.y, p2.z);
            mid2 = P(p1.x, p2.y, p1.z);
        }else if (d.y == 0){
            mid1 = P(p1.x, p1.y, p2.z);
            mid2 = P(p2.x, p1.y, p1.z);
        }else if(d.z == 0){
            mid1 = P(p1.x, p2.y, p1.z);
            mid2 = P(p2.x, p1.y, p1.z);
        }
        else {
            assert(false);
        }

        if(!checkFilled(p1, mid1, filled) && !checkFilled(mid1, p2, filled)){
            return {mid1 - p1, p2-mid1};
        }
        if(!checkFilled(p1, mid2, filled) && !checkFilled(mid2, p2, filled)){
            return {mid2 - p1, p2-mid2};
        }
    }
    return {};
}

vector<vector<P>> get_path(P p1, P p2, const Filled& filled, const unordered_map<P, int>& dist) {
	if (p1 == p2) return {};
	vector<P> path1;
	{
		path1 = {p2};
		while (!(path1.back() == p1)) {
			auto p = path1.back();
			int dd = dist.find(p)->second - 1;
			for (int k = 0; k < 6; ++ k) {
				auto pp = p + dir[k];
				if (dist.count(pp) && dist.find(pp)->second == dd) {
					path1.push_back(pp);
					goto next2;
				}
			}
			throw "[get_path] something wrong";
			next2:;
		}
	}
	vector<vector<P>> res;
	auto pos = p1;
	int n = path1.size();
	-- n;
	for (;;) {
		for (int i = 0; i < n; ++ i) {
			auto pp = path1[i];
			int dx = pp.x - pos.x;
			int dy = pp.y - pos.y;
			int dz = pp.z - pos.z;
			auto d = P(dx,dy,dz);
			auto move = calc_move(pos, pp, d, filled);
			if (!move.empty()) {
				res.push_back(move);
				if (i == 0) return res;
				pos = pp;
				n = i;
				goto out3;
			}
		}
		throw "[get_path] move not found";
		out3:;
	}
	throw "not implemented";
}

void assemble() {
	vector<P> nears;
	for (int x = -1; x <= 1; ++ x) for (int y = -1; y <= 1; ++ y) for (int z = -1; z <= 1; ++ z) {
		if (is_near(P(x,y,z),P(0,0,0))) {
			nears.push_back(P(x,y,z));
		}
	}

	vector<vector<P>> a(1);
	set<P> b, used;
	for (int y = 0; y < R-1; ++ y) {
		for (int x = 1; x < R-1; ++ x) {
			for (int z = 1; z < R-1; ++ z) {
				if (Tgt(x,y,z)) {
					if (y == 0) {
						a.back().push_back(P(x,y,z));
						used.insert(P(x,y,z));
					} else {
						b.insert(P(x,y,z));
					}
				}
			}
		}
	}
	while (!b.empty()) {
		auto used2 = used;
		a.push_back(vector<P>());
		for (auto i = b.begin(); i != b.end(); ) {
			for (int k = 0; k < 6; ++ k) {
				if (used.count(*i + dir[k])) {
					a.back().push_back(*i);
					used2.insert(*i);
					i = b.erase(i);
					goto next1;
				}
			}
			++ i;
			next1:;
		}
		if (a.back().empty()) {
			throw "model is something wrong";
		}
		used = used2;
	}

	P pos(0,0,0);

	auto filled = newFilled();
	vector<P> aa;
	for (auto x : a) {
		vector<tuple<int,int,P>> bb;
		for (auto p : x) {
			if (p.z % 2) {
				bb.push_back(make_tuple(p.z, -p.x, p));
			} else {
				bb.push_back(make_tuple(p.z, p.x, p));
			}
		}
		sort(bb.begin(), bb.end());
		for (auto p : bb) {
			aa.push_back(get<2>(p));
		}
	}

	UnionFind uf_cache;
	CacheBox cacheb(R);

	for (const auto& p : aa) {
		auto filled2 = filled;
		Fset(filled2, p);
		cacheb.touch(p);

		UnionFind uf2;
		if (cacheb.dirty) {
			//cerr << "regen" << p << endl;
			uf_cache = UnionFind(R*R*R);
			cacheb.setPos(p);
			for (int x = 0; x < R; ++ x) for (int y = 0; y < R; ++ y) for (int z = 0; z < R; ++ z) if (!Fget(filled2, P(x,y,z))) {
				P p1(x,y,z);
				if (cacheb.check(p1)) continue;
				if (x+1 < R) {
					P p2(x+1, y, z);
					if (!cacheb.check(p2) && !Fget(filled2, p2)) uf_cache.unionSet(uf_idx(p1,R),uf_idx(p2,R));
				}
				if (y+1 < R) {
					P p2(x, y+1, z);
					if (!cacheb.check(p2) && !Fget(filled2, p2)) uf_cache.unionSet(uf_idx(p1,R),uf_idx(p2,R));
				}
				if (z+1 < R) {
					P p2(x, y, z+1);
					if (!cacheb.check(p2) && !Fget(filled2, p2)) uf_cache.unionSet(uf_idx(p1,R),uf_idx(p2,R));
				}
			}
		}
		uf2 = uf_cache;
		auto box = cacheb.box2;
		for (int x = get<0>(box); x <= get<1>(box); ++ x)
		for (int y = get<2>(box); y <= get<3>(box); ++ y)
		for (int z = get<4>(box); z <= get<5>(box); ++ z)
		if (!Fget(filled2, P(x,y,z))) {
			P p1(x,y,z);
			if (x+1 < R) {
				P p2(x+1, y, z);
				if (!Fget(filled2, p2)) uf2.unionSet(uf_idx(p1,R),uf_idx(p2,R));
			}
			if (y+1 < R) {
				P p2(x, y+1, z);
				if (!Fget(filled2, p2)) uf2.unionSet(uf_idx(p1,R),uf_idx(p2,R));
			}
			if (z+1 < R) {
				P p2(x, y, z+1);
				if (!Fget(filled2, p2)) uf2.unionSet(uf_idx(p1,R),uf_idx(p2,R));
			}
		}
		UnionFind uf1 = uf2;
		for (int k = 0; k < 6; ++ k) {
			auto pp = p + dir[k];
			if (!is_in(pp, R) || Fget(filled, pp)) continue;
			uf1.unionSet(uf_idx(p,R),uf_idx(pp,R));
		}

		vector<P> target_d;
		set<P> target_pp;
		for (auto d : nears) {
			auto pp = p + d;
			if (!is_in(pp, R) || Fget(filled, pp)) continue;
			if (uf1.root(uf_idx(pos,R)) != uf1.root(uf_idx(pp,R))) continue;
			if (uf2.root(uf_idx(pp,R)) != uf2.root(0)) continue;
			target_d.push_back(d);
			target_pp.insert(pp);
		}

		unordered_map<P, int> dist;
		calc_dist(dist, pos, filled, target_pp);

		vector<vector<P>> min_path;
		int min_cost = INF;
		P next_pos;
		for (auto d : target_d) {
			auto pp = p + d;
			if (!dist.count(pp)) {
				throw "dist is not enough";
			}
			auto path = get_path(pos, pp, filled, dist);
			int cost = path.size();
			if (cost < min_cost) {
				min_path = path;
				min_cost = cost;
				next_pos = pp;
			}
		}
		if (min_cost == INF) {
			throw "no path";
		}
		for (auto val : min_path) {
			if (val.size() == 1) {
				SMove(val[0]);
			} else if (val.size() == 2) {
				LMove(val[0], val[1]);
			} else {
				throw "invalid min_path element";
			}
		}
		pos = next_pos;
		if (is_near(p, pos)) {
			Fset(filled, p);
			cacheb.touch(p);
			Fill(p - pos);
		} else {
			throw "is not near";
		}
	}
	{
		unordered_map<P, int> dist;
		set<P> targets = { P(0,0,0) };
		calc_dist(dist, pos, filled, targets);
		auto path = get_path(pos, P(0,0,0), filled, dist);
		for (auto val : path) {
			if (val.size() == 1) {
				SMove(val[0]);
			} else if (val.size() == 2) {
				LMove(val[0], val[1]);
			} else {
				throw "invalid min_path element";
			}
		}
	}
}

void solve() {
	if (Src.R) disassemble();
	if (Tgt.R) assemble();
	Halt();
}

}; // struct Main end

void run(char **argv){
	Model model_src(argv[1]);
	Model model_tgt(argv[2]);
	std::ofstream ofs(argv[3]);
	auto inst = new Main(model_src, model_tgt, ofs);
	inst->solve();
}

int main(int argc, char **argv){
	try {
		if (argc != 4) throw "./ai SRC TGT OUTPUT";
		run(argv);
	} catch (const char* msg) {
		puts(msg);
		return 1;
	}
}
