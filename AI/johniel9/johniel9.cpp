#include "../../include/model.hpp"
#include "../../include/output.hpp"
#include "../../include/point.hpp"

#include <algorithm>
#include <map>
#include <set>
#include <vector>

#define each(i, c) for (auto& i : c)
#define unless(cond) if (!(cond))

template<typename P, typename Q>
ostream& operator << (ostream& os, pair<P, Q> p)
{
  os << "(" << p.first << "," << p.second << ")";
  return os;
}

using namespace std;

typedef pair<Point, Point> Region;

enum harmonics { H, L };

struct PlaneY {
  Point corner;
  int len;
  vector<Point> corners() const
  {
    Point a = corner + Point(  0,  0,   0);
    Point b = corner + Point(  0,  0, len);
    Point c = corner + Point(len,  0, len);
    Point d = corner + Point(len,  0,   0);
    // MEMO: keep this order
    return {a, b, c, d};
  }
};

std::ostream& operator << (std::ostream& os, const PlaneY p)
{
  os << "(" << p.corner  << "," << p.len << ")";
  return os;
}

struct Bot {
  int id;
  Point pos;
  set<int> seeds;
  void wait(OutputBase* o)
  {
    cout << "Bot(" << id << "): wait" << endl;
    o->Wait();
  }
  void moveUp(OutputBase* o)
  {
    return moveY(o, 1);
  }
  void moveDown(OutputBase* o)
  {
    return moveY(o, -1);
  }
  void moveX(OutputBase* o, int len)
  {
    cout << "Bot(" << id << pos << "): move x, " << len << endl;
    assert(abs(len) <= 15 && "range error X: SMove");
    o->SMove({len, 0, 0});
    pos.x += len;
  }
  void moveY(OutputBase* o, int len)
  {
    cout << "Bot(" << id << pos << "): move y, " << len << endl;
    assert(abs(len) <= 15 && "range error Y: SMove");
    o->SMove({0, len, 0});
    pos.y += len;
  }
  void moveZ(OutputBase* o, int len)
  {
    cout << "Bot(" << id << pos << "): move z, " << len << endl;
    assert(abs(len) <= 15 && "range error Z: SMove");
    o->SMove({0, 0, len});
    pos.z += len;
  }
  // return true if it reached given point. othervise false.
  // REQUIRED: Bot.pos != p
  bool getClose(OutputBase* o, Point target)
  {
    assert(pos != target);
    cout << "Bot(" << id << pos << "): getClose, " << pos << " -> " << target << endl;
    const int lld = 15;
    {
      int diff = target.x - pos.x;
      if (diff) {
        if (diff < 0) diff = max(diff, -lld);
        else          diff = min(diff, +lld);
        moveX(o, diff);
        return pos == target;
      }
    }
    {
      int diff = target.y - pos.y;
      if (diff) {
        if (diff < 0) diff = max(diff, -lld);
        else          diff = min(diff, +lld);
        moveY(o, diff);
        return pos == target;
      }
    }
    {int diff = target.z - pos.z;
      if (diff) {
        if (diff < 0) diff = max(diff, -lld);
        else          diff = min(diff, +lld);
        moveZ(o, diff);
        return pos == target;
      }
    }
  }
  void getCloseOrWait(OutputBase* o, Point target)
  {
    if (pos == target) wait(o);
    else getClose(o, target);
  }
  void gfill(OutputBase* o, Point nd, Point fd)
  {
    cout << "Bot(" << id << "): gfill " << nd << ' ' << fd << endl;
    o->GFill(nd, fd);
  }
  void flip(OutputBase* o)
  {
    cout << "Bot(" << id << "): flip" << endl;
    o->Flip();
  }
  Bot fission(OutputBase* o, Point nd, int m)
  {
    cout << "Bot(" << id << "): fission" << nd << ", " << m << endl;
    assert(m <= seeds.size());
    o->Fission(nd, m);

    Bot b;
    b.id = *(seeds.begin());
    seeds.erase(b.id);
    b.pos = pos + nd;

    for (int i = 0; i < m; ++i) {
      b.seeds.insert(*(seeds.begin()));
      seeds.erase(*(seeds.begin()));
    }
    return b;
  }
  void fusionP(OutputBase* o, Point nd)
  {
    cout << "Bot(" << id << pos << "): fusionP" << endl;
    o->FusionP(nd);
  }
  void fusionS(OutputBase* o, Point nd)
  {
    cout << "Bot(" << id << pos << "): fusionS" << endl;
    o->FusionS(nd);
  }
  void halt(OutputBase* o)
  {
    cout << "Bot(" << id << pos << "): halt" << endl;
    o->Halt();
  }
  void fill(OutputBase* o, Point nd)
  {
    cout << "Bot(" << id << pos << "): fill " << nd << endl;
    o->Fill(nd);
  }
};

