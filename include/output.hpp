#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include "point.hpp"
#include <fstream>

using namespace std;

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
	enum DIR{
		DIR_X = 0b01,
		DIR_Y = 0b10,
		DIR_Z = 0b11
	};
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
	void SMove(const Point &to){
	    auto dir_dist = PtoDIR(to);
	    DIR dir = dir_dist.first;
	    int8_t dist = dir_dist.second;
		const char buf[2] = {(char)((dir << 4) | 0b0100), (char)(0b11111 & (dist+15))};
		ofs.write(buf, 2);
	}

	void LMove(Point via, Point to){
		auto dir_dist1 = PtoDIR(via), dir_dist2 = PtoDIR(to);
		DIR dir1 = dir_dist1.first;
		int8_t dist1 = dir_dist1.second;
		DIR dir2 = dir_dist2.first;
		int8_t dist2 = dir_dist2.second;
		const char buf[2] = {(char)((dir2 << 6) | (dir1 << 4) | 0b1100), (char)(((dist2 + 5) << 4) | (dist1 + 5))};
		ofs.write(buf, 2);
	}

	void FusionP(Point with){
		int8_t nd = to_nd(with);
		const char buf[1] = {(char)((nd << 3) | 0b111)};
		ofs.write(buf, 1);
	}
	void FusionS(Point with){
		int8_t nd = to_nd(with);
		const char buf[1] = {(char)((nd << 3) | 0b110)};
		ofs.write(buf, 1);
	}
	void Fission(Point to, int8_t m){
		int8_t nd = to_nd(to);
		const char buf[2] = {(char)((nd << 3) | 0b101), (char)m};
		ofs.write(buf, 2);
	}
	void Fill(Point to){
		int8_t nd = to_nd(to);
		const char buf[1] = {(char)((nd << 3) | 0b011)};
		ofs.write(buf, 1);
	}
	void Void(Point to){
		int8_t nd = to_nd(to);
		const char buf[1] = {(char)((nd << 3) | 0b010)};
		ofs.write(buf, 1);

	}
	void GFill(Point vertex, Point region){
    region += Point(30, 30, 30);
		int8_t nd = to_nd(vertex);
		const char buf[4] = {(char)((nd << 3) | 0b001), char(region.x), char(region.y), char(region.z)};
		ofs.write(buf, 4);
	}
	void GVoid(Point vertex, Point region){
		int8_t nd = to_nd(vertex);
		const char buf[4] = {(char)((nd << 3) | 0b000), char(region.x), char(region.y), char(region.z)};
		ofs.write(buf, 4);
	}

	pair<DIR, int8_t> PtoDIR(const Point &p){
		DIR dir;
		int8_t dist;
		if(p.x != 0){
			dir = DIR_X;
			dist = p.x;
		}
		else if(p.y != 0){
			dir = DIR_Y;
			dist = p.y;
		}
		else if(p.z != 0){
			dir = DIR_Z;
			dist = p.z;
		}else{
		    assert(false);
		}
		return make_pair(dir, dist);
	}


};


#endif /* _OUTPUT_H_ */
