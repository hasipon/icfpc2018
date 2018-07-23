struct AsmMain : OutputBase {
	const Model& Src, Tgt;
	const int R;
	int size_x = 0, size_y = 0, size_z = 0;
	bool flipped = false;
	AsmMain(const Model& Src, const Model& Tgt, std::ofstream& ofs) : OutputBase(ofs), Src(Src), Tgt(Tgt), R(max(Src.R, Tgt.R)) {}

Filled newFilled() const { return Filled((R*R*R+63)/64); }
void Fset(Filled& a, P p) const {
	int x = (p.x * R + p.y) * R + p.z;
	a[x / 64] |= 1ULL << (x % 64);
}
void Freset(Filled& a, P p) const {
	int x = (p.x * R + p.y) * R + p.z;
	a[x / 64] &= ~(1ULL << (x % 64));
}
bool Fget(const Filled& a, P p) const {
	int x = (p.x * R + p.y) * R + p.z;
	return (a[x / 64] >> (x % 64)) & 1;
}

int L5(int n) {
	return max(-5, min(5, n));
}

void moveTo(P from, P to) {
	auto d = to - from;
	{
		int cnt = 0;
		if (d.x != 0) ++cnt;
		if (d.y != 0) ++cnt;
		if (d.z != 0) ++cnt;
		if (cnt == 0) return;
		if (cnt == 3) throw "[moveTo] something wrong";
	}
	while (d.x > 15) { SMove(P(15,0,0)); d.x -= 15; }
	while (d.x < -15) { SMove(P(-15,0,0)); d.x += 15; }
	while (d.y > 15) { SMove(P(0,15,0)); d.y -= 15; }
	while (d.y < -15) { SMove(P(0,-15,0)); d.y += 15; }
	while (d.z > 15) { SMove(P(0,0,15)); d.z -= 15; }
	while (d.z < -15) { SMove(P(0,0,-15)); d.z += 15; }
	for (;;) {
		int cnt = 0;
		if (d.x != 0) ++cnt;
		if (d.y != 0) ++cnt;
		if (d.z != 0) ++cnt;
		if (cnt == 0) return;
		if (cnt == 1) { SMove(d); return; }
		P dd(L5(d.x), L5(d.y), L5(d.z));
		LMove((dd.x != 0) ? P(0,dd.y,dd.z) : P(dd.x,0,dd.z), dd);
		d -= dd;
	}
}

void do_fill(P& pos, const set<P>& goal) {
	for (auto p : goal) {
		moveTo(pos, p);
		pos = p;
		Fill(P(0,-1,0));
	}
}

void simple(const int y, const Filled& filled) {
	set<P> goal;
	for (int x = 0; x < size_x; ++ x) for (int z = 0; z < size_z; ++ z) {
		if (Fget(filled, P(x,y,z))) goal.insert(P(x,y+1,z));
	}
	P p0(size_x-1, y+1, size_z-1);
	P pos = p0;
	do_fill(pos, goal);
	moveTo(pos, p0);
}

void smart(const int y, const Filled& filled) {
	set<P> gr, not_gr, used;
	for (int x = 0; x < size_x; ++ x) for (int z = 0; z < size_z; ++ z) {
		if (Fget(filled, P(x,y,z))) (Fget(filled, P(x,y-1,z)) ? gr : not_gr).insert(P(x,y+1,z));
	}
	P p0(size_x-1, y+1, size_z-1);
	P pos = p0;
	while (!gr.empty()) {
		do_fill(pos, gr);
		used.insert(gr.begin(), gr.end());
		gr.clear();
		for (auto p : not_gr) {
			if (used.count(p+P(1,0,0)) || used.count(p+P(-1,0,0)) || used.count(p+P(0,0,1)) || used.count(p+P(0,0,-1))) {
				gr.insert(p);
			}
		}
		for (auto p : gr) not_gr.erase(p);
	}
	if (!not_gr.empty()) {
		flipped = true;
		Flip();
		do_fill(pos, not_gr);
	}
	moveTo(pos, p0);
}

void assemble() {
	auto filled = newFilled();
	for (int y = 0; y < R-1; ++ y) {
		for (int x = 1; x < R-1; ++ x) {
			for (int z = 1; z < R-1; ++ z) {
				if (Tgt(x,y,z)) {
					Fset(filled, P(x,y,z));
					size_x = max(size_x, x);
					size_y = max(size_y, y);
					size_z = max(size_z, z);
				}
			}
		}
	}
	size_x += 2; size_y += 2; size_z += 2;

	moveTo(P(0,0,0), P(size_x-1,0,size_z-1));
	for (int y = 0; y <= size_y-2; ++ y) {
		SMove(P(0,1,0));
		if (y == 0 || flipped) {
			simple(y, filled);
		} else {
			smart(y, filled);
		}
	}
	if (flipped) Flip();
	moveTo(P(size_x-1,size_y-1,size_z-1),P(0,size_y-1,0));
	moveTo(P(0,size_y-1,0),P(0,0,0));
}

void solve() {
	if (Tgt.R) assemble();
	Halt();
}

};
