#include "timer.hpp"

using namespace std;

/*******************************************************************************
 * time_function
 *
 * times the running time of a function in seconds
 */
double time_function(void (*function)(char *, char *), char* train, char* test) {

  // begin clock
  clock_t start = clock();

  // call the function
  function(train, test);

  // return time in seconds
  return (clock() - start) / (double) CLOCKS_PER_SEC;
}

/*******************************************************************************
 * peform_run
 *
 * run the forward algorithm using the train and sets
 */
void perform_run(char* train, char* test) {

  hmm m("<start>", "<end>", train);

  ifstream is2(test);
  sentence_iterator si2(&is2);
  
  double total_probability = 1.0;
  for (sentence_iterator end; si2 != end; ++si2)
    total_probability *= m.forward(*si2);
}