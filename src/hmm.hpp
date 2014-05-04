#ifndef __hmm__
#define __hmm__

#include <set>
#include <string>
#include <map>
#include <vector>

#include <stdlib.h>

#include "data_parser.hpp"


/*******************************************************************************
 * emissions_t
 *
 * probabilities of seeing any vocab words from the set of states
 */

typedef std::map<std::string, std::map<std::string, double> > emissions_t;


/*******************************************************************************
 * transitions_t
 *
 * probabilities of transitioning from any state to any other state
 */

typedef std::map<std::string, std::map<std::string, double> > transitions_t;


/*******************************************************************************
 * hmm
 */

class hmm {
  private:
    std::string start_tag;
    std::string end_tag;

    void make_random_model(sentence_iterator& it);

  public:
    hmm(std::string start_tag, std::string end_tag);

    hmm(std::string start_tag, std::string end_tag, sentence_iterator& it);

    hmm(std::string start_tag, std::string end_tag, char* file_name);

    double forward(const sentence& s);

    // state emitting tag
    emissions_t emissions;
    // state transitioning to state
    transitions_t transitions;
    // vector containing all possible tags
    std::vector<std::string> tag_vector;
};

#endif
