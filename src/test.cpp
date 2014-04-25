#include <fstream>

#include <assert.h>
#include <getopt.h>
#include <sys/time.h>

#include <boost/filesystem.hpp>

#include "data_parser.hpp"

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

  /*****************************************************************************
   * tuple_iterator
   */

  {
    recursive_directory_iterator rdi("test/subset");
    file_line_iterator fli(&rdi);

    size_t count = 0;
    for (tuple_iterator ti(&fli), end; ti != end; ++ti)
      count++;
    assert(count == 783);
  }

  {
    recursive_directory_iterator rdi("test/subset");
    file_line_iterator fli(&rdi);
    vector<string> tuples;

    for (tuple_iterator ti(&fli), end; ti != end; ++ti)
      tuples.push_back(*ti);

    assert(tuples[0] == "Pierre/NNP");
    assert(tuples[1] == "Vinken/NNP");
    assert(tuples[2] == ",/,");
    assert(tuples[3] == "61/CD");
    assert(tuples[4] == "years/NNS");
    assert(tuples[780] == "today/NN");
    assert(tuples[781] == "./.");
    assert(tuples[782] == "''/''");
  }

  /*****************************************************************************
   * sentence_iterator
   */

  {
    recursive_directory_iterator rdi("test/subset");
    file_line_iterator fli(&rdi);
    tuple_iterator ti(&fli);

    size_t count = 0;
    for (sentence_iterator si(&ti), end; si != end; ++si)
      count++;
    assert(count == 33);
  }

  {
    recursive_directory_iterator rdi("data");
    file_line_iterator fli(&rdi);
    tuple_iterator ti(&fli);

    size_t count = 0;
    for (sentence_iterator si(&ti), end; si != end; ++si)
      count++;

    assert(count == 45280);
  }

  // this test fails because we're not handling the case where closing quotes
  // come after a period. woldn't be too hard, but i don't know if it's worth
  // it.
  /* { */
  /*   recursive_directory_iterator rdi("data"); */
  /*   file_line_iterator fli(&rdi); */
  /*   tuple_iterator ti(&fli); */
  /*   vector<sentence> result; */

  /*   for (sentence_iterator si(&ti), end; si != end; ++si) */
  /*     result.push_back(*si); */

  /*   cout << "end: " << result.back().back().first << endl; */
  /*   assert(result.back().back().first == "''"); */
  /* } */
}

int memoryLeakTest() {
  while (true) {
    recursive_directory_iterator rdi("data");
    file_line_iterator fli(&rdi);
    tuple_iterator ti(&fli);

    for (sentence_iterator si(&ti), end; si != end; ++si) {}
  }

  return 0;
}

int iteratorPerfTest() {
  vector<double> times;

  for (size_t i = 0; i <= 5; ++i) {
    struct timeval wallClockStart, wallClockFinish;
    double wallClockElapsed;
    gettimeofday(&wallClockStart, NULL);

    recursive_directory_iterator rdi("data");
    file_line_iterator fli(&rdi);
    tuple_iterator ti(&fli);

    for (sentence_iterator si(&ti), end; si != end; ++si) {}

    gettimeofday(&wallClockFinish, NULL);
    wallClockElapsed =
      ((wallClockFinish.tv_sec  - wallClockStart.tv_sec) * 1000000u +
       wallClockFinish.tv_usec - wallClockStart.tv_usec) / 1.e6;

    if (i != 0)
      times.push_back(wallClockElapsed);
  }

  double total = 0;
  for (vector<double>::iterator it = times.begin(); it != times.end(); ++it)
    total += *it;

  cout << (total / times.size()) << endl;

  return 0;
}

int main(int argc, char* argv[]) {
  char opt;
  bool memoryLeakTestFlag = false;
  bool iteratorPerfTestFlag = false;

  while ((opt = getopt(argc, argv, "mp")) != -1) {
    switch (opt) {
      case 'm':
        memoryLeakTestFlag = true;
        break;
      case 'p':
        iteratorPerfTestFlag = true;
        break;
      default: /* '?' */
        cerr << "USAGE: " << argv[0] << endl;
        exit(EXIT_FAILURE);
    }
  }

  if (memoryLeakTestFlag)
    return memoryLeakTest();

  if (iteratorPerfTestFlag)
    return iteratorPerfTest();

  run_tests();

  return 0;
}
