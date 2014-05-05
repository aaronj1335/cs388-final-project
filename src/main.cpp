#include <iostream>

#include <omp.h>

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
  
  // get arguments for training and testing set
  if (argc == 3) {
    // we are not using parallel
    train = argv[1];
    test = argv[2];
  } else if (argc == 5) {
    // we are parallel
    is_parallel = true;
    train = argv[1];
    test = argv[2];
    level_one_threads = atoi(argv[3]);
    level_two_threads = atoi(argv[4]);
  } else {
    // not enough arguments, exit
    cout << "Parallel" << endl;
    cout << "   Usage: main [train] [test] [level_one_threads] [level_two_threads]" << endl;
    cout << "\nSequential" << endl;
    cout << "   Usage: main [train] [test]" << endl;

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