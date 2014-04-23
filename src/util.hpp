#ifndef __util__
#define __util__

#include <istream>
#include <fstream>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/filesystem.hpp>

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
    line_iterator();

    explicit line_iterator(std::istream* is);
};

class file_line_iterator :
  public boost::iterator_facade<
    file_line_iterator,
    boost::filesystem::recursive_directory_iterator,
    std::input_iterator_tag,
    std::string> {

  boost::filesystem::recursive_directory_iterator* di;
  boost::filesystem::recursive_directory_iterator di_end;
  line_iterator li_end;
  std::ifstream is;
  line_iterator li;

  friend class boost::iterator_core_access;

  void make_line_iterator();

  void advance_until_file_or_end(bool at_least_once = false);

  void increment();

  bool equal(const file_line_iterator& other) const;

  const std::string& dereference() const;

  public:
    file_line_iterator();

    explicit file_line_iterator(
        boost::filesystem::recursive_directory_iterator* di);
};

#endif
