#include "../include/model.hpp"

using namespace std;

int main(int argc, char *argv[])
{
  if (argc != 2) {
    puts("usage: modeler FILEPATH");
    exit(1);
  }

  Model m(argv[1]);
  cout << m.R << endl;
  cout << distance(m.begin(), m.end()) << endl;
  each (i, m) {
    cout << i.x << ' ' << i.y << ' ' << i.z << endl;
  }
  return 0;
}
