#include "../../include/model.hpp"
#include "../../include/mutable_model.hpp"
#include "../../include/TraceDumper.hpp"

using namespace std;

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
  Bot(coordinate _c, Region _r, Model* _m, MutableModel* _mm)
    : curr(_c), region(_r), m(_m), mm(_mm)
  {
    bid = Bot::bots.size();
    fissioned = (region.second.x == m->R);
    // cout << "new Bot(" << bid << "): " << curr << ' ' << region << ' ' << fissioned << endl;
  }
  virtual bool run(vector<Command*>& commands);
  coordinate curr;
  Region region;
  Model* m;
  MutableModel* mm;
  int bid;

  static harmonics h;
  static vector<Bot*> bots;

  static bool runAll(vector<Command*>& commands)
  {
    each (b, bots) cout << b->bid << ' ' << b->curr  << endl;
    bool finished = true;
    int size = Bot::bots.size();
    for (int i = 0; i < size; ++i) {
      finished = finished && Bot::bots[i]->run(commands);
    }
    return finished;
  }

  static void init(Model* m)
  {
    Bot::bots.clear();

    const int bots = m->R / 20 + (bool)(m->R % 20);

    MutableModel mm(m->R);
    vector<Command*> commands;

    coordinate p({0, 0, 0});
    coordinate q({bots, 0, m->R - 1});
    Bot* b = new Bot(coordinate({0, 0, 0}), Region(p, q), m, &mm);
    Bot::bots.push_back(b);
    return ;
  }

private:
  Bot* fission(vector<Command*>& commands);
  bool fissioned;
};
vector<Bot*> Bot::bots;
harmonics Bot::h = L;

bool Bot::run(vector<Command*>& commands)
{
  // cout << region << ' ' << curr << endl;
  if (!inside(region, curr)) {
    int diff = (region.first - curr).x;
    cout << "SMOVE(" << bid << "): " << diff << ' ' << curr << ' ' << region << endl;
    commands.push_back(new SMove(DIR_X, diff));
    curr.x += diff;
    return false;
  }
  if (!fissioned) {
    fission(commands);
    return false;
  }
  cout << "SMOVE(" << bid << "): " << endl;
  commands.push_back(new Wait());
  return true;
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
  Bot* b = new Bot(c, r,  m, mm);
  Bot::bots.push_back(b);

  cout << "FISSION(" << bid << "): " << curr + d << endl;
  commands.push_back(new Fission(d, 20 - Bot::bots.size()));
  return b;
}

vector<Command*> Johniel2::solve(Model m)
{
  Bot::init(&m);
  vector<Command*> commands;
  while (!Bot::runAll(commands)) ;

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
