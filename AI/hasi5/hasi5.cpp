#include "../../include/TraceDumper.hpp"
#include "../../include/model.hpp"
#include <vector>
#include <set>
#include <unordered_map>
#include <time.h>

namespace std{
    template<>
    class hash<coordinate>{
        public:
        size_t operator () ( const coordinate &p ) const{ return (p.x << 16) ^ (p.y << 8) ^ p.z; }
    };
}

const int INF = 1<<30;

typedef coordinate P;

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

bool checkFilled(P p1, P p2, const set<P>& filled){
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
                if (filled.count(P(x, y, z)))return true;
            }
    return false;
}

Command* check_move(P p1, P p2, P d, const set<P>& filled) {
    int cnt = 0;
    if (d.x != 0) ++cnt;
    if (d.y != 0) ++cnt;
    if (d.z != 0) ++cnt;

    // SMOVE
    if (cnt == 1
        && -15 <= d.x && d.x <= 15
        && -15 <= d.y && d.y <= 15
        && -15 <= d.z && d.z <= 15) {
        if(checkFilled(p1, p2, filled))return nullptr;
        return new SMove(d);
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
            return new LMove(mid1 - p1, p2-mid1);
        }
        if(!checkFilled(p1, mid2, filled) && !checkFilled(mid2, p2, filled)){
            return new LMove(mid2 - p1, p2-mid2);
        }
    }
    return nullptr;
}

const P dir[6] = {
	P(0,-1,0),
	P(+1,0,0),
	P(-1,0,0),
	P(0,0,+1),
	P(0,0,-1),
	P(0,+1,0),
};

struct UnionFind {
	vector<int> data;
	UnionFind() {}
	UnionFind(int n) : data(n, -1) {}
	int root(int x) {
		return data[x] < 0 ? x : data[x] = root(data[x]);
	}
	bool merge(int x, int y) {
		x = root(x);
		y = root(y);
		if (x == y) return false;
		if (data[y] < data[x]) swap(x, y);
		data[x] += data[y];
		data[y] = x;
		return true;
	}
};

bool is_near(P p1, P p2) {
	int dx = abs(p1.x - p2.x);
	int dy = abs(p1.y - p2.y);
	int dz = abs(p1.z - p2.z);
	return max(max(dx,dy),dz) <= 1 && 1 <= dx+dy+dz && dx+dy+dz <= 2;
}

bool is_in(P p, int R) {
	return 0 <= p.x && p.x < R && 0 <= p.y && p.y < R && 0 <= p.z && p.z < R;
}

int uf_idx(P p, int R) {
	return (p.x * R + p.y) * R + p.z;
}



void calc_dist(unordered_map<P, int>& dist, P pos, const set<P>& filled, const set<P>& targets, int R) {
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
				if (is_in(pp, R) && !filled.count(pp) && !dist.count(pp)) {
					dist[pp] = dd;
					if (targets.count(pp)) if (++ cnt_targets == num_targets) return;
					Q.push(pp);
				}
			}
		}
	}
	cerr << "calc_dist failure" << endl;
	throw 1;
}

vector<Command*> get_path(P p1, P p2, const set<P>& filled, const unordered_map<P, int>& dist, int R) {
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
			cerr << "[get_path] something wrong" << endl;
			throw 1;
			next2:;
		}
	}
	vector<Command*> res;
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
			auto move = check_move(pos, pp, d, filled);
			if (move != nullptr) {
				res.push_back(move);
				if (i == 0) return res;
				pos = pp;
				n = i;
				goto out3;
			}
		}
		cerr << "[get_path] move not found" << endl;
		throw 1;
		out3:;
	}
	cerr << "not implemented" << endl;
	throw 1;
}

