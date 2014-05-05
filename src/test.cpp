#include <iostream>
#include <fstream>
#include <algorithm>

#include <stdlib.h>
#include <assert.h>
#include <getopt.h>
#include <sys/time.h>

#include <omp.h>

#include "data_parser.hpp"
#include "hmm.hpp"

using namespace std;

template <typename T>
bool close(T a, T b) {
  T threshold = (a + b) / 2 * 0.0001;

  return a - b < threshold && b - a < threshold;
}

void run_tests() {

  /*****************************************************************************
   * sentence_iterator
   */
  cout << "sentence_iterator" << endl;

  {
    ifstream is("test/presubset/one.pos");
    size_t dist = distance(sentence_iterator(&is), sentence_iterator());

    assert(dist == 10);
  }

  {
    ifstream is("test/presubset/one.pos");
    vector<sentence> sentences;

    copy(sentence_iterator(&is), sentence_iterator(), back_inserter(sentences));

    assert(sentences.size() == 10);
  }

  {
    ifstream is("data/converted/1/section_0.pos");

    size_t count = 0;

    for (sentence_iterator i(&is), end; i != end; ++i, ++count) {
      if (count == 0) {
        assert(i->front().first == "Pierre");
        assert(i->back().second == ".");
      } else if (count == 1) {
        assert(i->front().first == "Mr.");
        assert(i->back().second == ".");
      }
    }

    assert(count == 45920);
  }


  /*****************************************************************************
   * hmm::forward
   */
  cout << "hmm::forward" << endl;

  {
    hmm m("s0", "sf");

    // from here: http://www.cs.utexas.edu/~mooney/cs388/old-midterm.pdf
    m.transitions["s0"] = map<string, double>();
    m.transitions["s0"]["s1"] = 0.2;
    m.transitions["s0"]["s2"] = 0.8;

    m.transitions["s1"] = map<string, double>();
    m.transitions["s1"]["s1"] = 0.2;
    m.transitions["s1"]["s2"] = 0.6;
    m.transitions["s1"]["sf"] = 0.2;

    m.transitions["s2"] = map<string, double>();
    m.transitions["s2"]["s1"] = 0.2;
    m.transitions["s2"]["s2"] = 0.6;
    m.transitions["s2"]["sf"] = 0.2;

    m.emissions["s1"] = map<string, double>();
    m.emissions["s1"]["A"] = 0.6;
    m.emissions["s1"]["B"] = 0.4;

    m.emissions["s2"] = map<string, double>();
    m.emissions["s2"]["A"] = 0.4;
    m.emissions["s2"]["B"] = 0.6;

    m.tag_vector.push_back("s1");
    m.tag_vector.push_back("s2");

    sentence s;
    s.push_back(pair<string, string>("A", ""));
    s.push_back(pair<string, string>("B", ""));

    assert(close(m.forward(s), 0.03872));
  }

  {
    hmm m("s0", "sf");

    m.transitions["s0"] = map<string, double>();
    m.transitions["s0"]["s1"] = 1.0;

    m.transitions["s1"] = map<string, double>();
    m.transitions["s1"]["s1"] = 0.7;
    m.transitions["s1"]["sf"] = 0.3;

    m.emissions["s1"] = map<string, double>();
    m.emissions["s1"]["X"] = 0.9;
    m.emissions["s1"]["Y"] = 0.1;

    m.tag_vector.push_back("s1");

    sentence s;
    s.push_back(pair<string, string>("X", ""));

    assert(close(m.forward(s), 0.27));

    sentence s2;
    s2.push_back(pair<string, string>("Y", ""));

    assert(close(m.forward(s2), 0.03));

    sentence s3;
    s3.push_back(pair<string, string>("X", ""));
    s3.push_back(pair<string, string>("X", ""));

    assert(close(m.forward(s3), 0.1701));
  }


  /*****************************************************************************
   * hmm::make_random_model
   */
  cout << "hmm::make_random_model" << endl;

  {
    ifstream is("test/presubset/one.pos");
    sentence_iterator si(&is);
    hmm m("<start>", "<end>", si);

    ifstream is2("test/presubset/one.pos");
    sentence_iterator si2(&is2);
    double total_probability = 1.0;

    for (sentence_iterator end; si2 != end; ++si2)
      total_probability *= m.forward(*si2);

    assert(close(total_probability, 3.77704e-131));
  }

  {
    char train[] = "test/presubset/one.pos";
    hmm m("<start>", "<end>", train);

    ifstream is2("test/presubset/one.pos");
    sentence_iterator si2(&is2);
    double total_probability = 1.0;

    for (sentence_iterator end; si2 != end; ++si2)
      total_probability *= m.forward(*si2);

    assert(close(total_probability, 3.77704e-131));
  }

  {
    ifstream is("data/converted/1/section_0.pos");
    sentence_iterator si(&is);
    hmm m("<start>", "<end>", si);

    /* ifstream is2("data/converted/1/section_0.pos"); */
    // it takes too long if we run the test on the full dataset
    ifstream is2("test/presubset/one.pos");
    sentence_iterator si2(&is2);
    double total_probability = 1.0;

    for (sentence_iterator end; si2 != end; ++si2)
      total_probability *= m.forward(*si2);

    assert(close(total_probability, 3.16697e-31));
  }

  {
    char train[] = "data/converted/1/section_0.pos";
    hmm m("<start>", "<end>", train);

    /* ifstream is2("data/converted/1/section_0.pos"); */
    // it takes too long if we run the test on the full dataset
    ifstream is2("test/presubset/one.pos");
    sentence_iterator si2(&is2);
    double total_probability = 1.0;

    for (sentence_iterator end; si2 != end; ++si2)
      total_probability *= m.forward(*si2);

    assert(close(total_probability, 3.16697e-31));
  }

  /*****************************************************************************
   * omp_nested
   */
  cout << "omp_nested" << endl;

  {
    vector<int> outer, inner;
    omp_set_nested(true);

    assert(omp_get_nested());

    omp_set_num_threads(2);
    #pragma omp parallel
    {
      #pragma omp critical
      {
        outer.push_back(omp_get_thread_num());
      }

      omp_set_num_threads(3);
      #pragma omp parallel
      {
        #pragma omp critical
        {
          inner.push_back(omp_get_thread_num());
        }
      }
    }

    sort(outer.begin(), outer.end());
    sort(inner.begin(), inner.end());

    assert(outer.size() == 2);
    assert(outer[0] == 0);
    assert(outer[1] == 1);

    assert(inner.size() == 6);
    assert(inner[0] == 0);
    assert(inner[1] == 0);
    assert(inner[2] == 1);
    assert(inner[3] == 1);
    assert(inner[4] == 2);
    assert(inner[5] == 2);
  }

}

