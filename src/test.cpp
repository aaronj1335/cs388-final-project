#include <fstream>

#include <assert.h>
#include <getopt.h>

#include <boost/filesystem.hpp>

#include "util.hpp"

using namespace std;
using namespace boost::filesystem;

void run_tests() {
  /*****************************************************************************
   * line_iterator
   */

  ifstream f3("test/simple/threelines.txt");
  ifstream f4("test/simple/fourlines.txt");

  assert(distance(line_iterator(&f3), line_iterator()) == 3);
  assert(distance(line_iterator(&f4), line_iterator()) == 4);

  /*****************************************************************************
   * file_line_iterator
   */

  {
    recursive_directory_iterator rdi("test/simple");

    size_t count = 0;
    for (file_line_iterator it(&rdi), end; it != end; ++it)
      count++;
    assert(count == 7);
  }

  {
    recursive_directory_iterator rdi("data");
    size_t count = 0;
    for (file_line_iterator it(&rdi), end; it != end; ++it)
      count++;
    assert(count == 688892);
  }

  {
    vector<string> expected, actual;
    expected.push_back("coffee");
    expected.push_back("beer");
    expected.push_back("whiskey");
    expected.push_back("purple drank");
    expected.push_back("foo");
    expected.push_back("bar");
    expected.push_back("baz");

    recursive_directory_iterator rdi("test/simple");

    for (file_line_iterator it(&rdi), end; it != end; ++it)
      actual.push_back(*it);

    assert(expected == actual);
  }
}

int main(int argc, char* argv[]) {
  char opt;

  while ((opt = getopt(argc, argv, "c")) != -1) {
    switch (opt) {
      default: /* '?' */
        cerr << "USAGE: " << argv[0] << endl;
        exit(EXIT_FAILURE);
    }
  }

  run_tests();

  return 0;
}
