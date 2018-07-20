#include "model.hpp"
#include <iostream>

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
  Model m("../problemsL/LA001_tgt.mdl");
  assert(m.R == 20);
  int cnt = 0;
  for (int i = 0; i < m.R; ++i) {
    for (int j = 0; j < m.R; ++j) {
      for (int k = 0; k < m.R; ++k) {
        if (m(i, j, k)) {
          cout << i << ' ' << j << ' ' << k << endl;
          ++cnt;
        }
      }
    }
  }
  cout << cnt <<endl;
  
  return 0;
}
