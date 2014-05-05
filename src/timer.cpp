#include "timer.hpp"

using namespace std;

/*******************************************************************************
 * time_function
 *
 * times the running time of a function in seconds
 */
double time_function(char* train, char* test, size_t level_one_threads,
    size_t level_two_threads) {

  hmm m("<start>", "<end>", train);

  struct timeval wall_clock_start, wall_clock_finished;
  double wall_clock_elapsed;
  gettimeofday(&wall_clock_start, NULL);

  omp_set_num_threads(level_one_threads);

  perform_run(m, test, level_two_threads);

  gettimeofday(&wall_clock_finished, NULL);
  wall_clock_elapsed =
    ((wall_clock_finished.tv_sec  - wall_clock_start.tv_sec) * 1000000u +
     wall_clock_finished.tv_usec - wall_clock_start.tv_usec) / 1.e6;

  // return time in seconds
  return wall_clock_elapsed;
}

/*******************************************************************************
 * peform_run
 *
 * run the forward algorithm using the train and sets
 */
void perform_run(hmm& m, char* test, size_t threads) {
  double total_probability = 1.0;

  #pragma omp parallel
  {
    int tid = omp_get_thread_num();
    char tid_str[64];
    sprintf(tid_str, "%d", tid);
    ifstream is((string(test) + "/" + "section_" + tid_str + ".pos").c_str());
    sentence_iterator si(&is);
    double local_total_probability = 1.0;

    omp_set_num_threads(threads);

    for (sentence_iterator end; si != end; ++si)
      local_total_probability *= m.parallel_forward(*si);

    #pragma omp atomic
      total_probability *= local_total_probability;
  }
}
