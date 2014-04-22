#ifndef __util__
#define __util__

#include <boost/filesystem.hpp>
#include <iostream>

bool find_file( const boost::filesystem::path& dir_path,
                const std::string& file_name,
                boost::filesystem::path& path_found);

#endif
