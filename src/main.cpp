#include <iostream>
#include <vector>

#include <omp.h>

#include "hmm.hpp"

#define DEBUG       0
#define VERBOSE     0
#define PARALLEL    0

using namespace std;

int main() {

    string stag = string("<S>");
    string etag = string("<E>");

    hmm *h = new hmm(stag, etag);

    cout << h->tag_vector.size() << endl;
    
    return 0;
}
