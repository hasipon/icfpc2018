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
	void solve();
	Filled newFilled() const { return Filled((R*R*R+63)/64); }
	void set(Filled& a, P p) const {
		int x = (p.x * R + p.y) * R + p.z;
		a[x / 64] |= 1ULL << (x % 64);
	}
	bool get(const Filled& a, P p) const {
		int x = (p.x * R + p.y) * R + p.z;
		return (a[x / 64] >> (x % 64)) & 1;
	}
};

void Main::solve() {
	Halt();
}

void run(int argc, char **argv){
	Model model_src(argv[1]);
	Model model_tgt(argv[2]);
	std::ofstream ofs(argv[3]);
	auto inst = new Main(model_src, model_tgt, ofs);
	inst->solve();
}

int main(int argc, char **argv){
	try {
		run(argc, argv);
	} catch (const char* msg) {
		puts(msg);
		return 1;
	}
}
