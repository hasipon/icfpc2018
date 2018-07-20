#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;
string lld2str(int a, int i) {
	char buf[100];
	if (!(1 <= a && a <= 3)) throw __LINE__;
	if (!(0 <= i && i <= 30)) throw __LINE__;
	int d[3] = {0,0,0};
	d[a-1] = i - 15;
	sprintf(buf, "<%d,%d,%d>", d[0], d[1], d[2]);
	return buf;
}
string sld2str(int a, int i) {
	char buf[100];
	if (!(1 <= a && a <= 3)) throw __LINE__;
	if (!(0 <= i && i <= 10)) throw __LINE__;
	int d[3] = {0,0,0};
	d[a-1] = i - 5;
	sprintf(buf, "<%d,%d,%d>", d[0], d[1], d[2]);
	return buf;
}
string nd2str(int nd) {
	char buf[100];
	if (!(0 <= nd && nd <= 26)) throw __LINE__;
	int d[3] = {0,0,0};
	d[0] = nd / 9 - 1;
	d[1] = nd / 3 % 3 - 1;
	d[2] = nd % 3 - 1;
	sprintf(buf, "<%d,%d,%d>", d[0], d[1], d[2]);
	return buf;
}
int main() {
	int ch;
	while ((ch = getchar()) != EOF) {
		if (ch == 0xFF) {
			cout << "Halt" << endl;
		} else if (ch == 0xFE) {
			cout << "Wait" << endl;
		} else if (ch == 0xFD) {
			cout << "Flip" << endl;
		} else if ((ch & 15) == 4) {
			int a = ch >> 4;
			int i = getchar();
			if (i == EOF) throw 1;
			cout << "SMove " << lld2str(a,i) << endl;
		} else if ((ch & 15) == 12) {
			int a1 = (ch >> 4) & 3;
			int a2 = ch >> 6;
			ch = getchar();
			int i1 = ch & 15;
			int i2 = ch >> 4;
			cout << "LMove " << sld2str(a1,i1) << " " << sld2str(a2,i2) << endl;
		} else if ((ch & 7) == 7) {
			int nd = ch >> 3;
			cout << "FusionP " << nd2str(nd) << endl;
		} else if ((ch & 7) == 6) {
			int nd = ch >> 3;
			cout << "FusionS " << nd2str(nd) << endl;
		} else if ((ch & 7) == 5) {
			int nd = ch >> 3;
			int m = getchar();
			if (m == EOF) throw 1;
			cout << "Fission " << nd2str(nd) << " " << m << endl;
		} else if ((ch & 7) == 3) {
			int nd = ch >> 3;
			cout << "Fill " << nd2str(nd) << endl;
		} else {
			throw 1;
		}
	}
}
