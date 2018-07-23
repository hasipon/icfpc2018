struct DisMain : OutputBase {
	const Model& Src, Tgt;
	const int R;
	bool flipped = false;
	DisMain(const Model& Src, const Model& Tgt, std::ofstream& ofs) : OutputBase(ofs), Src(Src), Tgt(Tgt), R(max(Src.R, Tgt.R)) {}

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

void move(P& pos, const P& to) {
	if (pos == to) return;

	auto d = to - pos;
	int cnt = 0;
	if (d.x != 0) ++cnt;
	if (d.y != 0) ++cnt;
	if (d.z != 0) ++cnt;
	if (cnt != 1) throw "move failure";

	while (d.x > 15) { SMove(P(15,0,0)); d.x -= 15; }
	while (d.x < -15) { SMove(P(-15,0,0)); d.x += 15; }
	while (d.y > 15) { SMove(P(0,15,0)); d.y -= 15; }
	while (d.y < -15) { SMove(P(0,-15,0)); d.y += 15; }
	while (d.z > 15) { SMove(P(0,0,15)); d.z -= 15; }
	while (d.z < -15) { SMove(P(0,0,-15)); d.z += 15; }
	if (d != P(0,0,0)) SMove(d);

	pos = to;
}

tuple<int,int,int,P> fission(tuple<int,int,int,P>& bot, int cap, P nd) {
	int bid = get<0>(bot);
	int s1 = get<1>(bot);
	int s2 = get<2>(bot);
	P pos = get<3>(bot);
	bot = make_tuple(bid, s2-cap+2, s2, pos);
	return make_tuple(s1, s1+1, s2-cap+1, pos+nd);
}

int sign(int n) {
	if (n < 0) return -1;
	if (n > 0) return +1;
	return 0;
}

// [ first : { true: Void, false: SMove }, second : arg ]
vector<pair<bool,P>> make_plan(P s, P t, Filled& filled) {
	vector<pair<bool,P>> res;
	int dx = sign(t.x - s.x);
	int dy = sign(t.y - s.y);
	int dz = sign(t.z - s.z);
	P d(dx,dy,dz);
	P pos = s, pos0 = s;
	int mlen = 0;
	do {
		auto prev = pos;
		pos += d;
		if (Fget(filled, pos)) {
			if (prev != pos0) res.push_back({false, prev-pos0});
			mlen = 0;
			pos0 = prev;
			res.push_back({true,d});
			Freset(filled, pos);
		} else if (mlen == 15) {
			res.push_back({false, prev-pos0});
			mlen = 0;
			pos0 = prev;
		}
		++ mlen;
	} while (pos != t);
	res.push_back({false, t-pos0});
	//cerr << "plan" << s << t << endl;
	//for (auto p : res) cerr << p.first << p.second << endl;
	return res;
}

void FGVoid(Filled& filled, P p, P nd, P fd) {
	auto p1 = p + nd;
	auto p2 = p1 + fd;
	int x1 = p1.x, x2 = p2.x;
	int y1 = p1.y, y2 = p2.y;
	int z1 = p1.z, z2 = p2.z;
	if (x1 > x2) swap(x1, x2);
	if (y1 > y2) swap(y1, y2);
	if (z1 > z2) swap(z1, z2);
	for (int x = x1; x <= x2; ++ x)
	for (int y = y1; y <= y2; ++ y)
	for (int z = z1; z <= z2; ++ z) {
		Freset(filled,P(x,y,z));
	}
}

void mmove(vector<P>& bpos, Filled& filled, const vector<int>& xs, const vector<int>& ys, const vector<int>& zs, bool gvoid) {
	if (bpos.size() != xs.size() * ys.size() * zs.size()) {
		cerr << bpos.size() << endl;
		cerr << xs.size() * ys.size() * zs.size() << endl;
		throw "[mmove] invalid input";
	}

	const int n = bpos.size();
	vector<bool> moved(n);
	set<P> target_s;
	for (auto x : xs) for (auto y : ys) for (auto z : zs) {
		P p(x,y,z);
		auto it = find(bpos.begin(), bpos.end(), p);
		if (it == bpos.end()) target_s.insert(p);
		else moved[it - bpos.begin()] = true;
	}
	if (target_s.empty()) return;

	vector<vector<pair<bool,P>>> plan(n);
	for (int i = 0; i < n; ++ i) if (!moved[i]) {
		bool ok = false;
		for (auto t : target_s) {
			int cnt = 0;
			if (t.x != bpos[i].x) ++ cnt;
			if (t.y != bpos[i].y) ++ cnt;
			if (t.z != bpos[i].z) ++ cnt;
			if (cnt == 1) {
				target_s.erase(t);
				ok = true;
				plan[i] = make_plan(bpos[i], t, filled);
				bpos[i] = t;
				break;
			}
		}
		if (!ok) {
			cerr << "bpos[i]=" << bpos[i] << endl;
			cerr << "target_s=" << endl;
			for (auto p : target_s) cerr << p << endl;
			throw "[mmove] target not found";
		}
	}
	if (!target_s.empty()) throw "[mmove] target_s must be empty";
	unsigned len_plan = 0;
	for (auto& a : plan) if (a.size() > len_plan) len_plan = a.size();
	mflip(n, filled);
	for (unsigned t = 0; t < len_plan; ++ t) {
		for (int i = 0; i < n; ++ i) {
			if (t < plan[i].size()) {
				if (plan[i][t].first) {
					Void(plan[i][t].second);
				} else {
					SMove(plan[i][t].second);
				}
			} else Wait();
		}
	}

	if(gvoid) {
		map<P, int> a[2][2], b[2][2], c[2][2], d[2][2];
		for (int base = 0; base + 1 < int(zs.size()); base+=2) {
			for (int i = 0; i < 2; i++) {
			    int z = zs[base + (i + 1) %2] - zs[base + i%2];
				a[0][i].insert({{P(xs[0], ys[0], zs[base + i]), z}});
				b[0][i].insert({{P(xs[0], ys[1], zs[base + i]),z}});
				c[0][i].insert({{P(xs[1], ys[0], zs[base + i]),z}});
				d[0][i].insert({{P(xs[1], ys[1], zs[base + i]),z}});

				if (base + 1 + 1 < int(zs.size())) {
					int z = zs[base + 1 + (i + 1) %2] - zs[base + 1+ i%2];
					a[1][i].insert({{P(xs[0], ys[0], zs[base + i + 1]), z}});
					b[1][i].insert({{P(xs[0], ys[1], zs[base + i + 1]), z}});
					c[1][i].insert({{P(xs[1], ys[0], zs[base + i + 1]), z}});
					d[1][i].insert({{P(xs[1], ys[1], zs[base + i + 1]), z}});
				}
			}
		}
		vector<tuple<bool, P, P>> cmd;
		for(int base = 0; base < 2; base++) {
			int cnt = 0;
			bool skipBase =false;
			for(int i = 0; i<2;i++){
				if(a[base][i].size() == 0){
					skipBase = true;
				}
			}
			if(skipBase)continue;
			for (auto p : bpos) {
				bool commanded = false;
				for (int i = 0; i < 2; i++) {
					int x = abs(xs[0] - xs[1]) - 2;
					int y = abs(ys[0] - ys[1]);
					int local = 0;
					if (a[base][i].count(p)) {
						if(a[base][i][p] == 0 && i ==1)continue;
						P nd = P(1, 0, 0);
						P fd = P(x, y, a[base][i][p]);
						cmd.push_back(make_tuple(true,nd,fd)); // GVoid(nd, fd);
						FGVoid(filled, p, nd, fd);
						local++;
						cnt++;
					}
					if (b[base][i].count(p) && y!=0){
						if(b[base][i][p] == 0 && i ==1)continue;
						P nd = P(1, 0, 0);
						P fd = P(x, -y, b[base][i][p]);
						cmd.push_back(make_tuple(true,nd,fd)); // GVoid(nd, fd);
						local++;
						cnt++;
					}
					if (c[base][i].count(p) && x!=0) {
						if(c[base][i][p] == 0 && i ==1)continue;
						P nd = P(-1, 0, 0);
						P fd = P(-x, y, c[base][i][p]);
						cmd.push_back(make_tuple(true,nd,fd)); // GVoid(nd, fd);
						local++;
						cnt++;
					}
					if (d[base][i].count(p) && x !=0 && y!=0) {
						if(d[base][i][p] == 0 && i ==1)continue;
						P nd = P(-1, 0, 0);
						P fd = P(-x, -y, d[base][i][p]);
						cmd.push_back(make_tuple(true,nd,fd)); // GVoid(nd, fd);
						local++;
						cnt++;
					}
					if(local == 1){
						commanded = true;
						break;
					}
					if(local > 1){
						throw "INVALID GVOID";
					}
				}
				if (!commanded) {
					cmd.push_back(make_tuple(false,P(),P())); // Wait();
				}
			}
			mflip(n, filled);
			for (auto c : cmd) {
				if (get<0>(c)) {
					GVoid(get<1>(c),get<2>(c));
				} else {
					Wait();
				}
			}
		}
	}
}

int uf_idx(P p) const {
	return (p.x * R + p.y) * R + p.z;
}

bool isGrounded(const Filled& filled) {
	int n = 0;
	UnionFind uf(R*R*R+1);
	for (int x = 0; x < R; ++ x) for (int y = 0; y < R; ++ y) for (int z = 0; z < R; ++ z) {
		P p(x,y,z);
		if (Fget(filled, p)) {
			if (y == 0) uf.unionSet(uf_idx(P(x,0,z)), R*R*R);
			++ n;
			if (x+1 < R) {
				P pp(x+1,y,z);
				if (Fget(filled, pp)) uf.unionSet(uf_idx(p),uf_idx(pp));
			}
			if (y+1 < R) {
				P pp(x,y+1,z);
				if (Fget(filled, pp)) uf.unionSet(uf_idx(p),uf_idx(pp));
			}
			if (z+1 < R) {
				P pp(x,y,z+1);
				if (Fget(filled, pp)) uf.unionSet(uf_idx(p),uf_idx(pp));
			}
		}
	}
	//cerr << "isGrounded" << endl;
	//cerr << "n=" << n << endl;
	//cerr << "uf.size=" << uf.size(R*R*R) << endl;
	return uf.size(R*R*R) == n+1;
}

void mflip(int nbot, const Filled& filled) {
	if (flipped || isGrounded(filled)) return;
	flipped = true;
	//cerr << "Flip !!!" << endl;
	for (int j = 0; j < nbot; ++ j) {
		if (j == 0) Flip(); else Wait();
	}
}

void disassemble() {
	auto filled = newFilled();
	int size_x = 0, size_y = 0, size_z = 0;
	for (int y = 0; y < R-1; ++ y) {
		for (int x = 1; x < R-1; ++ x) {
			for (int z = 1; z < R-1; ++ z) {
				if (Src(x,y,z)) {
					Fset(filled, P(x,y,z));
					size_x = max(size_x, x);
					size_y = max(size_y, y);
					size_z = max(size_z, z);
				}
			}
		}
	}
	size_x += 2; size_y += 2; size_z += 2;
	size_x = max(10, size_x);
	size_y = max(10, size_y);
	size_z = max(10, size_z);

	vector<int> zs;
	for (int i = 0; i < size_z-1; i += 30) zs.push_back(i);
	zs.push_back(size_z-1);

	vector<tuple<int,int,int,P>> bots { make_tuple(0,1,39,P(0,0,0)) };
	for (unsigned i = 1; i < zs.size(); ++ i) {
		int nbot = bots.size();
		for (int j = 0; j < nbot; ++ j) {
			if (j == nbot-1) {
				auto nd = P(0,0,1);
				auto r = fission(bots[j], 4, nd);
				bots.push_back(r);
				Fission(nd, get<2>(r) - get<1>(r) + 1);
			}
			else Wait();
		}
	}
	for (;;) {
		int bidx = -1;
		int nbot = bots.size();
		for (int i = nbot-1; i >= 0; -- i) {
			if (get<3>(bots[i]).z != zs[i]) {
				bidx = i;
				break;
			}
		}
		if (bidx < 0) break;
		for (int i = 0; i < nbot; ++ i) {
			if (i == bidx) {
				auto& pos = get<3>(bots[i]);
				auto d = P(0,0,min(15, zs[i] - pos.z));
				SMove(d);
				pos += d;
			} else Wait();
		}
	}
	{
		int nbot = bots.size();
		for (int j = 0; j < nbot; ++ j) {
			auto nd = P(0,1,0);
			auto r = fission(bots[j], 2, nd);
			bots.push_back(r);
			Fission(nd, get<2>(r) - get<1>(r) + 1);
		}
		sort(bots.begin(), bots.end());
	}
	vector<P> bpos;
	for (auto b : bots) bpos.push_back(get<3>(b));
	vector<int> xs = {0};
	vector<int> ys = {0, size_y-1};
	mmove(bpos, filled, xs, ys, zs, false);
	//cerr << "bpos" << endl; for (auto p : bpos) cerr << p << endl;
	ys[0] = max(0, size_y-1-30);
	mmove(bpos, filled, xs, ys, zs, false);
	//cerr << "bpos" << endl; for (auto p : bpos) cerr << p << endl;
	{
		int nbot = bots.size();
		for (int j = 0; j < nbot; ++ j) {
			get<3>(bots[j]) = bpos[j];
		}
	}
	/*{
		int nbot = bots.size();
		for (int j = 0; j < nbot; ++ j) {
			if (j == 0) Flip(); else Wait();
		}
	}*/
	{
		int nbot = bots.size();
		vector<bool> b(nbot);
		bool needVoid = false;
		for (int j = 0; j < nbot; ++ j) {
			auto p = get<3>(bots[j]) + P(1,0,0);
			b[j] = Fget(filled, p);
			if (b[j]) {
				Freset(filled, p);
				needVoid = true;
			}
		}
		if (needVoid) {
			mflip(nbot, filled);
			for (int j = 0; j < nbot; ++ j) {
				if (b[j]) Void(P(1,0,0)); else Wait();
			}
		}
	}
	{
		int nbot = bots.size();
		for (int j = 0; j < nbot; ++ j) {
			auto nd = P(1,0,0);
			auto r = fission(bots[j], 1, nd);
			bots.push_back(r);
			Fission(nd, get<2>(r) - get<1>(r) + 1);
		}
		sort(bots.begin(), bots.end());
	}
	bpos.clear();
	for (auto b : bots) bpos.push_back(get<3>(b));
	//cerr << "bpos" << endl; for (auto p : bpos) cerr << p << endl;
	xs.push_back(min(30, size_x-1));
	mmove(bpos, filled, xs, ys, zs, true);
	int state = 0;
	for (;;) {
		//cerr << "state = " << state << endl;
		if (state == 0) { // x plus
			if (xs[1] == size_x-1) {
				state = 2;
			} else {
				xs[1] = min(size_x-1, xs[1] + 29);
				mmove(bpos, filled, xs, ys, zs, false);
				xs[0] += 29;
				mmove(bpos, filled, xs, ys, zs, true);
			}
		} else if (state == 2) { // x reset
			if (ys[0] == 0) break;
			if (xs[0] != 0) {
				xs[0] = 0;
				mmove(bpos, filled, xs, ys, zs, false);
			}
			if (xs[1] != min(30,size_x-1)) {
				xs[1] = min(30,size_x-1);
				mmove(bpos, filled, xs, ys, zs, false);
			}
			state = 3;
		} else if (state == 3) {
			ys[0] = max(0, ys[0] - 30);
			mmove(bpos, filled, xs, ys, zs, false);
			ys[1] -= 30;
			mmove(bpos, filled, xs, ys, zs, true);
			state = 0;
		} else {
			throw "[disassemble] invalid state";
		}
	}
	if (flipped) {
		int nbot = bots.size();
		for (int j = 0; j < nbot; ++ j) {
			if (j == 0) Flip(); else Wait();
		}
	}
	auto emptyFilled = newFilled();
	if (xs[0] != 0) { xs[0] = 0; mmove(bpos, emptyFilled, xs, ys, zs, false); }
	if (xs[1] != 1) { xs[1] = 1; mmove(bpos, emptyFilled, xs, ys, zs, false); }
	if (ys[0] != 0) { ys[0] = 0; mmove(bpos, emptyFilled, xs, ys, zs, false); }
	if (ys[1] != 1) { ys[1] = 1; mmove(bpos, emptyFilled, xs, ys, zs, false); }
	for (int i = 0; i < (int)zs.size(); ++ i) {
		if (zs[i] != i) { zs[i] = i; mmove(bpos, emptyFilled, xs, ys, zs, false); }
	}
	{
		vector<P> a;
		for (auto p : bpos) {
			if (p.x == 0) {
				FusionP(P(1,0,0));
				a.push_back(p);
			} else {
				FusionS(P(-1,0,0));
			}
		}
		//for (auto p : a) cerr << p << endl;
		bpos.swap(a);
	}
	{
		vector<P> a;
		for (auto p : bpos) {
			if (p.y == 0) {
				FusionP(P(0,1,0));
				a.push_back(p);
			} else {
				FusionS(P(0,-1,0));
			}
		}
		//for (auto p : a) cerr << p << endl;
		bpos.swap(a);
	}
	{
		for (int n = bpos.size(); n > 1; -- n) {
			for (int i = 0; i < n; ++ i) {
				if (i == n-2) {
					FusionP(P(0,0,1));
				} else if (i == n-1) {
					FusionS(P(0,0,-1));
				} else Wait();
			}
		}
	}
}

void solve() {
	if (Src.R) disassemble();
}

}; // struct DisMain end
