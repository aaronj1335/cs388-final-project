#ifndef __util__
#define __util__

#include <string>
#include <vector>
#include <utility>
#include <sstream>
#include <iterator>

/*******************************************************************************
 * sentence
 *
 * a sentence is a vector of (word, part-of-speech) pairs
 */

typedef std::vector<std::pair<std::string, std::string> > sentence;

std::ostream& operator<<(std::ostream& os, const sentence& s);


/*******************************************************************************
 * sentence_iterator
 */

class sentence_iterator :
    public std::iterator<std::input_iterator_tag, sentence> {

  std::istream* is;
  std::string line;
  sentence s;

  void advance();

  void convert();

  public:
    sentence_iterator();

    sentence_iterator(std::istream* is);

    sentence_iterator& operator++();

    sentence_iterator& operator++(int junk);

    sentence operator*() const;

    const sentence* operator->() const;

    bool operator==(const sentence_iterator& rhs) const;

    bool operator!=(const sentence_iterator& rhs) const;
};


#endif
