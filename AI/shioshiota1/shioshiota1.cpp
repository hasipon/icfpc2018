#include <vector>
#include "../../include/TraceDumper.hpp"
#include "../../include/model.hpp"

typedef coordinate P;

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
		if (dy != 0) commands.push_back(new SMove(P(0,dy,0)));

		while (dx < -15) { commands.push_back(new SMove(P(-15,0,0))); dx += 15; }
		while (dx > 15) { commands.push_back(new SMove(P(15,0,0))); dx -= 15; }
		if (dx != 0) commands.push_back(new SMove(P(dx,0,0)));

		while (dz < -15) { commands.push_back(new SMove(P(0,0,-15))); dz += 15; }
		while (dz > 15) { commands.push_back(new SMove(P(0,0,15))); dz -= 15; }
		if (dz != 0) commands.push_back(new SMove(P(0,0,dz)));

		commands.push_back(new Fill(coordinate(0,-1,0)));
		pos = pp;
	}
	{
		int dx = -pos.x;
		int dy = -pos.y;
		int dz = -pos.z;

		while (dx < -15) { commands.push_back(new SMove(P(-15,0,0))); dx += 15; }
		while (dx > 15) { commands.push_back(new SMove(P(15,0,0))); dx -= 15; }
		if (dx != 0) commands.push_back(new SMove(P(dx,0,0)));

		while (dz < -15) { commands.push_back(new SMove(P(0,0,-15))); dz += 15; }
		while (dz > 15) { commands.push_back(new SMove(P(0,0,15))); dz -= 15; }
		if (dz != 0) commands.push_back(new SMove(P(0,0,dz)));

		while (dy < -15) { commands.push_back(new SMove(P(0,-15,0))); dy += 15; }
		while (dy > 15) { commands.push_back(new SMove(P(0,15,0))); dy -= 15; }
		if (dy != 0) commands.push_back(new SMove(P(0,dy,0)));
	}
	commands.push_back(new Flip());
	commands.push_back(new Halt());

	dumpTrace(argv[2], commands);
}
