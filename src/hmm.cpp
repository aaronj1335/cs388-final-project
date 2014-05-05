#include <string>
#include <iostream>
#include <fstream>

#include "hmm.hpp"

using namespace std;


double testable_rand() {
#if TEST
  return 0.1;
#else
  return ((double) rand()) / ((double)RAND_MAX);
#endif
}


/*******************************************************************************
 * hmm
 */

void hmm::make_random_model(sentence_iterator& it) {
  map<string, double> vocab;
  map<string, double> tags;

  for (sentence_iterator end; it != end; ++it) {
    for (size_t i = 0; i < it->size(); ++i) {
      vocab[it->at(i).first] = testable_rand();
      tags[it->at(i).second] = testable_rand();
    }
  }

  tags[end_tag] = testable_rand();

  for (map<string, double>::iterator it = tags.begin();
      it != tags.end();
      ++it) {
    emissions[it->first] = vocab;
    transitions[it->first] = tags;
    tag_vector.push_back(it->first);
  }

  transitions[start_tag] = tags;
  transitions[end_tag] = tags;
}

hmm::hmm(string start_tag, string end_tag) :
  start_tag(start_tag),
  end_tag(end_tag) {}



hmm::hmm(string start_tag, string end_tag, sentence_iterator& it) {
  hmm(start_tag, end_tag);
  make_random_model(it);
}

hmm::hmm(string start_tag, string end_tag, char* file_name) {
  ifstream is(file_name);
  sentence_iterator si(&is);

  hmm(start_tag, end_tag);
  make_random_model(si);
}

// kinda based on https://gist.github.com/aaronj1335/9650261
double hmm::forward(const sentence& s) {
  vector<vector<double> > lattice;

  // i think these should just be strait doubles, but i think it'd be better to
  // save the lattice across invocations and resize it from one call to the
  // next. i think that'd b easier w/ vectors than realloc.
  for (size_t i = 0; i < s.size(); ++i) {
    lattice.push_back(vector<double>());
    for (size_t j = 0; j < tag_vector.size(); ++j)
      lattice[i].push_back(0);
  }

  // iterate through each timestep
  size_t t = 0;
  for (; t < s.size(); ++t) {

    // and consider each state
    for (size_t st = 0; st < tag_vector.size(); ++st) {
      if (t == 0)
        // should this go in the initialization loop?
        lattice[t][st] = transitions[start_tag][tag_vector[st]];
      else
        for (size_t o = 0; o < tag_vector.size(); ++o)
          lattice[t][st] +=
            lattice[t - 1][o] * transitions[tag_vector[o]][tag_vector[st]];

      lattice[t][st] *= emissions[tag_vector[st]][s[t].first];
    }
  }

  double probability = 0;
  for (size_t st = 0; st < tag_vector.size(); ++st)
    probability +=
      lattice[t - 1][st] * transitions[tag_vector[st]][end_tag];

  return probability;
}

double hmm::parallel_forward(const sentence& s) {
  vector<vector<double> > lattice(s.size());
  size_t num_pos = tag_vector.size();

  #pragma omp parallel for
    for (size_t i = 0; i < s.size(); ++i) {
      lattice[i] = vector<double>();
      for (size_t j = 0; j < num_pos; ++j)
        lattice[i].push_back(0);
    }

  size_t t = 0;
  for (; t < s.size(); ++t) {

    #pragma omp parallel for
      for (size_t st = 0; st < num_pos; ++st) {
        if (t == 0)
          // should this go in the initialization loop?
          lattice[t][st] = transitions[start_tag][tag_vector[st]];
        else
          for (size_t o = 0; o < num_pos; ++o)
            lattice[t][st] +=
              lattice[t - 1][o] * transitions[tag_vector[o]][tag_vector[st]];

        lattice[t][st] *= emissions[tag_vector[st]][s[t].first];
      }
  }

  double probability = 0;
  for (size_t st = 0; st < num_pos; ++st)
    probability +=
      lattice[t - 1][st] * transitions[tag_vector[st]][end_tag];

  return probability;
}
