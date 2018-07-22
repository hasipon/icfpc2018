#include "../../include/point.hpp"
#include "../../include/model.hpp"
#include "../../include/output.hpp"
#include <stdio.h>
#include <algorithm>
#include <vector>

using namespace std;

typedef Point P;

typedef vector<uint64_t> Filled;

struct Main : OutputBase {
	const Model& Src, Tgt;
	const int R;
	Main(const Model& Src, const Model& Tgt, std::ofstream& ofs) : OutputBase(ofs), Src(Src), Tgt(Tgt), R(max(Src.R, Tgt.R)) {}

Filled newFilled() const { return Filled((R*R*R+63)/64); }
void set(Filled& a, P p) const {
	int x = (p.x * R + p.y) * R + p.z;
	a[x / 64] |= 1ULL << (x % 64);
}
bool get(const Filled& a, P p) const {
	int x = (p.x * R + p.y) * R + p.z;
	return (a[x / 64] >> (x % 64)) & 1;
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
				if (get(filled, P(x, y, z)))return true;
			}
	return false;
}

void check_move(P p1, P p2, P d, const Filled& filled) {
	int cnt = 0;
	if (d.x != 0) ++cnt;
	if (d.y != 0) ++cnt;
	if (d.z != 0) ++cnt;

	// SMOVE
	if (cnt == 1
		&& -15 <= d.x && d.x <= 15
		&& -15 <= d.y && d.y <= 15
		&& -15 <= d.z && d.z <= 15) {
		if(checkFilled(p1, p2, filled)) throw "checkFilled";
		SMove(d); return;
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
			LMove(mid1 - p1, p2-mid1); return;
		}
		if(!checkFilled(p1, mid2, filled) && !checkFilled(mid2, p2, filled)){
			LMove(mid2 - p1, p2-mid2); return;
		}
	}
	cout << d <<endl;
	cout << p1 <<' ' << p2 << endl;
	throw "check_move failure";
}

void solve() {
	Flip();

	vector<P> a;
	for (int y = 0; y < R-1; ++ y) {
		for (int x = 1; x < R-1; ++ x) {
			if (x % 2) {
				for (int z = 1; z < R-1; ++ z) {
					if (Tgt(x,y,z)) {
						a.push_back(P(x,y,z));
					}
				}
			} else {
				for (int z = R-2; z > 0; -- z) {
					if (Tgt(x,y,z)) {
						a.push_back(P(x,y,z));
					}
				}
			}
		}
	}

	auto filled = newFilled();
	P pos(0,0,0);
	for (const auto& p : a) {
		auto pp = p + P(0,1,0);
		int dx = pp.x - pos.x;
		int dy = pp.y - pos.y;
		int dz = pp.z - pos.z;

		while (dy < -15) { SMove(P(0,-15,0)); dy += 15; }
		while (dy > 15) { SMove(P(0,15,0)); dy -= 15; }

		while (dx < -15) { SMove(P(-15,0,0)); dx += 15; }
		while (dx > 15) { SMove(P(15,0,0)); dx -= 15; }

		while (dz < -15) { SMove(P(0,0,-15)); dz += 15; }
		while (dz > 15) { SMove(P(0,0,15)); dz -= 15; }

		int cnt = 0;
		if(dx != 0)cnt++;
		if(dy != 0)cnt++;
		if(dz != 0)cnt++;
		if (abs(dy) > 5 && cnt >= 2) {
			SMove(P(0, dy, 0));
			dy = 0;
			cnt--;
		}
		if (abs(dx) > 5 && cnt >= 2) {
			SMove(P(dx, 0, 0));
			dx = 0;
			cnt--;
		}
		if (abs(dz) > 5 && cnt >= 2) {
			SMove(P(0, 0, dz));
			dz = 0;
			cnt--;
		}
		if(dz != 0 && dx != 0 && dy != 0){
			SMove(P(0,dy,0));
			dy = 0;
		}

		pos = pp - P(dx, dy, dz);

		check_move(pos, pp, P(dx, dy,dz), filled);

		Fill(P(0,-1,0));
		set(filled, p);
		pos = pp;
	}
	{
		int dx = -pos.x;
		int dz = -pos.z;

		while (dx < -15) { SMove(P(-15,0,0)); dx += 15; }
		while (dx > 15) { SMove(P(15,0,0)); dx -= 15; }
		while (dz < -15) { SMove(P(0,0,-15)); dz += 15; }
		while (dz > 15) { SMove(P(0,0,15)); dz -= 15; }

		int cnt = 0;
		if(dx != 0)cnt++;
		if(dz != 0)cnt++;
		if (abs(dx) > 5 && cnt >= 2) {
			SMove(P(dx, 0, 0));
			dx = 0;
			cnt--;
		}
		if (abs(dz) > 5 && cnt >= 2) {
			SMove(P(0, 0, dz));
			dz = 0;
			cnt--;
		}

		pos = P(0,pos.y,0) - P(dx, 0, dz);
		check_move(pos, P(0,pos.y,0), P(dx,0,dz), filled);

		int dy = -pos.y;
		while (dy < -15) { SMove(P(0,-15,0)); dy += 15; }
		while (dy > 15) { SMove(P(0,-15,0)); dy -= 15; }
		pos = P(0,0,0) - P(0, dy, 0);
		check_move(pos, P(0,0,0), P(0,dy,0), filled);
	}

	Flip();
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
