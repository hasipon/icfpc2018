#include "../../include/model.hpp"
#include "../../include/output.hpp"
#include <stdio.h>

struct Main : OutputBase {
	const Model& model_src, model_dst;
	Main(const Model& model_src, const Model& model_dst, std::ofstream& ofs) : OutputBase(ofs), model_src(model_src), model_dst(model_dst) {
	}
	void solve();
};

void Main::solve() {
	Halt();
}

void run(int argc, char **argv){
	Model model_src(argv[1]);
	Model model_dst(argv[2]);
	std::ofstream ofs(argv[3]);
	auto inst = new Main(model_src, model_dst, ofs);
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
