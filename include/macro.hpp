#ifndef _MACRO_H_
#define _MACRO_H_

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <utility>
#include <vector>

#include <cassert>
#include <cstdio>

#define each(i, c) for (auto& i : c)
#define unless(cond) if (!(cond))

template<typename P, typename Q>
std::ostream& operator << (std::ostream& os, std::pair<P, Q> p)
{
  os << "(" << p.first << "," << p.second << ")";
  return os;
}

#endif /* _MACRO_H_ */