Bot newInitialBot(void)
{
  Bot b{1, Point(0, 0, 0)};
  for (int i = 2; i <= 40; ++i) {
    b.seeds.insert(i);
  }
  return b;
}

class Solver : protected OutputBase {
public:
  Solver(std::ofstream& ofs) : OutputBase(ofs) {}
  virtual void solve(void) = 0;
};

class Johniel9 : public Solver {
public:
  Johniel9(const Model& src_, const Model& dst_, std::ofstream& ofs)
    : src(src_), dst(dst_), Solver(ofs), R(max(src.R, dst.R)) {}
  virtual void solve(void);

  class Squad : public vector<Bot> {
  public:
    Squad(Bot a, Bot b, Bot c, Bot d)
    {
      push_back(a);
      push_back(b);
      push_back(c);
      push_back(d);
    }
    void show(void)
    {
      cout << "Squad" ;
      each (i, (*this)) cout << make_pair(i.id, i.pos);
      cout << endl;
    }
  };
private:

  Squad newSquad(Bot a);
  bool assemblePlaneY(Squad&, PlaneY);
  bool assembleWithPlanes(const int, Squad&);
  void getClose(Squad&, vector<Point>);
  void finalize(Squad&);
  void distribute(Squad&);

  const Model& src;
  const Model& dst;
  const int R;
};

void Johniel9::getClose(Squad& s, vector<Point> ps)
{
  each (p, ps) assert(s[0].pos.y == ps[0].y);
  cout << "getClose:"; each (p, ps) cout << p ; cout << endl;
  if (s[3].pos.y + 3 <= R - 2) {
    ps[1] += Point(0, 1, 0);
    ps[2] += Point(0, 2, 0);
    ps[3] += Point(0, 3, 0);

    s[0].wait(this);
    s[1].moveY(this, +1);
    s[2].moveY(this, +2);
    s[3].moveY(this, +3);

    while (s[0].pos != ps[0] || s[1].pos != ps[1] || s[2].pos != ps[2] || s[3].pos != ps[3]) {
      s[0].getCloseOrWait(this, ps[0]);
      s[1].getCloseOrWait(this, ps[1]);
      s[2].getCloseOrWait(this, ps[2]);
      s[3].getCloseOrWait(this, ps[3]);
    }

    s[0].wait(this);
    s[1].moveY(this, -1);
    s[2].moveY(this, -2);
    s[3].moveY(this, -3);

  } else {
    distribute(s);
    for (int i = 0; i < 4; ++i) {
      while (s[i].pos != ps[i]) {
        (i == 0) ? s[0].getCloseOrWait(this, ps[0]) : s[0].wait(this);
        (i == 1) ? s[1].getCloseOrWait(this, ps[1]) : s[1].wait(this);
        (i == 2) ? s[2].getCloseOrWait(this, ps[2]) : s[2].wait(this);
        (i == 3) ? s[3].getCloseOrWait(this, ps[3]) : s[3].wait(this);
      }
    }
  }
  return ;
}

Johniel9::Squad Johniel9::newSquad(Bot a)
{
  Bot b = a.fission(this, Point(0, 0, 1), 20);

  Bot d = a.fission(this, Point(1, 0, 0), 10);
  Bot c = b.fission(this, Point(1, 0, 0), 10);

  // MEMO: check this order
  return Squad({a, b, c, d});
}

bool Johniel9::assemblePlaneY(Squad& squad, PlaneY p)
{
  each (s, squad) assert(s.pos.y == p.corner.y + 1);
  cout << "assemblePlaneY: " << p << endl;

  if (p.len == 0) {
    if (p.corner.y == R - 2) {
      distribute(squad);
      for (Point target = p.corner + Point(0, 1, 0); squad[0].pos != target; ) {
        squad[0].getClose(this, target);
        squad[1].wait(this);
        squad[2].wait(this);
        squad[3].wait(this);
      }
      squad[0].fill(this, Point(0, -1, 0));
      squad[1].wait(this);
      squad[2].wait(this);
      squad[3].wait(this);
      distribute(squad);
    } else {
      const Point prev = squad[0].pos;
      squad[0].wait(this);
      squad[1].moveY(this, +1);
      squad[2].moveY(this, +1);
      squad[3].moveY(this, +1);
      for (Point target = p.corner + Point(0, 1, 0); squad[0].pos != target; ) {
        squad[0].getClose(this, target);
        squad[1].wait(this);
        squad[2].wait(this);
        squad[3].wait(this);
      }
      squad[0].fill(this, Point(0, -1, 0));
      squad[1].wait(this);
      squad[2].wait(this);
      squad[3].wait(this);
      while (squad[0].pos != prev) {
        squad[0].getClose(this, prev);
        squad[1].wait(this);
        squad[2].wait(this);
        squad[3].wait(this);
      }
      squad[0].wait(this);
      squad[1].moveY(this, -1);
      squad[2].moveY(this, -1);
      squad[3].moveY(this, -1);
    }
    return true;
  }

  vector<Point> corners = p.corners();

  vector<Point> top = corners;
  each (i, top) ++i.y;

  auto samePositions = [&] () {
    for (int i = 0; i < 4; ++i) {
      if (squad[i].pos != top[i]) return false;
    }
    return true;
  };

  while (!samePositions()) getClose(squad, top);

  squad.show();
  squad[0].gfill(this, Point(0, -1, 0), squad[2].pos - squad[0].pos);
  squad[1].gfill(this, Point(0, -1, 0), squad[3].pos - squad[1].pos);
  squad[2].gfill(this, Point(0, -1, 0), squad[0].pos - squad[2].pos);
  squad[3].gfill(this, Point(0, -1, 0), squad[1].pos - squad[3].pos);

  return true;
}

