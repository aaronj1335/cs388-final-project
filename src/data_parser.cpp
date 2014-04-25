#include <assert.h>
#include <string.h>

#include "data_parser.hpp"

using namespace std;
using namespace boost::filesystem;


/*******************************************************************************
 * line_iterator
 */

void line_iterator::increment() {
  if (is && !getline(*is, line))
    is = NULL;
}

bool line_iterator::equal(const line_iterator& other) const {
  return is == other.is;
}

const string& line_iterator::dereference() const {
  return line;
}

line_iterator::line_iterator() : is(NULL) {}

line_iterator::line_iterator(istream* is) : is(is) {
  increment();
}


/*******************************************************************************
 * file_line_iterator
 */

void file_line_iterator::make_line_iterator() {
  if (is)
    delete is;

  is = new ifstream((*di)->path().native().c_str());

  if (li)
    delete li;

  li = new line_iterator(is);
}

void file_line_iterator::advance_until_file_or_end(bool at_least_once) {
  if (at_least_once)
    ++(*di);

  while ((*di) != di_end && is_directory((*di)->path()))
    ++(*di);

  if ((*di) == di_end)
    di = NULL;
}

void file_line_iterator::increment() {
  assert(di);

  ++(*li);

  if ((*li) == li_end) {
    advance_until_file_or_end(true);

    if (di && (*di) != di_end)
      make_line_iterator();
  }
}

bool file_line_iterator::equal(const file_line_iterator& other) const {
  return di == other.di;
}

const string& file_line_iterator::dereference() const {
  return **li;
}

file_line_iterator::file_line_iterator() : di(NULL), is(NULL), li(NULL) {}

file_line_iterator::file_line_iterator(recursive_directory_iterator* di) :
    di(di),
    is(NULL),
    li(NULL),
    di_end(),
    li_end() {

  advance_until_file_or_end();

  if (di && (*di) != di_end)
    make_line_iterator();
}

file_line_iterator::~file_line_iterator() {
  if (is)
    delete is;

  if (li)
    delete li;
}


/*******************************************************************************
 * tuple_iterator
 */

char const* const tuple_iterator::ARTICLE_SEP = "====";

bool tuple_iterator::line_is_invalid() const {
  if (line.size() == 0)
    return true;

  if (strncmp(line.c_str(), ARTICLE_SEP, ARTICLE_SEP_LEN) == 0)
    return true;

  return false;
}

bool tuple_iterator::tuple_is_invalid() const {
  if (tuple.size() == 0)
    return true;

  if (tuple == "[")
    return true;

  if (tuple.find('/') == string::npos)
    return true;

  return false;
}

// TODO: refactor. we basically just need to go from something like this:
//
//    ======================================
//
//    [ Pierre/NNP Vinken/NNP ]
//    ,/, 
//    [ 61/CD years/NNS ]
//    old/JJ ,/, will/MD join/VB 
//    [ the/DT board/NN ]
//    as/IN 
//    [ a/DT nonexecutive/JJ director/NN Nov./NNP 29/CD ]
//    ./. 
//
//    [ Mr./NNP Vinken/NNP ]
//
// to something like this:
//
//    Pierre/NNP
//    Vinken/NNP
//    ,/,
//    61/CD
//    years/NNS
//    old/JJ
//    ,/,
//    will/MD
//    join/VB
//    the/DT
//    board/NN
//    as/IN
//    a/DT
//    nonexecutive/JJ
//    director/NN
//    Nov./NNP
//    29/CD
//    ./.
//    Mr./NNP
//    Vinken/NNP
//
// and this is DRAMATICALLY overcomplicate. i spent way too long trying to just
// use an istringstream, and kept getting segfaults.
void tuple_iterator::increment(bool initial_increment) {
  assert(fli);

  if (initial_increment) {
    if (!tuples.empty()) {
      tuple = tuples.front();
      tuples.pop_front();
    } else {
      tuple.clear();
    }
  }

  while (tuple_is_invalid() && *fli != fli_end) {
    if (tuples.empty()) {
      if (initial_increment)
        ++(*fli);

      line = **fli;

      while (line_is_invalid() && *fli != fli_end)
        line = *(++(*fli)); // lolwut

      if (*fli != fli_end) {
        tuples.clear();
        istringstream iss(line);
        copy(istream_iterator<string>(iss), istream_iterator<string>(),
            back_inserter(tuples));
      }
    }

    if (*fli == fli_end)
      break;

    if (!tuples.empty()) {
      tuple = tuples.front();
      tuples.pop_front();
    }
  }

  if (*fli == fli_end)
    fli = NULL;
}

bool tuple_iterator::equal(const tuple_iterator& other) const {
  return fli == other.fli;
}

const string& tuple_iterator::dereference() const {
  return tuple;
}

tuple_iterator::tuple_iterator() : fli(NULL) {}

tuple_iterator::tuple_iterator(file_line_iterator* fli) :
  fli(fli),
  fli_end() {

  if (fli && (*fli) != fli_end)
    increment(false);
}


/*******************************************************************************
 * sentence_iterator
 */

char const* const sentence_iterator::PERIOD = "./.";

void sentence_iterator::advance() {
  string s = **ti;
  size_t slash_idx;

  snt.clear();

  while (strncmp(s.c_str(), PERIOD, PERIOD_LEN) != 0) {
    slash_idx = s.find('/');

    assert(slash_idx != string::npos);
    snt.push_back(pair<string, string>( s.substr(0, slash_idx),
                                        s.substr(slash_idx + 1, s.size())));
    ++(*ti);

    if (*ti != ti_end)
      s = **ti;
    else
      break;
  }

  if (*ti != ti_end) {
    slash_idx = s.find('/');
    assert(slash_idx != string::npos);
    snt.push_back(pair<string, string>( s.substr(0, slash_idx),
                                        s.substr(slash_idx + 1, s.size())));
  }
}

void sentence_iterator::increment() {
  assert(ti);

  ++(*ti);

  if (*ti != ti_end)
    advance();

  if (*ti == ti_end)
    ti = NULL;
}

bool sentence_iterator::equal(const sentence_iterator& other) const {
  return ti == other.ti;
}

const sentence& sentence_iterator::dereference() const {
  return snt;
}

sentence_iterator::sentence_iterator() : ti(NULL) {}

sentence_iterator::sentence_iterator(tuple_iterator* ti) :
  ti(ti),
  ti_end() {

  if (ti && *ti != ti_end)
    advance();
}
