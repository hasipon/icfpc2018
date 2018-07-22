#ifndef _MODEL_H_
#define _MODEL_H_

#include <stdio.h>
#include <assert.h>
#include <string>

class Model {
public:
	int R;
	char* buf;

	Model(const char* filepath) : R(0), buf(nullptr) {
		if (filepath == std::string("void")) return;

		FILE* fp = fopen(filepath, "r");
		if (fp == nullptr) throw "Model fopen failure";
		R = fgetc(fp);
		assert(0 < R && R <= 250);
		int buf_size = (R*R*R+7)/8;
		buf = new char[buf_size];
		if (fread(buf, 1, buf_size, fp) != buf_size) {
			throw "Model fread failure";
		}
		fclose(fp);
	}

	inline bool operator ()(int x, int y, int z) const {
		if (buf == nullptr) return false;
		int p = (x * R + y) * R + z;
		return (buf[p/8] >> (p%8)) & 1;
	}
};

#endif /* _MODEL_H_ */
