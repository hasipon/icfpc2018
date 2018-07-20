#ifndef _MODEL_H_
#define _MODEL_H_

#include "macro.hpp"

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

typedef coordinate difference;

coordinate operator += (coordinate& c, difference d) {
  c.x += d.x;
  c.y += d.y;
  c.z += d.z;
  return c;
}

coordinate operator + (coordinate c, difference d) {
  coordinate e = c;
  e.x += d.x;
  e.y += d.y;
  e.z += d.z;
  return e;
}

const std::vector<coordinate> nd = {
  {-1, -1, 0},
  {-1, 0, -1},
  {-1, 0, 0},
  {-1, 0, 1},
  {-1, 1, 0},
  {0, -1, -1},
  {0, -1, 0},
  {0, -1, 1},
  {0, 0, -1},
  {0, 0, 1},
  {0, 1, -1},
  {0, 1, 0},
  {0, 1, 1},
  {1, -1, 0},
  {1, 0, -1},
  {1, 0, 0},
  {1, 0, 1},
  {1, 1, 0},
};

class Model {
public:
  int R;
  Model() {}
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
  inline void insert(int x, int y, int z) {
    this->insert({x, y, z});
    return ;
  }
  inline void insert(coordinate c) {
    s.insert(c);
    return ;
  }
  inline bool operator ()(coordinate c) const {
    return s.count(c);
  }
  inline bool operator ()(int x, int y, int z) const {
    return s.count({x, y, z});
  }
  typedef std::set<coordinate>::iterator iterator;
  typedef std::set<coordinate>::const_iterator const_iterator;
  iterator begin() {
    return s.begin();
  }
  const_iterator begin() const {
    return s.begin();
  }
  iterator end() {
    return s.end();
  }
  const_iterator end() const {
    return s.end();
  }
private:
  std::set<coordinate> s;
};

#endif /* _MODEL_H_ */
