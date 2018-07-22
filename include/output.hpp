#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include <fstream>

struct OutputBase {
	std::ofstream& ofs;

	OutputBase(std::ofstream& ofs) : ofs(ofs) {}

	void Halt() {
		const char buf[1] = {(char)0b11111111};
		ofs.write(buf, 1);
	}
};

#endif /* _OUTPUT_H_ */
