#include <assert.h>

#include "data_parser.hpp"

using namespace std;


/*******************************************************************************
 * sentence_iterator
 */

void sentence_iterator::advance() {
  getline(*is, line);
  convert();
}

void sentence_iterator::convert() {
  istringstream iss(line);

  s.clear();

  for (istream_iterator<string> it(iss), end; it != end; ++it) {

    string token = *it;
    size_t idx = token.find_last_of('/');
    string word = token.substr(0, idx);
    string part_of_speech = token.substr(idx + 1, token.size());

    s.push_back(pair<string, string>(word, part_of_speech));
  }
}

sentence_iterator::sentence_iterator() : is(NULL) {}

sentence_iterator::sentence_iterator(istream* is) :
    is(is),
    line() {

  if (is && !is->eof())
    advance();
}

sentence_iterator& sentence_iterator::operator++() {
  assert(is && !is->eof());

  if (is && !is->eof())
    advance();

  if (is->eof())
    is = NULL;

  return *this;
}

sentence_iterator& sentence_iterator::operator++(int junk) {
  return (*this)++;
}

sentence sentence_iterator::operator*() const {
  return s;
}

const sentence* sentence_iterator::operator->() const {
  return &s;
}

bool sentence_iterator::operator==(const sentence_iterator& rhs) const {
  return is == rhs.is;
}

bool sentence_iterator::operator!=(const sentence_iterator& rhs) const {
  return is != rhs.is;
}
