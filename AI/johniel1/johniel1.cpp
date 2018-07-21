#include "../../include/mutable_model.hpp"
#include "../../include/TraceDumper.hpp"

using namespace std;

class Solver {
  virtual vector<Command*> solve(Model m) = 0;
};

class Sweep : public Solver {
public:
  Sweep(){}
  virtual vector<Command*> solve(Model m);
};

bool get_close(coordinate& curr, coordinate end, vector<Command*>& v) {
  const int lld = 15;
  // X
  while (true) {
    int diff = (end.x - curr.x) / lld * lld;
    unless (diff) break;
    v.push_back(new SMove(DIR_X, diff));
    curr.x += diff;
  }
  if (curr.x != end.x) {
    v.push_back(new SMove(DIR_X, end.x - curr.x));
    curr.x = end.x;
  }
  // Y
  while (true) {
    int diff = (end.y - curr.y) / lld * lld;
    unless (diff) break;
    v.push_back(new SMove(DIR_Y, diff));
    curr.y += diff;
  }
  if (curr.y != end.y) {
    v.push_back(new SMove(DIR_Y, end.y - curr.y));
    curr.y = end.y;
  }
  // Z
  while (true) {
    int diff = (end.z - curr.z) / lld * lld;
    unless (diff) break;
    v.push_back(new SMove(DIR_Z, diff));
    curr.y += diff;
  }
  if (curr.z != end.z) {
    v.push_back(new SMove(DIR_Z, end.z - curr.z));
    curr.z = end.z;
  }

  return curr == end;
}

vector<Command*> Sweep::solve(Model m)
{
  vector<Command*> v;
  coordinate curr;
  MutableModel mm(m.R);

  const int inf = 1 << 28;
  while (curr.y != m.R) {
    coordinate target(inf, inf, inf);
    int mx_y = 0;
    each (i, m) {
      mx_y = max(mx_y, i.y);
      if (i.y + 1 == curr.y && curr.md(i) < curr.md(target) && mm.LowHarmonicsFillable(i)) {
        target = i;
      }
    }
    cout << curr << " -> " << target << endl;

    if (target.x == inf) {
      if (curr.y == mx_y + 1) break;
      v.push_back(new SMove(DIR_Y, 1));
      curr += coordinate({0, 1, 0});
    } else {
      coordinate dst = target + coordinate(0, +1, 0);
      assert(get_close(curr, dst, v));
      each (i, md1) {
        if (i.y == -1 && mm.LowHarmonicsFillable(curr + i)) {
          v.push_back(new Fill(i));
          mm.fill(curr + i);
        }
      }
    }
  }
  assert(get_close(curr, coordinate(0, curr.y, 0), v));
  assert(get_close(curr, coordinate(0, 0, 0), v));
  v.push_back(new Halt());
  return v;
}

void usage(void)
{
  cerr << "usage: a.out INPUT OUTPUT" << endl;
  return ;
}

int main(int argc, char *argv[])
{
  if (argc != 3) {
    usage();
    return 1;
  }
  const char* input = argv[1];
  const char* output = argv[2];

  Sweep sweep;
  Model m(input);
  vector<Command*> commands = sweep.solve(m);
  dumpTrace(output, commands);

  return 0;
}