int main(int argc, char **argv){
	vector<P> nears;
	for (int x = -1; x <= 1; ++ x) for (int y = -1; y <= 1; ++ y) for (int z = -1; z <= 1; ++ z) {
		if (is_near(P(x,y,z),P(0,0,0))) {
			nears.push_back(P(x,y,z));
		}
	}

	cout << argv[1] << endl;
	Model model(argv[1]);
	int R = model.R;

	vector<vector<P>> a(1);
	set<P> b, used;
	for (int y = 0; y < R-1; ++ y) {
		for (int x = 1; x < R-1; ++ x) {
			for (int z = 1; z < R-1; ++ z) {
				if (model(x,y,z)) {
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
			cerr << "model is something wrong" << endl;
			throw 1;
		}
		used = used2;
	}

	P pos(0,0,0);
	vector<Command*> commands;

	set<P> filled;
	vector<P> aa;
	for (auto x : a) {
		aa.insert(aa.end(), x.begin(), x.end());
	}

	UnionFind uf_cache;
	CacheBox cacheb(R);

	double sum1 = 0;
	double sum2 = 0;
	double sum3 = 0;
	for (const auto& p : aa) {
		//cerr << p << endl;

		auto t0 = clock();

		auto filled2 = filled;
		filled2.insert(p);
		cacheb.touch(p);

		UnionFind uf2;
		if (cacheb.dirty) {
			//cerr << "regen" << p << endl;
			uf_cache = UnionFind(R*R*R);
			cacheb.setPos(p);
			for (int x = 0; x < R; ++ x) for (int y = 0; y < R; ++ y) for (int z = 0; z < R; ++ z) if (!filled2.count(P(x,y,z))) {
				P p1(x,y,z);
				if (cacheb.check(p1)) continue;
				if (x+1 < R) {
					P p2(x+1, y, z);
					if (!cacheb.check(p2) && !filled2.count(p2)) uf_cache.merge(uf_idx(p1,R),uf_idx(p2,R));
				}
				if (y+1 < R) {
					P p2(x, y+1, z);
					if (!cacheb.check(p2) && !filled2.count(p2)) uf_cache.merge(uf_idx(p1,R),uf_idx(p2,R));
				}
				if (z+1 < R) {
					P p2(x, y, z+1);
					if (!cacheb.check(p2) && !filled2.count(p2)) uf_cache.merge(uf_idx(p1,R),uf_idx(p2,R));
				}
			}
		}
		uf2 = uf_cache;
		auto box = cacheb.box2;
		for (int x = get<0>(box); x <= get<1>(box); ++ x)
		for (int y = get<2>(box); y <= get<3>(box); ++ y)
		for (int z = get<4>(box); z <= get<5>(box); ++ z)
		if (!filled2.count(P(x,y,z))) {
			P p1(x,y,z);
			if (x+1 < R) {
				P p2(x+1, y, z);
				if (!filled2.count(p2)) uf2.merge(uf_idx(p1,R),uf_idx(p2,R));
			}
			if (y+1 < R) {
				P p2(x, y+1, z);
				if (!filled2.count(p2)) uf2.merge(uf_idx(p1,R),uf_idx(p2,R));
			}
			if (z+1 < R) {
				P p2(x, y, z+1);
				if (!filled2.count(p2)) uf2.merge(uf_idx(p1,R),uf_idx(p2,R));
			}
		}
		UnionFind uf1 = uf2;
		for (int k = 0; k < 6; ++ k) {
			auto pp = p + dir[k];
			if (!is_in(pp, R) || filled.count(pp)) continue;
			uf1.merge(uf_idx(p,R),uf_idx(pp,R));
		}

		vector<P> target_d;
		set<P> target_pp;
		for (auto d : nears) {
			auto pp = p + d;
			if (!is_in(pp, R) || filled.count(pp)) continue;
			if (uf1.root(uf_idx(pos,R)) != uf1.root(uf_idx(pp,R))) continue;
			if (uf2.root(uf_idx(pp,R)) != uf2.root(0)) continue;
			target_d.push_back(d);
			target_pp.insert(pp);
		}

		auto t1 = clock();

		unordered_map<P, int> dist;
		calc_dist(dist, pos, filled, target_pp, R);

		auto t2 = clock();
		vector<Command*> min_path;
		int min_cost = INF;
		P next_pos;
		for (auto d : target_d) {
			auto pp = p + d;
			if (!dist.count(pp)) {
				cerr << "dist is not enough" << endl;
				throw 1;
			}
			auto path = get_path(pos, pp, filled, dist, R);
			int cost = path.size();
			if (cost < min_cost) {
				min_path = path;
				min_cost = cost;
				next_pos = pp;
			}
		}
		if (min_cost == INF) {
			cerr << "no path" << endl;
			throw 1;
		}
		commands.insert(commands.end(), min_path.begin(), min_path.end());
		pos = next_pos;
		if (is_near(p, pos)) {
			filled.insert(p);
			cacheb.touch(p);
			commands.push_back(new Fill(p - pos));
		} else {
			cerr << "is not near" << endl;
			throw 1;
		}
		auto t3 = clock();
		sum1 += t1-t0;
		sum2 += t2-t1;
		sum3 += t3-t2;
	}
	cout << "sum1=" << sum1 << endl;
	cout << "sum2=" << sum2 << endl;
	cout << "sum3=" << sum3 << endl;
	{
		unordered_map<P, int> dist;
		set<P> targets = { P(0,0,0) };
		calc_dist(dist, pos, filled, targets, R);
		auto path = get_path(pos, P(0,0,0), filled, dist, R);
		commands.insert(commands.end(), path.begin(), path.end());
	}
	commands.push_back(new Halt());

	dumpTrace(argv[2], commands);
}
