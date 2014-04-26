#ifndef __hmm__
#define __hmm__

#include <set>
#include <string>
#include <map>
#include <vector>

#include "data_parser.hpp"

typedef std::map<std::string, std::map<std::string, double> > emissions_t;
typedef std::map<std::string, std::map<std::string, double> > transitions_t;

// class structure for HMM
class hmm {
  private:
    std::string start_tag;
    std::string end_tag;

  public:
    hmm(std::string start_tag, std::string end_tag);

    double forward(const sentence& s);

    // state emitting tag
    emissions_t emissions;
    // state transitioning to state
    transitions_t transitions;
    // vector containing all possible tags
    std::vector<std::string> tag_vector;
};

#endif
