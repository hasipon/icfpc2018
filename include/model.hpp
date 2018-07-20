#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <cassert>
#include <map>
#include <queue>

typedef unsigned char byte;

struct coordinate {
  int x, y, z;
  coordinate(): x(0), y(0), z(0) {}
  coordinate(int _x, int _y, int _z): x(_x), y(_y), z(_z) {}
  coordinate(std::initializer_list<int> init) {
    assert(init.size() == 3);
    auto itr = init.begin();
    x = *itr;
    ++itr;
    y = *itr;
    ++itr;
    z = *itr;
  }
};

std::ostream& operator << (std::ostream& os, const coordinate c)
{
  os << "(" << c.x << "," << c.y << "," << c.z << ")";
  return os;
}

inline bool operator < (coordinate a, coordinate b)
{
  if (a.x != b.x) return a.x < b.x;
  if (a.y != b.y) return a.y < b.y;
  return a.z < b.z;
}

class Model {
public:
  int R;
  Model(std::string filepath) {
    std::ifstream fin(filepath);
    byte b;
    fin >> b;
    R = b;
    assert(0 < R && R <= 250);
    int x, y, z;
    x = y = z = 0;
    while (fin >> b) {
      for (int nth = 0; nth < 8 /* byte */; ++nth) {
        if (b & (1 << nth)) {
          s.insert(coordinate(x, y, z));
        }
        ++x;
        y += x / R;
        x %= R;
        z += y / R;
        y %= R;
      }
    }
  }
  inline bool operator ()(coordinate c) const {
    return s.count(c);
  }
  inline bool operator ()(int x, int y, int z) const {
    return s.count({x, y, z});
  }
private:
  std::set<coordinate> s;
};
