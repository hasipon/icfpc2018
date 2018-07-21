#include "../../include/TraceDumper.hpp"
#include "../../include/model.hpp"
#include <vector>
#include <set>

typedef coordinate P;

const P dir[6] = {
	P(0,-1,0),
	P(+1,0,0),
	P(-1,0,0),
	P(0,0,+1),
	P(0,0,-1),
	P(0,+1,0),
};

bool is_near(P p1, P p2) {
	int dx = abs(p1.x - p2.x);
	int dy = abs(p1.y - p2.y);
	int dz = abs(p1.z - p2.z);
	return max(max(dx,dy),dz) <= 1 && 1 <= dx+dy+dz && dx+dy+dz <= 2;
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

pair<vector<Command*>, bool> get_path(P p1, P p2, const set<P>& filled, int R, int limit) {
	{
		int dx = p2.x - p1.x;
		int dy = p2.y - p1.y;
		int dz = p2.z - p1.z;
		auto d = P(dx,dy,dz);
		if (dx == 0 && dy == 0 && dz == 0) return {{}, true};
		auto move = check_move(p1, p2, d, filled);
		if (move != nullptr) return {{ move }, true};
	}
	auto pos = p1;
	vector<Command*> res;
	for (;;) {
		set<P> checked = filled;
		queue<P> Q;
		Q.push(p2);
		checked.insert(p2);
		while (!Q.empty()) {
			auto pp = Q.front(); Q.pop();
			int dx = pp.x - pos.x;
			int dy = pp.y - pos.y;
			int dz = pp.z - pos.z;
			auto d = P(dx,dy,dz);
			auto move = check_move(pos, pp, d, filled);
			if (move != nullptr) {
				res.push_back(move);
				if (pp.x == p2.x && pp.y == p2.y && pp.z == p2.z) return {res, true};
				if (res.size() == limit) return {{}, false};
				pos = pp;
				goto out2;
			}
			for (auto dd : dir) {
				auto pp2 = pp + dd;
				if (!(0 <= pp2.x && pp2.x < R && 0 <= pp2.y && pp2.y < R && 0 <= pp2.z && pp2.z < R) || checked.count(pp2)) continue;
				Q.push(pp2);
				checked.insert(pp2);
			}
		}
		return {{}, false};
		out2:;
	}
}

int main(int argc, char **argv){
	vector<P> nears;
	for (int x = -1; x <= 1; ++ x) for (int y = -1; y <= 1; ++ y) for (int z = -1; z <= 1; ++ z) {
		if (is_near(P(x,y,z),P(0,0,0))) {
			nears.push_back(P(x,y,z));
		}
	}

	cout  << argv[1] << endl;
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
	for (const auto& p : aa) {
		if (is_near(p, pos)) {
			filled.insert(p);
			commands.push_back(new Fill(p - pos));
		} else {
			vector<Command*> min_path;
			int min_cost = 1<<30;
			P next_pos;
			for (auto d : nears) {
				auto pp = p + d;
				if (!filled.count(pp)) {
					auto path = get_path(pos, pp, filled, R, min_cost);
					if (!path.second) continue;
					int cost = path.first.size();
					if (cost < min_cost) {
						min_path = path.first;
						min_cost = cost;
						next_pos = pp;
					}
				}
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
		}
	}
	{
		auto path = get_path(pos, P(0,0,0), filled, R, 1<<30).first;
		commands.insert(commands.end(), path.begin(), path.end());
	}
	commands.push_back(new Halt());

	dumpTrace(argv[2], commands);
}
