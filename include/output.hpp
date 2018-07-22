#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include "point.hpp"
#include <fstream>

int8_t to_nd(const Point &c) {
	int dx = c.x;
	int dy = c.y;
	int dz = c.z;
	assert(-1 <= dx && dx <= 1);
	assert(-1 <= dy && dy <= 1);
	assert(-1 <= dz && dz <= 1);
	return (dx + 1) * 9 + (dy + 1) * 3 + (dz + 1);
}

struct OutputBase {
	std::ofstream& ofs;

	OutputBase(std::ofstream& ofs) : ofs(ofs) {}

	void Halt() {
		const char buf[1] = {(char)0b11111111};
		ofs.write(buf, 1);
	}
	void Wait(){
		const char buf[1] = {(char)0b11111110};
		ofs.write(buf, 1);
	}
	void Flip(){
		const char buf[1] = {(char)0b11111101};
		ofs.write(buf, 1);
	}
	void SMove(Point to){

	}
	void LMove(Point via, Point to){

	}

	void FusionP(Point with){

	}
	void FusionS(Point with){

	}
	void Fission(Point to, int8_t m){

	}
	void Fill(Point to){

	}
	void Void(Point to){

	}
	void GFill(Point vertex, Point region){

	}
	void GVoid(Point vertex, Point region){

	}


};

#endif /* _OUTPUT_H_ */
