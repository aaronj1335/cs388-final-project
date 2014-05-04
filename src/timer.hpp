#ifndef __timer__
#define __timer__

#include <fstream>
#include <ctime>

#include "data_parser.hpp"
#include "hmm.hpp"

/*******************************************************************************
 * time_function
 *
 * times the running time of a function in seconds
 */
double time_function(void (*function)(char*, char*), char* train, char* test);

/*******************************************************************************
 * peform_run
 *
 * run the forward algorithm using the train and sets
 */
void perform_run(char* train, char* test);

#endif