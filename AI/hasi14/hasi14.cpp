#include "../../include/point.hpp"
#include "../../include/model.hpp"
#include "../../include/output.hpp"
#include "../../include/union_find.hpp"
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <unordered_map>
#include <time.h>

using namespace std;

#include "common.hpp"
#include "asm.hpp"
#include "dis.hpp"

void run(char **argv){
	Model model_src(argv[1]);
	Model model_tgt(argv[2]);
	std::ofstream ofs(argv[3]);
	auto inst1 = new DisMain(model_src, model_tgt, ofs);
	inst1->solve();
	auto inst2 = new AsmMain(model_src, model_tgt, ofs);
	inst2->solve();
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
