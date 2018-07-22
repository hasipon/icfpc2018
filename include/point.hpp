#ifndef _POINT_H_
#define _POINT_H_

#include <assert.h>
#include <algorithm>
#include <iostream>
#include <vector>

struct Point {
  int x, y, z;
  Point(): x(0), y(0), z(0) {}
  Point(int _x, int _y, int _z): x(_x), y(_y), z(_z) {}
  Point(std::initializer_list<int> init) {
    assert(init.size() == 3);
    auto itr = init.begin();
    x = *itr;
    ++itr;
    y = *itr;
    ++itr;
    z = *itr;
  }
  int md(Point other) {
    return abs(x - other.x) + abs(y - other.y) + abs(z - other.z);
  }
  int cd() {
    return std::max({x, y, z});
  }
};

std::ostream& operator << (std::ostream& os, const Point c)
{
  os << "(" << c.x << "," << c.y << "," << c.z << ")";
  return os;
}

inline bool operator < (Point a, Point b)
{
  if (a.x != b.x) return a.x < b.x;
  if (a.y != b.y) return a.y < b.y;
  return a.z < b.z;
}

inline bool operator == (Point a, Point b)
{
  return a.x == b.x && a.y == b.y && a.z == b.z;
}

inline bool operator <= (Point a, Point b)
{
  return a < b || a == b;
}

typedef Point difference;

Point operator += (Point& c, difference d) {
  c.x += d.x;
  c.y += d.y;
  c.z += d.z;
  return c;
}

Point operator + (Point c, difference d) {
  Point e = c;
  e += d;
  return e;
}

Point operator -= (Point& c, difference d) {
  c.x -= d.x;
  c.y -= d.y;
  c.z -= d.z;
  return c;
}

Point operator - (Point c, difference d) {
  Point e = c;
  e -= d;
  return e;
}

Point operator *= (Point& c, int m) {
  c.x *= m;
  c.y *= m;
  c.z *= m;
  return c;
}

Point operator * (Point c, int m) {
  Point e = c;
  e *= m;
  return e;
}

Point operator /= (Point& c, int m) {
  c.x /= m;
  c.y /= m;
  c.z /= m;
  return c;
}

Point operator / (Point c, int m) {
  Point e = c;
  e /= m;
  return e;
}

const std::vector<Point> nd = {
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

const std::vector<Point> md1 = {
  {-1, 0, 0},
  {0, -1, 0},
  {0, 0, -1},
  {+1, 0, 0},
  {0, +1, 0},
  {0, 0, +1},
};

namespace std{
    template<>
    class hash<Point>{
        public:
        size_t operator () ( const Point &p ) const{ return (p.x << 16) ^ (p.y << 8) ^ p.z; }
    };
}

#endif /* _POINT_H_ */
