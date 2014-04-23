#include "util.hpp"

using namespace std;

void line_iterator::increment() {
  if (is && !std::getline(*is, line))
    is = NULL;
}

bool line_iterator::equal(const line_iterator& other) const {
  return this->is == other.is;
}

const std::string& line_iterator::dereference() const {
  return line;
}

line_iterator::line_iterator(std::istream* is) : is(is), line() {
  increment();
}
