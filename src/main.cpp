#include <iostream>

#include <stdlib.h>
#include <omp.h>
#include <getopt.h>

#include "hmm.hpp"
#include "timer.hpp"

#define DEBUG       0
#define VERBOSE     0
#define PARALLEL    0

using namespace std;

void usage(const char *const argv0, int exit_val) {
  cerr << "Usage: " << argv0;
  cerr << " -r train_file -t test_file";
  cerr << " [-n level_one_threads -m level_two_threads -p perf_time_file]";
  cerr << " [-h]";
  cerr << endl;
  exit(exit_val);
}

int main(int argc, char* argv[]) {

  // datasets to use
  char* train = NULL;
  char* test = NULL;
  char* perf_time_fname = NULL;

  // variables for parallelization
  int level_one_threads = 1;
  int level_two_threads = 1;

  char opt;
  while ((opt = (char) getopt(argc, argv, "hr:t:n:m:p:")) != -1) {
    switch (opt) {
      case 'r':
        train = optarg;
        break;
      case 't':
        test = optarg;
        break;
      case 'n':
        level_one_threads = atoi(optarg);
        break;
      case 'm':
        level_two_threads = atoi(optarg);
        break;
      case 'p':
        perf_time_fname = optarg;
        break;
      case 'h':
        usage(argv[0], 0);
        break;
      default:
        usage(argv[0], EXIT_FAILURE);
        break;
    }
  }

  if (train == NULL || test == NULL)
    usage(argv[0], EXIT_FAILURE);

  // average over several runs
  /* int runs = 3; */
  // (eff that whatevr)
  int runs = 1;

  double total_time = 0;
  hmm m("<start>", "<end>", train);

  // warm up file system cache
  /* time_function(m, test, level_one_threads, level_two_threads); */

  for (int i = 0; i < runs; i++)
    total_time += time_function(m, test, level_one_threads, level_two_threads);

  if (perf_time_fname) {
    cerr << "writing time to '" << perf_time_fname << "': " <<
      (total_time / runs) << endl;
    ofstream f(perf_time_fname);
    f << (total_time / runs);
    f.close();
  }

  return 0;
}