// return false if already finished, otherwise true.
bool Johniel9::assembleWithPlanes(const int y, Squad& squad)
{
  cout << "Y=" << y << endl;
  squad.show();
  each (s, squad) assert(s.pos.y == y + 1);

  set<Point> matters;
  for (int x = 0; x < R; ++x) {
    for (int z = 0; z < R; ++z) {
      if (dst(x, y, z)) {
        matters.insert({x, y, z});
      }
    }
  }
  if (matters.empty()) return false;

  while (matters.size()) {
    PlaneY p{*matters.begin(), 0};
    for (int len = 0; len <= 30; ++len) {
      bool flg = true;
      for (int i = 0; i <= len; ++i) {
        flg = flg && matters.count(p.corner + Point(i, 0, len));
        flg = flg && matters.count(p.corner + Point(len, 0, i));
      }
      if (!flg) break;
      p.len = len;
    }
    assemblePlaneY(squad, p);
    for (int x = 0; x <= p.len; ++x) {
      for (int z = 0; z <= p.len; ++z) {
        matters.erase(p.corner + Point(x, 0, z));
      }
    }
  }

  return true;
}

void Johniel9::distribute(Squad& s)
{
  const int y = s[0].pos.y;
  Point ps[] = {
    Point(0, y, 0),
    Point(0, y, R-1),
    Point(R-1, y, R-1),
    Point(R-1, y, 0),
  };

  s[0].getCloseOrWait(this, ps[0]);
  s[1].wait(this);
  s[2].getCloseOrWait(this, ps[2]);
  s[3].wait(this);

  s[0].getCloseOrWait(this, ps[0]);
  s[1].wait(this);
  s[2].getCloseOrWait(this, ps[2]);
  s[3].wait(this);

  for (int j = 0; j < 8; ++j) {
    bool flg = true;
    for (int i = 0; i < 4; ++i) {
      flg = flg && (s[i].pos == ps[i]);
    }
    if (flg) break;
    for (int i = 0; i < 4; ++i) {
      s[i].getCloseOrWait(this, ps[i]);
    }
  }
  return ;
}

void Johniel9::finalize(Squad& s)
{
  Point a = s[0].pos + Point(0, 0, 0);
  Point b = s[0].pos + Point(0, 0, 1);
  Point c = s[0].pos + Point(1, 0, 1);
  Point d = s[0].pos + Point(1, 0, 0);
  getClose(s, {a, b, c, d});

  cout << "fusion: ";
  s.show();

  s[0].fusionP(this, s[1].pos - s[0].pos);
  s[1].fusionS(this, s[0].pos - s[1].pos);
  s[2].fusionS(this, s[3].pos - s[2].pos);
  s[3].fusionP(this, s[2].pos - s[3].pos);

  s[0].fusionP(this, s[3].pos - s[0].pos);
  s[3].fusionS(this, s[0].pos - s[3].pos);

  for (Point p(0, R - 2, 0); s[0].pos != p; s[0].getClose(this, p)) ;
  for (Point p(0,     0, 0); s[0].pos != p; s[0].getClose(this, p)) ;
  s[0].flip(this);
  s[0].halt(this);

  return ;
}

void Johniel9::solve()
{
  Bot ini = newInitialBot();
  ini.flip(this);
  Squad s = newSquad(ini);

  for (int y = 0; y < R - 1; ++y) {
    each (bot, s) bot.moveUp(this);
    if (!assembleWithPlanes(y, s)) break;
  }
  finalize(s);

  return ;
}

void usage(void)
{
  cerr << "usage: a.out SRC_MDL TGT_MDL OUTPUT" << endl;
  return ;
}

int main(int argc, char *argv[])
{
  if (argc != 4) {
    usage();
    return 1;
  }

  Model src(argv[1]);
  Model dst(argv[2]);

  cout << make_pair(src.R, dst.R) << endl;

	std::ofstream ofs(argv[3]);
  Johniel9(src, dst, ofs).solve();

  return 0;
}
