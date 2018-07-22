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
typedef tuple<int,int,int> triple_t;

triple_t add(triple_t x, triple_t y) {
	return make_tuple(get<0>(x) + get<0>(y), get<1>(x) + get<1>(y), get<2>(x) + get<2>(y));
}
triple_t lld(int a, int i) {
	if (!(1 <= a && a <= 3)) throw "invalid lld a";
	if (!(0 <= i && i <= 30)) throw "invalid lld i";
	int d[3] = {0,0,0};
	d[a-1] = i - 15;
	return make_tuple(d[0], d[1], d[2]);
}
triple_t sld(int a, int i) {
	if (!(1 <= a && a <= 3)) throw "invalid sld a";
	if (!(0 <= i && i <= 10)) throw "invalid sld i";
	int d[3] = {0,0,0};
	d[a-1] = i - 5;
	return make_tuple(d[0], d[1], d[2]);
}
triple_t nd(int x) {
	if (!(0 <= x && x <= 26)) throw "invalid nd";
	int d[3] = {0,0,0};
	d[0] = x / 9 - 1;
	d[1] = x / 3 % 3 - 1;
	d[2] = x % 3 - 1;
	return make_tuple(d[0], d[1], d[2]);
}
triple_t fd(int a[3]) {
	for (int i = 0; i < 3; ++ i) {
		if (!(0 <= a[i] && a[i] <= 60)) throw "invalid fd";
	}
	return make_tuple(a[0]-30, a[1]-30, a[2]-30);
}
string triple2str(triple_t x) {
	char buf[100];
	sprintf(buf, "<%d,%d,%d>", get<0>(x), get<1>(x), get<2>(x));
	return buf;
}
int mlen(triple_t x) {
	return abs(get<0>(x)) + abs(get<1>(x)) + abs(get<2>(x));
}

void run(char** argv);

int main(int argc, char** argv) {
	if (argc < 4){
		printf("%s R trace_file out_file\n", argv[0]);
		return 0;
	}
	try {
		run(argv);
	} catch (const char* msg) {
		puts(msg);
		return 1;
	}
}

void run(char** argv) {
	FILE *trace = fopen(argv[2], "rb");

	ofstream ofs(argv[3]);

	vector<pair<int,pair<int64_t,triple_t>>> bots, bots_next;
	map<triple_t,pair<int,int>> fusionP;
	map<triple_t,int> fusionS;
	long long t = 0;
	int idx = 0;

	bots.push_back({0, {(1LL<<40)-2, make_tuple(0,0,0)}});

	int ch;
	while ((ch = fgetc(trace)) != EOF) {
		int bid = bots[idx].first;
		if (ch == 0xFF) {
			ofs << "Halt";
		} else if (ch == 0xFE) {
			ofs << "Wait";
			bots_next.push_back(bots[idx]);
		} else if (ch == 0xFD) {
			ofs << "Flip";
			bots_next.push_back(bots[idx]);
		} else if ((ch & 15) == 4) {
			int a = ch >> 4;
			int i = fgetc(trace);
			if (i == EOF) throw "unexpected EOF SMove";
			auto d = lld(a,i);
			ofs << "SMove " << triple2str(d);
			bots_next.push_back(bots[idx]);
			auto& pos = bots_next.back().second.second;
			pos = add(pos, d);
			ofs << " # c' = " << triple2str(pos);
		} else if ((ch & 15) == 12) {
			int a1 = (ch >> 4) & 3;
			int a2 = ch >> 6;
			ch = fgetc(trace);
			if (ch == EOF) throw "unexpected EOF LMove";
			int i1 = ch & 15;
			int i2 = ch >> 4;
			auto d1 = sld(a1,i1);
			auto d2 = sld(a2,i2);
			ofs << "LMove " << triple2str(d1) << " " << triple2str(d2);
			bots_next.push_back(bots[idx]);
			auto& pos = bots_next.back().second.second;
			pos = add(pos, add(d1,d2));
			ofs << " # c' = " << triple2str(pos);
		} else if ((ch & 7) == 7) {
			auto d = nd(ch >> 3);
			ofs << "FusionP " << triple2str(d);
			fusionP[bots[idx].second.second] = {bots[idx].first, bots[idx].second.first};
		} else if ((ch & 7) == 6) {
			auto d = nd(ch >> 3);
			ofs << "FusionS " << triple2str(d);
			fusionS[add(bots[idx].second.second, d)] = (1 << bots[idx].first) | bots[idx].second.first;
		} else if ((ch & 7) == 5) {
			auto d = nd(ch >> 3);
			int m = fgetc(trace);
			if (m == EOF) throw "unexpected EOF Fission";
			ofs << "Fission " << triple2str(d) << " " << m;
			int seed1 = bots[idx].second.first;
			if (seed1 == 0) throw "invalid seed Fission";
			int bid1 = bots[idx].first;
			int bid2;
			for (int i = 0; ; ++ i) {
				if ((seed1>>i)&1) {
					bid2 = i;
					seed1 &= ~(1<<i);
					break;
				}
			}
			int seed2 = 0;
			for (int i = 0; m > 0 && i < 20; ++ i) {
				if ((seed1>>i)&1) {
					seed1 &= ~(1<<i);
					seed2 |= 1<<i;
					-- m;
				}
			}
			if (m > 0) throw "invalid m Fission";
			auto pos1 = bots[idx].second.second;
			auto pos2 = add(pos1, d);
			bots_next.push_back({bid1, {seed1, pos1}});
			bots_next.push_back({bid2, {seed2, pos2}});
			ofs << " # bot'.bid = " << bid2;
			ofs << " bot'.pos = " << triple2str(pos2);
		} else if ((ch & 7) == 2) {
			auto d = nd(ch >> 3);
			ofs << "Void " << triple2str(d);
			bots_next.push_back(bots[idx]);
		} else if ((ch & 7) == 3) {
			auto d = nd(ch >> 3);
			ofs << "Fill " << triple2str(d);
			bots_next.push_back(bots[idx]);
		} else if ((ch & 7) == 1) {
			auto d = nd(ch >> 3);
			int a[3];
			for (int i = 0; i < 3; ++ i) {
				a[i] = fgetc(trace);
				if (a[i] == EOF) throw "unexpected EOF GFill";
			}
			ofs << "GFill " << triple2str(d) << " " << triple2str(fd(a));
		} else if ((ch & 7) == 0) {
			auto d = nd(ch >> 3);
			int a[3];
			for (int i = 0; i < 3; ++ i) {
				a[i] = fgetc(trace);
				if (a[i] == EOF) throw "unexpected EOF GVoid";
			}
			ofs << "GVoid " << triple2str(d) << " " << triple2str(fd(a));
		} else {
			throw "invalid command";
		}

		ofs << " # bid=" << bid+1;
		++ idx;
		if (idx == (int)bots.size()) {
			idx = 0;
			bots = bots_next;
			bots_next.clear();
			for (auto p : fusionP) {
				if (!fusionS.count(p.first)) throw "invalid fusion";
				int seed = p.second.second | fusionS[p.first];
				bots.push_back({p.second.first, {seed, p.first}});
			}
			fusionP.clear();
			fusionS.clear();
			sort(bots.begin(), bots.end());
			++ t;
			ofs << " t=" << t;
		}
		ofs << endl;
	}
}
