#include "../../include/model.hpp"
#include "../../include/output.hpp"
#include <stdio.h>
#include <algorithm>

using namespace std;

struct Main : OutputBase {
	const Model& Src, Tgt;
	const int R;
	Main(const Model& Src, const Model& Tgt, std::ofstream& ofs) : OutputBase(ofs), Src(Src), Tgt(Tgt), R(max(Src.R, Tgt.R)) {}
	void solve();
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
