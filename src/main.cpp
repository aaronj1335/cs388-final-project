#include <iostream>
#include <vector>

#include <omp.h>

#include "hmm.h"

#define DEBUG       0
#define VERBOSE     0
#define PARALLEL    0

using namespace std;

int main() {

    string stag = string("<S>");
    string etag = string("<E>");

    state start = {stag, stag};
    state end = {etag, etag};

    HMM *h = new HMM(start, end);

    cout << h->tagVector.size() << endl;
    
    return 0;
}