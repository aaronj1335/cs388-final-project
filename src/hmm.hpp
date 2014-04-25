#ifndef __hmm__
#define __hmm__

#include <set>
#include <string>
#include <map>
#include <vector>

// holds information for state
struct state {
    std::string word;
    std::string tag;
};

// class structure for HMM
class HMM {
    private:
        state start;
        state end;

    public:
        HMM(state _start, state _end);

        // state emitting tag
        std::map<std::string, std::map<std::string, double> > emissions;
        // state transitioning to state
        std::map<std::string, std::map<std::string, double> > transitions;
        // vector containing all possible tags
        std::vector<std::string> tagVector;
};

#endif