#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <tuple>
using namespace std;
typedef long long energy_t;
typedef tuple<int,int,int> triple_t;

triple_t add(triple_t x, triple_t y) {
	return make_tuple(get<0>(x) + get<0>(y), get<1>(x) + get<1>(y), get<2>(x) + get<2>(y));
}
triple_t lld(int a, int i) {
	if (!(1 <= a && a <= 3)) throw 1;
	if (!(0 <= i && i <= 30)) throw 1;
	int d[3] = {0,0,0};
	d[a-1] = i - 15;
	return make_tuple(d[0], d[1], d[2]);
}
triple_t sld(int a, int i) {
	if (!(1 <= a && a <= 3)) throw 1;
	if (!(0 <= i && i <= 10)) throw 1;
	int d[3] = {0,0,0};
	d[a-1] = i - 5;
	return make_tuple(d[0], d[1], d[2]);
}
triple_t nd(int x) {
	if (!(0 <= x && x <= 26)) throw 1;
	int d[3] = {0,0,0};
	d[0] = x / 9 - 1;
	d[1] = x / 3 % 3 - 1;
	d[2] = x % 3 - 1;
	return make_tuple(d[0], d[1], d[2]);
}
string triple2str(triple_t x) {
	char buf[100];
	sprintf(buf, "<%d,%d,%d>", get<0>(x), get<1>(x), get<2>(x));
	return buf;
}
int mlen(triple_t x) {
	return abs(get<0>(x)) + abs(get<1>(x)) + abs(get<2>(x));
}

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("%s R", argv[0]);
		return 0;
	}

	int R = atoi(argv[1]);

	energy_t energy = 0;
	bool harmonics = false;
	vector<pair<int,pair<int,triple_t>>> bots, bots_next;
	long long t = 0;
	int idx = 0;

	bots.push_back({0, {(1<<20)-1, make_tuple(0,0,0)}});

	int ch;
	while ((ch = getchar()) != EOF) {
		if (idx == 0) {
			if (harmonics) {
				energy += 30 * R * R * R;
			} else {
				energy += 3 * R * R * R;
			}
			energy += bots.size() * 20;
		}
	
		int bid = bots[idx].first;
		if (ch == 0xFF) {
			cout << "Halt";
		} else if (ch == 0xFE) {
			cout << "Wait";
			bots_next.push_back(bots[idx]);
		} else if (ch == 0xFD) {
			cout << "Flip";
			bots_next.push_back(bots[idx]);
			harmonics = !harmonics;
		} else if ((ch & 15) == 4) {
			int a = ch >> 4;
			int i = getchar();
			if (i == EOF) throw 1;
			auto d = lld(a,i);
			cout << "SMove " << triple2str(d);
			bots_next.push_back(bots[idx]);
			auto& pos = bots_next.back().second.second;
			pos = add(pos, d);
			cout << " # c' = " << triple2str(pos);
			energy += 2 * mlen(d);
		} else if ((ch & 15) == 12) {
			int a1 = (ch >> 4) & 3;
			int a2 = ch >> 6;
			ch = getchar();
			int i1 = ch & 15;
			int i2 = ch >> 4;
			auto d1 = sld(a1,i1);
			auto d2 = sld(a2,i2);
			cout << "LMove " << triple2str(d1) << " " << triple2str(d2);
			bots_next.push_back(bots[idx]);
			auto& pos = bots_next.back().second.second;
			pos = add(pos, add(d1,d2));
			cout << " # c' = " << triple2str(pos);
			energy += 2 * (mlen(d1) + 2 + mlen(d2));
		} else if ((ch & 7) == 7) {
			auto d = nd(ch >> 3);
			cout << "FusionP " << triple2str(d);
			bots_next.push_back(bots[idx]);
		} else if ((ch & 7) == 6) {
			auto d = nd(ch >> 3);
			cout << "FusionS " << triple2str(d);
			bots_next.push_back(bots[idx]);
		} else if ((ch & 7) == 5) {
			auto d = nd(ch >> 3);
			int m = getchar();
			if (m == EOF) throw 1;
			cout << "Fission " << triple2str(d) << " " << m;
			bots_next.push_back(bots[idx]);
		} else if ((ch & 7) == 3) {
			auto d = nd(ch >> 3);
			cout << "Fill " << triple2str(d);
			bots_next.push_back(bots[idx]);
			energy += 12;
		} else {
			throw 1;
		}

		cout << " # bid=" << bid+1;
		++ idx;
		if (idx == (int)bots.size()) {
			idx = 0;
			bots = bots_next;
			bots_next.clear();
			++ t;
			cout << " t=" << t << " energy=" << energy;
		}
		cout << endl;
	}
}
