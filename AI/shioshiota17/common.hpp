#include "../../include/model.hpp"
const int INF = 1<<30;

typedef Point P;

typedef vector<uint64_t> Filled;

bool is_in(P p, int R) {
	return 0 <= p.x && p.x < R && 0 <= p.y && p.y < R && 0 <= p.z && p.z < R;
}

int uf_idx(P p, int R) {
	return (p.x * R + p.y) * R + p.z;
}

bool is_near(P p1, P p2) {
	int dx = abs(p1.x - p2.x);
	int dy = abs(p1.y - p2.y);
	int dz = abs(p1.z - p2.z);
	return max(max(dx,dy),dz) == 1 && dx+dy+dz <= 2;
}

struct CacheBox {
	const int R;
	tuple<int,int,int,int,int,int> box1, box2;
	bool dirty;
	CacheBox(int R) : R(R), dirty(true) {}
	void setPos(P p) {
		int K = 5;
		dirty = false;
		box1 = make_tuple(
			max(0,   p.x - K),
			min(R-1, p.x + K),
			max(0,   p.y - K),
			min(R-1, p.y + K),
			max(0,   p.z - K),
			min(R-1, p.z + K)
		);
		box2 = make_tuple(
			max(0,   p.x - K - 1),
			min(R-1, p.x + K + 1),
			max(0,   p.y - K - 1),
			min(R-1, p.y + K + 1),
			max(0,   p.z - K - 1),
			min(R-1, p.z + K + 1)
		);
	}
	void touch(P p) {
		if (dirty) return;
		if (!check(p)) dirty = true;
	}
	bool check(P p) {
		return
			get<0>(box1) <= p.x && p.x <= get<1>(box1) &&
			get<2>(box1) <= p.y && p.y <= get<3>(box1) &&
			get<4>(box1) <= p.z && p.z <= get<5>(box1);
	}
};

class BoundingBox {
public:
	pair<int, int> x, y, z;
	BoundingBox(){}
	BoundingBox(const Model &mdl){
		int R = mdl.R;
		x = y = z = make_pair(R, 0);
		for(int i = 1; i<mdl.R-1; i++){
			for(int j = 1; j<mdl.R-1; j++){
				for(int k = 1; k<mdl.R-1; k++){
					if(mdl(i, j, k)){
						x.first = min(i-1, x.first);
						y.first = min(j-1, y.first);
						z.first = min(k-1, z.first);
						x.second = max(i+1, x.second);
						y.second = max(j+1, y.second);
						z.second = max(k+1, z.second);
					}
				}
			}
		}
		if(x.second - x.first < 10){
		    x.second =min(x.first + 10, R-1);
		    x.first = max(0, x.second - 10);
		}
		if(y.second - y.first < 10){
			y.second =min(y.first + 10, R-1);
			y.first = max(0, y.second - 10);
		}
		if(z.second - z.first < 10){
			z.second =min(z.first + 10, R-1);
			z.first = max(0, z.second - 10);
		}
	}
};
