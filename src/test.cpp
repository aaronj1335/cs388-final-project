#include <iostream>
#include <fstream>

#include <assert.h>

#include "util.hpp"

using namespace std;
using namespace boost::filesystem;

int main(int argc, char* argv[]) {
  path p = path();

  find_file(path("data/penn-treebank-wsj"), string("wsj_2178.pos"), p);

  assert(p.native() == string("data/penn-treebank-wsj/21/wsj_2178.pos"));

  return 0;
}
