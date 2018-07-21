#include <vector>
#include "../../include/TraceDumper.hpp"
#include "../../include/model.hpp"

typedef coordinate P;

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
	cout << d <<endl;
	cout << p1 <<' ' << p2 << endl;
	throw 1;
	return nullptr;
}

int main(int argc, char **argv){
	cout  << argv[1] << endl;
	Model model(argv[1]);
	int R = model.R;

	vector<P> a;
	for (int y = 0; y < R-1; ++ y) {
		for (int x = 1; x < R-1; ++ x) {
			if (x % 2) {
				for (int z = 1; z < R-1; ++ z) {
					if (model(x,y,z)) {
						a.push_back(P(x,y,z));
					}
				}
			} else {
				for (int z = R-2; z > 0; -- z) {
					if (model(x,y,z)) {
						a.push_back(P(x,y,z));
					}
				}
			}
		}
	}

	set<P> filled;
	P pos(0,0,0);
	vector<Command*> commands;
	commands.push_back(new Flip());
	for (const auto& p : a) {
		auto pp = p + P(0,1,0);
		int dx = pp.x - pos.x;
		int dy = pp.y - pos.y;
		int dz = pp.z - pos.z;

		while (dy < -15) { commands.push_back(new SMove(P(0,-15,0))); dy += 15; }
		while (dy > 15) { commands.push_back(new SMove(P(0,15,0))); dy -= 15; }

		while (dx < -15) { commands.push_back(new SMove(P(-15,0,0))); dx += 15; }
		while (dx > 15) { commands.push_back(new SMove(P(15,0,0))); dx -= 15; }

		while (dz < -15) { commands.push_back(new SMove(P(0,0,-15))); dz += 15; }
		while (dz > 15) { commands.push_back(new SMove(P(0,0,15))); dz -= 15; }

		int cnt = 0;
		if(dx != 0)cnt++;
		if(dy != 0)cnt++;
		if(dz != 0)cnt++;
		if (abs(dy) > 5 && cnt >= 2) {
			commands.push_back(new SMove(P(0, dy, 0)));
			dy = 0;
			cnt--;
		}
		if (abs(dx) > 5 && cnt >= 2) {
			commands.push_back(new SMove(P(dx, 0, 0)));
			dx = 0;
			cnt--;
		}
		if (abs(dz) > 5 && cnt >= 2) {
			commands.push_back(new SMove(P(0, 0, dz)));
			dz = 0;
			cnt--;
		}
		if(dz != 0 && dx != 0 && dy != 0){
			commands.push_back(new SMove(P(0,dy,0)));
			dy = 0;
		}

		pos = pp - P(dx, dy, dz);

		commands.push_back(check_move(pos, pp, P(dx, dy,dz), filled));

		commands.push_back(new Fill(coordinate(0,-1,0)));
		filled.insert(p);
		pos = pp;
	}
	{
		int dx = -pos.x;
		int dy = -pos.y;
		int dz = -pos.z;

		while (dx < -15) { commands.push_back(new SMove(P(-15,0,0))); dx += 15; }
		while (dx > 15) { commands.push_back(new SMove(P(15,0,0))); dx -= 15; }
		while (dz < -15) { commands.push_back(new SMove(P(0,0,-15))); dz += 15; }
		while (dz > 15) { commands.push_back(new SMove(P(0,0,15))); dz -= 15; }
		while (dy < -15) { commands.push_back(new SMove(P(0,-15,0))); dy += 15; }
		while (dy > 15) { commands.push_back(new SMove(P(0,15,0))); dy -= 15; }
		int cnt = 0;
		if(dx != 0)cnt++;
		if(dy != 0)cnt++;
		if(dz != 0)cnt++;
		if (abs(dy) > 5 && cnt >= 2) {
			commands.push_back(new SMove(P(0, dy, 0)));
			dy = 0;
			cnt--;
		}
		if (abs(dx) > 5 && cnt >= 2) {
			commands.push_back(new SMove(P(dx, 0, 0)));
			dx = 0;
			cnt--;
		}
		if (abs(dz) > 5 && cnt >= 2) {
			commands.push_back(new SMove(P(0, 0, dz)));
			dz = 0;
			cnt--;
		}
		if(dz != 0 && dx != 0 && dy != 0){
			commands.push_back(new SMove(P(0,dy,0)));
			dy=0;
		}
		pos = P(0,0,0) - P(dx, dy, dz);

		commands.push_back(check_move(pos, P(0,0,0), P(dx,dy,dz), filled));

	}
	commands.push_back(new Flip());
	commands.push_back(new Halt());

	dumpTrace(argv[2], commands);
}
