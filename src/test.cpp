#include <iostream>
#include <fstream>

#include <assert.h>

#include <boost/range/join.hpp>

#include "util.hpp"

using namespace std;

int main(int argc, char* argv[]) {
  ifstream f3("test/threelines.txt");
  ifstream f4("test/fourlines.txt");

  assert(distance(line_iterator(&f3), line_iterator()) == 3);
  assert(distance(line_iterator(&f4), line_iterator()) == 4);

  return 0;
}
