#ifndef __timer__
#define __timer__

#include <fstream>

#include <stdio.h>
#include <sys/time.h>

#include <omp.h>

#include "data_parser.hpp"
#include "hmm.hpp"

/*******************************************************************************
 * time_function
 *
 * times the running time of a function in seconds
 */
double time_function(hmm& m, char* test, size_t level_one_threads,
    size_t level_two_threads);

/*******************************************************************************
 * peform_run
 *
 * run the forward algorithm using the train and sets
 */
void perform_run(hmm& hmm, char* test, size_t threads);

#endif
