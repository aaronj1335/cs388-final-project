#include <assert.h>
#include <string.h>

#include "util.hpp"

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
