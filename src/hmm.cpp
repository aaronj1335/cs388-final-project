#include <string>

#include "hmm.hpp"

using namespace std;

hmm::hmm(string start_tag, string end_tag) :
  start_tag(start_tag),
  end_tag(end_tag) {}

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
