#include "../include/model.hpp"
#include "../include/macro.hpp"

using namespace std;


int main(int argc, char *argv[])
{
  {
    coordinate c;
    assert(c.x == 0);
    assert(c.y == 0);
    assert(c.z == 0);
  }
  {
    coordinate c(1, 2, 3);
    assert(c.x == 1);
    assert(c.y == 2);
    assert(c.z == 3);
  }
  {
    coordinate c({1, 2, 3});
    assert(c.x == 1);
    assert(c.y == 2);
    assert(c.z == 3);
  }
  {
    coordinate c({1, 2, 3});
    difference d({10, 20, 30});
    c += d;
    assert(c.x == 11);
    assert(c.y == 22);
    assert(c.z == 33);
  }
  {
    coordinate p({11, 22, 33});
    coordinate q({1, 2, 3});
    p -= q;
    assert(p.x == 10);
    assert(p.y == 20);
    assert(p.z == 30);
  }
  {
    coordinate c({1, 2, 3});
    c *= 10;
    assert(c.x == 10);
    assert(c.y == 20);
    assert(c.z == 30);
  }
  {
    coordinate c({10, 20, 30});
    c /= 10;
    assert(c.x == 1);
    assert(c.y == 2);
    assert(c.z == 3);
  }
  {
    coordinate c({11, 22, 33});
    assert(c.md({1, 2, 3}) == 60);
    assert(c.cd() == 33);
  }

  Model m("../problemsL/LA002_tgt.mdl");
  for (int i = 0; i < m.R; ++i) {
    for (int j = 0; j < m.R; ++j) {
      for (int k = 0; k < m.R; ++k) {
        if (m(i, j, k)) {
          cout << i << ' ' << j << ' ' << k << endl;
        }
      }
    }
  }

  int cnt = 0;
  each (i, m) {
    ++cnt;
  }
  assert(0 < cnt);
  return 0;
}
