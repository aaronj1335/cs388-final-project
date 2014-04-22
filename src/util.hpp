#ifndef __util__
#define __util__

#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>

bool find_file( const boost::filesystem::path& dir_path,
                const std::string& file_name,
                boost::filesystem::path& path_found);

// used for iterating over lines in a file:
//
//   line_iterator in(std::istream("file.txt"));
//   ifstream inf("test/threelines.txt");
//   for (line_iterator it(inf); it != line_iterator(); it++)
//     cout << "line: " << *it << endl;
//
class line {
  std::string data;

  friend std::istream& operator>>(std::istream& is, line& line);

  friend std::ostream& operator<<(std::ostream& os, const line& line);

  public:
    const std::string& string() const;
};

typedef std::istream_iterator<line> line_iterator;

#endif
