#include "../../include/TraceDumper.hpp"
#include "../../include/model.hpp"
#include <vector>
#include <set>
#include <time.h>

const int INF = 1<<30;

typedef coordinate P;

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

bool uf_idx(P p, int R) {
	return (p.x * R + p.y) * R + p.z;
}

bool is_connected(P p1, P p2, const set<P>& filled, int R) {
	set<P> visited = filled;
	queue<P> Q;
	visited.insert(p1);
	Q.push(p1);
	while (!Q.empty()) {
		auto p = Q.front(); Q.pop();
		if (p == p2) return true;
		for (int k = 0; k < 6; ++ k) {
			auto pp = p + dir[k];
			if (is_in(pp, R) && !visited.count(pp)) {
				visited.insert(pp);
				Q.push(pp);
			}
		}
	}
	return false;
}

Command* check_move(P p1, P p2, P d, const set<P>& filled) {
	int cnt = 0;
	if (d.x != 0) ++ cnt;
	if (d.y != 0) ++ cnt;
	if (d.z != 0) ++ cnt;
	if (cnt == 1 && -15 <= d.x && d.x <= 15 && -15 <= d.y && d.y <= 15 && -15 <= d.z && d.z <= 15) {
		int xmin = min(p1.x, p2.x), xmax = max(p1.x, p2.x);
		int ymin = min(p1.y, p2.y), ymax = max(p1.y, p2.y);
		int zmin = min(p1.z, p2.z), zmax = max(p1.z, p2.z);
		for (int x = xmin; x <= xmax; ++ x) for (int y = ymin; y <= ymax; ++ y) for (int z = zmin; z <= zmax; ++ z) {
			if (filled.count(P(x,y,z))) goto out1;
		}
		return new SMove(d);
		out1:;
	}
	return nullptr;
}

void calc_dist(map<P, int>& dist, P pos, const set<P>& filled, const set<P>& targets, int R) {
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
}

vector<Command*> get_path(P p1, P p2, const set<P>& filled, const map<P, int>& dist, int R) {
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
	double sum1 = 0;
	double sum2 = 0;
	double sum3 = 0;
	for (const auto& p : aa) {
		//cerr << p << endl;

		auto t0 = clock();

		auto filled2 = filled;
		filled2.insert(p);

		UnionFind uf2(R*R*R);
		for (int x = 0; x < R; ++ x) for (int y = 0; y < R; ++ y) for (int z = 0; z < R; ++ z) if (!filled.count(P(x,y,z))) {
			P p1(x,y,z);
			if (x+1 < R) {
				P p2(x+1, y, z);
				if (!filled.count(p2)) uf2.merge(uf_idx(p1,R),uf_idx(p2,R));
			}
			if (y+1 < R) {
				P p2(x, y+1, z);
				if (!filled.count(p2)) uf2.merge(uf_idx(p1,R),uf_idx(p2,R));
			}
			if (z+1 < R) {
				P p2(x, y, z+1);
				if (!filled.count(p2)) uf2.merge(uf_idx(p1,R),uf_idx(p2,R));
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

		map<P, int> dist;
		calc_dist(dist, pos, filled, target_pp, R);

		auto t2 = clock();
		vector<Command*> min_path;
		int min_cost = INF;
		P next_pos;
		for (auto d : target_d) {
			auto pp = p + d;
			if (!dist.count(pp)) continue;
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
		map<P, int> dist;
		set<P> targets = { P(0,0,0) };
		calc_dist(dist, pos, filled, targets, R);
		auto path = get_path(pos, P(0,0,0), filled, dist, R);
		commands.insert(commands.end(), path.begin(), path.end());
	}
	commands.push_back(new Halt());

	dumpTrace(argv[2], commands);
}
