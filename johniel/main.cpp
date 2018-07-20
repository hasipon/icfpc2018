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

  
  Model m("../problemsL/LA002_tgt.mdl");
  for (int i = 0; i < m.R; ++i) {
    for (int j = 0; j < m.R; ++j) {
      for (int k = 0; k < m.R; ++k) {
        if (m(i, j, k)) {
          // printf("scatter3(%d,%d,%d,\'filled\')\n", i, j, k);
          cout << i << ' ' << j << ' ' << k << endl;
        }
      }
    }
  }
  // puts("view(0,30)");

  int cnt = 0;
  each (i, m) {
    ++cnt;
  }
  assert(0 < cnt);
  return 0;
}
