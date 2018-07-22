#include "../../include/model.hpp"
#include "../../include/mutable_model.hpp"
#include "../../include/TraceDumper.hpp"

using namespace std;

set<coordinate> vis;

bool get_close(coordinate& curr, coordinate end, vector<Command*>& v)
{
  const int lld = 15;
  // X
  while (true) {
    int diff = (end.x - curr.x) / lld * lld;
    unless (diff) break;
    v.push_back(new SMove(DIR_X, diff));
    curr.x += diff;
    return true;
  }
  if (curr.x != end.x) {
    v.push_back(new SMove(DIR_X, end.x - curr.x));
    curr.x = end.x;
    return true;
  }
  // Y
  while (true) {
    int diff = (end.y - curr.y) / lld * lld;
    unless (diff) break;
    v.push_back(new SMove(DIR_Y, diff));
    curr.y += diff;
    return true;
  }
  if (curr.y != end.y) {
    v.push_back(new SMove(DIR_Y, end.y - curr.y));
    curr.y = end.y;
    return true;
  }
  // Z
  while (true) {
    int diff = (end.z - curr.z) / lld * lld;
    unless (diff) break;
    v.push_back(new SMove(DIR_Z, diff));
    curr.y += diff;
    return true;
  }
  if (curr.z != end.z) {
    v.push_back(new SMove(DIR_Z, end.z - curr.z));
    curr.z = end.z;
    return true;
  }

  return curr == end;
}

typedef pair<coordinate, coordinate> Region;

bool inside(Region r, coordinate c)
{
  return min(r.first, r.second) <= c && c <= max(r.first, r.second);
}

enum harmonics { H, L };

class Solver {
  virtual vector<Command*> solve(Model m) = 0;
};

class Johniel2 : public Solver {
public:
  Johniel2(){}
  virtual vector<Command*> solve(Model m);
};

class Bot {
public:
  Bot(coordinate _c, Region _r, Model* _m) : curr(_c), region(_r), m(_m)
  {
    bid = Bot::bots.size();
    fissioned = (region.second.x == m->R);
  }
  virtual bool run(vector<Command*>& commands);
  coordinate curr;
  const Region region;
  Model* m;
  int bid;

  static harmonics h;
  static vector<Bot*> bots;

  static bool runAll(vector<Command*>& commands)
  {
    each (b, bots) cout << b->bid << ": curr=" << b->curr  << endl;
    int cnt = 0;
    int size = Bot::bots.size();
    for (int i = 0; i < size; ++i) {
      cnt += Bot::bots[i]->run(commands);
    }
    return cnt;
  }

  static void init(Model* m)
  {
    Bot::bots.clear();

    const int bots = m->R / 20 + (bool)(m->R % 20);

    vector<Command*> commands;

    coordinate p({0, 0, 0});
    coordinate q({bots, 0, m->R - 1});
    Bot* b = new Bot(coordinate({0, 0, 0}), Region(p, q), m);
    Bot::bots.push_back(b);
    return ;
  }

private:
  bool fillRegion(vector<Command*>& commands);
  Bot* fission(vector<Command*>& commands);
  void moveUp(vector<Command*>& commands)
  {
    cout << "UP(" << bid << "): " << curr << " -> " << curr + coordinate(0, 1, 0) << endl;
    commands.push_back(new SMove(DIR_Y, 1));
    curr.y += 1;
  }
  void fill(difference d, vector<Command*>& commands)
  {
    assert(vis.count(curr + d) == 0);
    cout << "FILL(" << bid << "): " << curr << ", " << d << endl;
    commands.push_back(new Fill(d));
    vis.insert(curr + d);
  }


  bool fissioned;
};
vector<Bot*> Bot::bots;
harmonics Bot::h = L;

bool Bot::run(vector<Command*>& commands)
{
  // cout << "Bot::run (" << bid << "): " << endl;

  if (!inside(region, curr)) {
    int diff = (region.first - curr).x;
    cout << "SMOVE(" << bid << "): " << diff << ' ' << curr << ' ' << region << endl;
    commands.push_back(new SMove(DIR_X, diff));
    curr.x += diff;
    return true;
  }
  if (!fissioned) {
    fission(commands);
    return true;
  }
  if (curr.y == 0) {
    moveUp(commands);
    return true;
  }
  if (fillRegion(commands)) {
    return true;
  }
  cout << "WAIT(" << bid << "): " << endl;
  commands.push_back(new Wait());
  return false;
}

Bot* Bot::fission(vector<Command*>& commands)
{
  fissioned = true;
  const int bots = m->R / 20 + (bool)(m->R % 20);

  Region r = region;
  r.first.x  += bots;
  r.second.x += bots;

  r.second.x = min(r.second.x, m->R);

  difference d = difference(1, 0, 0);
  coordinate c = curr + d;
  Bot* b = new Bot(c, r,  m);
  Bot::bots.push_back(b);

  cout << "FISSION(" << bid << "): " << curr + d << endl;
  commands.push_back(new Fission(d, 20 - Bot::bots.size()));
  return b;
}

bool Bot::fillRegion(vector<Command*>& commands)
{
  each (i, md1) {
    if (inside(region, curr + i) && (*m)(curr + i) &&  vis.count(curr + i) == 0 && i.y == -1) {
      fill(i, commands);
      return true;
    }
  }
  const int inf = 1 << 28;
  coordinate target(inf, inf, inf);
  int cnt = 0;
  each (i, *m) {
    if (inside(region, i) && vis.count(i) == 0) {
      ++cnt;
      if (curr.y - 1 == i.y && curr.md(i) <= curr.md(target)) {
        if (curr.md(i) == curr.md(target)) {
          if (i < target) target = i;
        } else {
          target = i;
        }
      }
    }
  }
  if (cnt == 0) {
    cout << "FIN(" << bid << "):" << endl;
    return false;
  }
  if (target.x == inf) {
    moveUp(commands);
  } else {
    cout << "get_close(" << bid << "): " << curr << " -> " << target + coordinate(0, 1, 0) << endl;
    assert(get_close(curr, target + coordinate(0, 1, 0), commands));
  }
  return true;
}

vector<Command*> Johniel2::solve(Model m)
{
  Bot::init(&m);
  vector<Command*> commands;
  commands.push_back(new Flip());

  while (Bot::runAll(commands)) cout << endl;

  commands.push_back(new Flip());
  commands.push_back(new Halt());
  return commands;
}

void usage(void)
{
  cerr << "usage: a.out INPUT OUTPUT" << endl;
  return ;
}

int main(int argc, char *argv[])
{
  {
    Region r(coordinate(2, 0, 0), coordinate(4, 0, 29));
    coordinate c(1, 0, 0);
    assert(!inside(r, c));
  }

  if (argc != 3) {
    usage();
    return 1;
  }
  const char* input = argv[1];
  const char* output = argv[2];

  Johniel2 j;
  Model m(input);
  vector<Command*> commands = j.solve(m);
  dumpTrace(output, commands);

  return 0;
}
