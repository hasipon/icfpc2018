#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
#include <map>
#include <algorithm>
#include <fstream>
#include <cstdio>

using namespace std;

typedef long long energy_t;

int read_stdin() {
	int ch = fgetc(stdin);
	if (ch == EOF) throw "unexpected EOF";
	return ch;
}

struct P {
	int x,y,z;
	P(){}
	P(int x, int y, int z) : x(x), y(y), z(z) {}
};

P lld(int a, int i) {
	if (!(1 <= a && a <= 3)) throw "invalid lld a";
	if (!(0 <= i && i <= 30)) throw "invalid lld i";
	int d[3] = {0,0,0};
	d[a-1] = i - 15;
	return P(d[0], d[1], d[2]);
}
P sld(int a, int i) {
	if (!(1 <= a && a <= 3)) throw "invalid sld a";
	if (!(0 <= i && i <= 10)) throw "invalid sld i";
	int d[3] = {0,0,0};
	d[a-1] = i - 5;
	return P(d[0], d[1], d[2]);
}
P nd(int x) {
	if (!(0 <= x && x <= 26)) throw "invalid nd";
	int d[3] = {0,0,0};
	d[0] = x / 9 - 1;
	d[1] = x / 3 % 3 - 1;
	d[2] = x % 3 - 1;
	return P(d[0], d[1], d[2]);
}
P fd(int a[3]) {
	for (int i = 0; i < 3; ++ i) {
		if (!(0 <= a[i] && a[i] <= 60)) throw "invalid fd";
	}
	return P(a[0]-30, a[1]-30, a[2]-30);
}

enum Command {
	Halt,
	Wait,
	Flip,
	SMove,
	LMove,
	FusionP,
	FusionS,
	Fission,
	Fill,
	Void,
	GFill,
	GVoid,
};

struct Bot {
	Command cmd;
	P arg1, arg2;
	int arg3;
	Bot() {}
	void input() {
		int ch = read_stdin();
		if (ch == 0xFF) {
			cmd = Halt;

		} else if (ch == 0xFE) {
			cmd = Wait;

		} else if (ch == 0xFD) {
			cmd = Flip;

		} else if ((ch & 15) == 4) {
			cmd = SMove;
			int ch2 = read_stdin();
			arg1 = lld(ch >> 4, ch2);

		} else if ((ch & 15) == 12) {
			cmd = LMove;
			int ch2 = read_stdin();
			arg1 = sld((ch >> 4) & 3, ch2 & 15);
			arg2 = sld(ch >> 6, ch2 >> 4);

		} else if ((ch & 7) == 7) {
			cmd = FusionP;
			arg1 = nd(ch >> 3);

		} else if ((ch & 7) == 6) {
			cmd = FusionS;
			arg1 = nd(ch >> 3);

		} else if ((ch & 7) == 5) {
			cmd = Fission;
			int ch2 = read_stdin();
			arg1 = nd(ch >> 3);
			arg3 = ch2;

		} else if ((ch & 7) == 3) {
			cmd = Fill;
			arg1 = nd(ch >> 3);

		} else if ((ch & 7) == 2) {
			cmd = Void;
			arg1 = nd(ch >> 3);

		} else if ((ch & 7) == 1) {
			cmd = GFill;
			int a[3];
			for (int i = 0; i < 3; ++ i) a[i] = read_stdin();
			arg1 = nd(ch >> 3);
			arg2 = fd(a);

		} else if ((ch & 7) == 0) {
			cmd = GVoid;
			int a[3];
			for (int i = 0; i < 3; ++ i) a[i] = read_stdin();
			arg1 = nd(ch >> 3);
			arg2 = fd(a);

		} else {
			throw "invalid command";
		}
	}
};

struct Model {
	int R;
	Model(const char* path) : R(0) {
		FILE* fp = fopen(path, "r");
		if (fp == nullptr) return;
		R = fgetc(fp);
		if (!(0 < R && R <= 250)) throw "invalid R";
		fclose(fp);
	}
};

struct Tracer {
	int R;
	vector<Bot> bots;
	Model Src, Dst;
	Tracer(const char* src_path, const char* dst_path) : bots(1), Src(src_path), Dst(dst_path) {
		R = max(Src.R, Dst.R);
		if (R == 0) throw "problem files not found";
	}
	void run() {
		energy_t energy = 0;
		bool harmonics = false;
		for (long long t = 0; ; ++ t) {
			if (bots.empty()) throw "no bots";

			if (harmonics) {
				energy += 30 * R * R * R;
			} else {
				energy += 3 * R * R * R;
			}
			energy += bots.size() * 20;

			bool halt = false, flip = false;
			vector<unsigned> fusionS;
			for (unsigned i = 0; i < bots.size(); ++ i) {
				auto& b = bots[i];
				b.input();
				if (b.cmd == Halt) halt = true;
				else if (b.cmd == Flip) {
					if (flip) throw "invalid Flip";
					flip = true;
					harmonics = !harmonics;
				} else if (b.cmd == FusionS) {
					fusionS.push_back(i);
				}
			}
			if (halt) {
				if (bots.size() != 1) throw "invalid Halt";
				break;
			}
			for (int i = (int)fusionS.size() - 1; i >= 0; -- i) {
				bots.erase(bots.begin() + fusionS[i]);
			}
			for (unsigned i = 0, n = bots.size(); i < n; ++ i) {
				auto& b = bots[i];
				if (b.cmd == Fission) {
					bots.push_back(Bot());
				}
			}
		}
		int ch = fgetc(stdin);
		if (ch != EOF) throw "not EOF";
		cout << energy << endl;
	}
};

void run(char** argv);

int main(int argc, char** argv) {
	if (argc < 3){
		printf("%s src_file dst_file < out_file\n", argv[0]);
		return 1;
	}
	try {
		Tracer t(argv[1], argv[2]);
		t.run();
	} catch (const char* msg) {
		puts(msg);
		return 1;
	}
}
