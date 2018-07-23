const int INF = 1<<30;

typedef Point P;

typedef vector<uint64_t> Filled;

bool is_in(P p, int R) {
	return 0 <= p.x && p.x < R && 0 <= p.y && p.y < R && 0 <= p.z && p.z < R;
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
