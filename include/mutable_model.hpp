#ifndef _MUTABLE_MODEL_H_
#define _MUTABLE_MODEL_H_

#include "model.hpp"

class MutableModel : public Model {
public:
  MutableModel(int _R) : uf(_R*_R*_R*_R) {
    R = _R;
    int cnt = 1;
    for (int x = 0; x < R; ++x) {
      for (int y = 0; y < R; ++y) {
        for (int z = 0; z < R; ++z) {
          long long int id = coord2no({x, y, z});
          no[id] = cnt++;
          if (y == 0) {
            uf.unite(0, no[id]);
          }
        }
      }
    }
  }
  // fill
  inline void fill(int x, int y, int z) {
    return this->fill({x, y, z});
  }
  inline void fill(coordinate c) {
    s.insert(c);
    each (d, md1) {
      coordinate a = c + d;
      if ((*this)(a)) {
        int p = no[coord2no(a)];
        int q = no[coord2no(c)];
        uf.unite(p, q);
      }
    }
    return ;
  }
  // fillable
  inline bool LowHarmonicsFillable(int x, int y, int z) {
    return LowHarmonicsFillable({x, y, z});
  }
  inline bool LowHarmonicsFillable(coordinate c) {
    return !(*this)(c) && beGrounded(c);
  }
  // beGrounded
  inline bool beGrounded(int x, int y, int z) {
    return beGrounded({x, y, z});
  }
  inline bool beGrounded(coordinate c) {
    if (c.y == 0) return true;
    each (i, md1) {
      if ((*this)(c + i) && grounded(c + i)) {
        return true;
      }
    }
    return false;
  }
  // grounded
  inline bool grounded(int x, int y, int z) {
    return this->grounded({x, y, z});
  }
  inline bool grounded(coordinate c) {
    return uf.isSameSet(0, no[coord2no(c)]);
  }
  // UnionFind
  class UnionFind {
  public:
    std::vector<int> rank, p;
    UnionFind(int size)
    {
      rank.resize(size, -(1 << 28));
      p.resize(size, -(1 << 28));
      for (int i = 0; i < size; ++i) {
        make(i);
      }
    }
    void make(int a)
    {
      rank[a] = 0;
      p[a] = a;
    }
    void unite(int a, int b)
    {
      link(find(a), find(b));
    }
    int find(int a)
    {
      return (a == p[a]) ? a : p[a] = find(p[a]);
    }
    bool isSameSet(int a, int b)
    {
      return find(a) == find(b);
    }
    void link (int a, int b)
    {
      if (rank[a] > rank[b]) {
        p[b] = a;
      } else {
        p[a] = b;
        if(rank[a] == rank[b]) rank[b]++;
      }
    }
  };
private:
  long long int coord2no(coordinate c) {
    return c.x * R * R + c.y * R + c.z + 1;
  }
  std::map<long long int, int> no;
  UnionFind uf;
};

#endif /* _MUTABLE_MODEL_H_ */
