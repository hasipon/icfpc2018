#ifndef _MODEL_H_
#define _MODEL_H_

#include <stdio.h>
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
  int md(coordinate other) {
    return abs(x - other.x) + abs(y - other.y) + abs(z - other.z);
  }
  int cd() {
    return std::max({x, y, z});
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

inline bool operator == (coordinate a, coordinate b)
{
  return a.x == b.x && a.y == b.y && a.z == b.z;
}

inline bool operator <= (coordinate a, coordinate b)
{
  return a < b || a == b;
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
  e += d;
  return e;
}

coordinate operator -= (coordinate& c, difference d) {
  c.x -= d.x;
  c.y -= d.y;
  c.z -= d.z;
  return c;
}

coordinate operator - (coordinate c, difference d) {
  coordinate e = c;
  e -= d;
  return e;
}

coordinate operator *= (coordinate& c, int m) {
  c.x *= m;
  c.y *= m;
  c.z *= m;
  return c;
}

coordinate operator * (coordinate c, int m) {
  coordinate e = c;
  e *= m;
  return e;
}

coordinate operator /= (coordinate& c, int m) {
  c.x /= m;
  c.y /= m;
  c.z /= m;
  return c;
}

coordinate operator / (coordinate c, int m) {
  coordinate e = c;
  e /= m;
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

const std::vector<coordinate> md1 = {
  {-1, 0, 0},
  {0, -1, 0},
  {0, 0, -1},
  {+1, 0, 0},
  {0, +1, 0},
  {0, 0, +1},
};

class Model {
public:
  int R;
  Model() {}
  Model(const char* filepath) {
    FILE* fp = fopen(filepath, "r");
    R = fgetc(fp);
    assert(0 < R && R <= 250);
    int x, y, z;
    x = y = z = 0;
    int b;
    while ((b = fgetc(fp)) != EOF) {
      for (int nth = 0; nth < 8 /* byte */; ++nth) {
        if (b & (1 << nth)) {
          s.insert(coordinate(x, y, z));
        }
        ++z;
        y += z / R;
        z %= R;
        x += y / R;
        y %= R;
      }
    }
    fclose(fp);
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
protected:
  std::set<coordinate> s;
};

#endif /* _MODEL_H_ */
