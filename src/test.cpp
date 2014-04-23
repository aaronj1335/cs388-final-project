#include <fstream>

#include <assert.h>

#include <boost/filesystem.hpp>

#include "util.hpp"

using namespace std;
using namespace boost::filesystem;

int main(int argc, char* argv[]) {
  ifstream f3("test/threelines.txt");
  ifstream f4("test/fourlines.txt");

  line_iterator lif3 = line_iterator(&f3);
  line_iterator lie = line_iterator();
  int dist3 = std::distance(lif3, lie);

  assert(dist3 == 3);
  line_iterator lif4 = line_iterator(&f4);
  assert(distance(lif4, lie) == 4);

  new (&f3) ifstream("test/threelines.txt");
  new (&f4) ifstream("test/fourlines.txt");

  recursive_directory_iterator di("test");

  size_t count = 0;
  for (file_line_iterator it(&di), end; it != end; ++it)
    count++;
  assert(count == 7);

  recursive_directory_iterator rdi("data");
  count = 0;
  for (file_line_iterator it(&rdi), end; it != end; ++it)
    count++;
  assert(count == 78023);

  return 0;
}
