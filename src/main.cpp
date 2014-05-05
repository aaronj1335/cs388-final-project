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

int main(int argc, char* argv[]) {

  // datasets to use
  char* train = NULL;
  char* test = NULL;

  // variables for parallelization
  bool is_parallel = false;
  int level_one_threads = -1;
  int level_two_threads = -1;
  
  char opt;
  while ((opt = getopt(argc, argv, "r:t:n:m:")) != -1) {
    switch (opt) {
      case 'r':
        train = optarg;
        break;
      case 't':
        test = optarg;
        break;
      case 'n':
        level_one_threads = atoi(optarg);
        is_parallel = true;
        break;
      case 'm':
        level_two_threads = atoi(optarg);
        is_parallel = true;
        break;
      default:
        break;
    }
  }

  if (train == NULL || test == NULL) {
    cerr << "Usage: " << argv[0];
    cerr << " -r train_file -t test_file";
    cerr << " [-n level_one_threads -m level_two_threads]";
    cerr << endl;
    
    return 1;
  }

  // create pointer to the function we wish to call
  void (*function)(char*, char*) = &perform_run;

  // these are not currently used
  (void) is_parallel;
  (void) level_one_threads;
  (void) level_two_threads;

  // average over several runs
  int runs = 10;
  
  float total_time = 0;
  for (int i = 0; i < runs; i++) {
    total_time += time_function(function, train, test);
  }

  // output total time
  cout << total_time / runs << " seconds" << endl;
  
  return 0;
}