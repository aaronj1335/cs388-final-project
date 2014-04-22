#include "util.hpp"

using namespace boost::filesystem;
using namespace std;

bool find_file(const path& dir_path, const string& file_name, path& path_found) {
  if (!exists(dir_path))
    return false;

  directory_iterator end_itr; // default construction yields past-the-end

  for (directory_iterator itr(dir_path); itr != end_itr; ++itr) {
    if (is_directory(itr->status())) {
      if (find_file( itr->path(), file_name, path_found))
        return true;
    } else if (itr->path().filename() == file_name) {
      path_found = itr->path();
      return true;
    }
  }

  return false;
}

