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
    // x
    {
      int diff = ((target.x - pos.x) / lld) * lld;
      if (diff) {
        moveX(o, diff);
        return pos == target;
      }
      if (target.x != pos.x) {
        moveX(o, target.x - pos.x);
        return pos == target;
      }
    }
    // y
    {
      int diff = ((target.y - pos.y) / lld) * lld;
      if (diff) {
        moveY(o, diff);
        return pos == target;
      }
      if (target.y != pos.y) {
        moveY(o, target.y - pos.y);
        return pos == target;
      }
    }
    // z
    {
      int diff = ((target.z - pos.z) / lld) * lld;
      if (diff) {
        moveZ(o, diff);
        return pos == target;
      }
      if (target.z != pos.z) {
        moveZ(o, target.z - pos.z);
        return pos == target;
      }
    }
  }
  void getCloseOrWait(OutputBase* o, Point target)
  {
    if (pos == target) return wait(o);
    getClose(o, target);
    return ;
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
    assert(seeds.size() <= m);
    o->Fission(pos + nd, 10);

    Bot b;
    b.id = *(seeds.begin());
    seeds.erase(b.id);

    for (int i = 0; i < m; ++i) {
      b.seeds.insert(*(seeds.begin()));
      seeds.erase(*(b.seeds.rbegin()));
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
};

Bot newBot(Point pos)
{
  static int cnt = 1;
  cout << "newBot: "<< cnt << endl;
  Bot b{cnt++, pos};
  for (int i = 1; i <= 40; ++i) {
    b.seeds.insert(i); 
  }
  return b;
}

Bot newInitialBot()
{
  return newBot(Point(0, 0, 0));
}

bool inside(Region r, Point c)
{
  return min(r.first, r.second) <= c && c <= max(r.first, r.second);
}

class Solver : protected OutputBase {
public:
  Solver(std::ofstream& ofs) : OutputBase(ofs) {}
  virtual void solve(void) = 0;
};

class Johniel3 : public Solver {
public:
  Johniel3(const Model& src_, const Model& dst_, std::ofstream& ofs)
    : src(src_), dst(dst_), Solver(ofs), R(max(src.R, dst.R)) {}
  virtual void solve(void);
  typedef vector<Bot> Squad;
private:

  Squad newSquad(Bot a);
  bool assemblePlaneY(Squad&, PlaneY);
  bool assembleWithPlanes(const int, Squad&);
  void getClose(Squad&, vector<Point>);

  const Model& src;
  const Model& dst;
  const int R;
};

void Johniel3::getClose(Squad& squad, vector<Point> ps)
{
    squad[0].wait(this);
    squad[1].wait(this);
    squad[2].getCloseOrWait(this, ps[2]);
    squad[3].wait(this);

    squad[0].wait(this);
    squad[1].wait(this);
    squad[2].getCloseOrWait(this, ps[2]);
    squad[3].getCloseOrWait(this, ps[3]);

    squad[0].wait(this);
    squad[1].getCloseOrWait(this, ps[1]);
    squad[2].getCloseOrWait(this, ps[2]);
    squad[3].getCloseOrWait(this, ps[3]);

    while (squad[0].pos != ps[0] || squad[1].pos != ps[1] || squad[2].pos != ps[2] || squad[3].pos != ps[3]) {
      for (int i = 0; i < 4; ++i) {
        squad[i].getCloseOrWait(this, ps[i]);
      }
    }
  return ;
}

Johniel3::Squad Johniel3::newSquad(Bot a)
{
  // ------- turn --------

  Point pb = Point(0, 0, 1);
  Fission(pb, 20);
  Bot b = newBot(a.pos + pb);

  // ------- turn --------
  
  Point pd = Point(1, 0, 0);
  Fission(pd, 10);
  Bot d = newBot(a.pos + pd);

  Point pc = Point(1, 0, 0);
  Fission(pc, 10);
  Bot c = newBot(b.pos + pc);
  
  // fission c

  // ------- turn --------
  
  // MEMO: check this order
  return Squad({a, b, c, d});
}

bool Johniel3::assemblePlaneY(Squad& squad, PlaneY p)
{
  assert(p.len != 0);
  
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

  squad[0].gfill(this, Point(0, -1, 0), squad[2].pos - squad[0].pos);
  squad[1].gfill(this, Point(0, -1, 0), squad[3].pos - squad[1].pos);
  squad[2].gfill(this, Point(0, -1, 0), squad[0].pos - squad[2].pos);
  squad[3].gfill(this, Point(0, -1, 0), squad[1].pos - squad[3].pos);

  return true;
}

// return false if already finished, otherwise true.
bool Johniel3::assembleWithPlanes(const int y, Squad& squad)
{
  cout << "Y:=" << y << endl;
  each (s, squad) assert(s.pos.y == y + 1);

  set<Point> matters;
  for (int x = 0; x < R; ++x) {
    for (int z = 0; z < R; ++z) {
      if (dst(x, y, z)) {
        matters.insert({x, y, z});
      }
    }
  }

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
    for (int x = 0; x <= p.len + 1; ++x) {
      for (int z = 0; z <= p.len + 1; ++z) {
        matters.erase(p.corner + Point(x, 0, z));        
      }
    }
  }

  return true;
}

void Johniel3::solve()
{
  Bot ini = newInitialBot();
  ini.flip(this);
  Squad s = newSquad(ini);

  for (int y = 0; y < R - 1; ++y) {
    each (bot, s) bot.moveUp(this);
    if (!assembleWithPlanes(y, s)) break;
  }

  Point a = s[0].pos + Point(0, 0, 0);
  Point b = s[0].pos + Point(0, 0, 1);
  Point c = s[0].pos + Point(1, 0, 1);
  Point d = s[0].pos + Point(1, 0, 0);
  getClose(s, {a, b, c, d});

  s[0].fusionP(this, s[1].pos - s[0].pos);
  s[1].fusionS(this, s[0].pos - s[1].pos);
  s[2].fusionS(this, s[3].pos - s[2].pos);
  s[3].fusionP(this, s[2].pos - s[3].pos);

  s[0].fusionP(this, s[3].pos - s[0].pos);
  s[3].fusionS(this, s[0].pos - s[3].pos);
  
  for (Point p(0, R - 2, 0); s[0].pos != p; s[0].getClose(this, p)) ;
  for (Point p(0,    0, 0); s[0].pos != p; s[0].getClose(this, p)) ;
  s[0].flip(this);  
  s[0].halt(this);
  
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
	std::ofstream ofs(argv[3]);
  Johniel3(src, dst, ofs).solve();

  return 0;
}
