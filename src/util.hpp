#ifndef __util__
#define __util__

#include <istream>

#include <boost/iterator/iterator_facade.hpp>

class line_iterator :
  public boost::iterator_facade<
    line_iterator,                    // this class
    std::istream,                     // the thing over which we're iterating
    std::input_iterator_tag,          // the iterator concept
    std::string> {                    // the deref type

  std::istream* is;
  std::string line;

  friend class boost::iterator_core_access;

  void increment();

  bool equal(const line_iterator& other) const;

  const std::string& dereference() const;

  public:
    line_iterator() : is(NULL), line() {}

    explicit line_iterator(std::istream* is);
};

#endif
