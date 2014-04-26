#ifndef __util__
#define __util__

#include <istream>
#include <fstream>
#include <sstream>
#include <list>

#include <boost/iterator/iterator_facade.hpp>
#include <boost/filesystem.hpp>
#include <boost/utility.hpp>

/*******************************************************************************
 * sentence
 *
 * a sentence is a vector of (word, part-of-speech) pairs
 */

typedef std::vector<std::pair<std::string, std::string> > sentence;


#if 0

/*******************************************************************************
 * line_iterator
 */

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


/*******************************************************************************
 * file_line_iterator
 */

class file_line_iterator :
  boost::noncopyable,
  public boost::iterator_facade<
    file_line_iterator,
    boost::filesystem::recursive_directory_iterator,
    std::input_iterator_tag,
    std::string> {

  boost::filesystem::recursive_directory_iterator* di;
  std::ifstream* is;
  line_iterator* li;
  boost::filesystem::recursive_directory_iterator di_end;
  line_iterator li_end;

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

    ~file_line_iterator();
};


/*******************************************************************************
 * tuple_iterator
 */

class tuple_iterator :
  public boost::iterator_facade<
    tuple_iterator,
    file_line_iterator,
    std::input_iterator_tag,
    std::string> {

  static char const* const ARTICLE_SEP;
  static const size_t ARTICLE_SEP_LEN = 4;

  file_line_iterator* fli;
  file_line_iterator fli_end;
  std::string line;
  std::string tuple;
  std::list<std::string> tuples;

  friend class boost::iterator_core_access;

  bool line_is_invalid() const;

  bool tuple_is_invalid() const;

  void increment(bool initial_increment = true);

  bool equal(const tuple_iterator& other) const;

  const std::string& dereference() const;

  public:
    tuple_iterator();

    explicit tuple_iterator(file_line_iterator* fli);
};


/*******************************************************************************
 * sentence_iterator
 */

class sentence_iterator :
  public boost::iterator_facade<
    sentence_iterator,
    tuple_iterator,
    std::input_iterator_tag,
    sentence> {

  static char const* const PERIOD;
  static const size_t PERIOD_LEN = 3;

  tuple_iterator *ti;
  sentence snt;
  tuple_iterator ti_end;

  friend class boost::iterator_core_access;

  void advance();

  void increment();

  bool equal(const sentence_iterator& other) const;

  const sentence& dereference() const;

  public:
    sentence_iterator();

    explicit sentence_iterator(tuple_iterator* fli);
};

#endif


/*******************************************************************************
 * formatted_sentence_iterator
 */

class formatted_sentence_iterator :
    public std::iterator<std::input_iterator_tag, sentence> {

  std::istream* is;
  std::string line;
  sentence s;

  void advance();

  void convert();

  public:
    formatted_sentence_iterator();

    formatted_sentence_iterator(std::istream* is);

    formatted_sentence_iterator& operator++();

    formatted_sentence_iterator& operator++(int junk);

    sentence operator*() const;

    const sentence* operator->() const;

    bool operator==(const formatted_sentence_iterator& rhs) const;

    bool operator!=(const formatted_sentence_iterator& rhs) const;
};


#endif