int memoryLeakTest() {
  while (true) {
    ifstream is("data/converted/1/section_0.pos");

    for (sentence_iterator i(&is), end; i != end; ++i) {
      sentence s = *i;
      s.front();
    }
  }

  return 0;
}

int iteratorPerfTest() {
  vector<double> times;

  for (size_t i = 0; i <= 5; ++i) {
    struct timeval wallClockStart, wallClockFinish;
    double wallClockElapsed;
    gettimeofday(&wallClockStart, NULL);

    ifstream is("data/converted/1/section_0.pos");

    for (sentence_iterator it(&is), end; it != end; ++it) {}

    gettimeofday(&wallClockFinish, NULL);
    wallClockElapsed =
      ((wallClockFinish.tv_sec  - wallClockStart.tv_sec) * 1000000u +
       wallClockFinish.tv_usec - wallClockStart.tv_usec) / 1.e6;

    if (i != 0)
      times.push_back(wallClockElapsed);
  }

  double total = 0;
  for (vector<double>::iterator it = times.begin(); it != times.end(); ++it)
    total += *it;

  cout << (total / times.size()) << endl;

  return 0;
}

int main(int argc, char* argv[]) {
  char opt;
  bool memoryLeakTestFlag = false;
  bool iteratorPerfTestFlag = false;

  while ((opt = getopt(argc, argv, "mp")) != -1) {
    switch (opt) {
      case 'm':
        memoryLeakTestFlag = true;
        break;
      case 'p':
        iteratorPerfTestFlag = true;
        break;
      default: /* '?' */
        cerr << "USAGE: " << argv[0] << endl;
        exit(EXIT_FAILURE);
    }
  }

  if (memoryLeakTestFlag)
    return memoryLeakTest();

  if (iteratorPerfTestFlag)
    return iteratorPerfTest();

  run_tests();

  return 0;
}
