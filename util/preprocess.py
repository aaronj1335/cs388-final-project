#!/usr/bin/python

import sys
import fnmatch
import os

from multiprocessing import Pool

def convert(file_name):
    """Convert the file to a better format"""

    with open(file_name) as f_handle:
        # TODO: finish implementing
        pass

    return True

def main(input_dir, extension=".pos", num_process=10):

    # only search for files with specific extension
    search = "*%s" % (extension)

    # grab filenames from the input directory
    matches = []
    for root, dirnames, filenames in os.walk(input_dir):
       for filename in fnmatch.filter(filenames, search):
           matches.append(os.path.join(root, filename))

    # create multiprocessing pool
    pool = Pool(processes=num_process)
    # call convert for each file name
    result = pool.map(convert, matches)
    
    # everything should come back as True
    assert all(result)
    assert len(matches) == len(result)

if __name__ == '__main__':

    if len(sys.argv) < 2:
        print "Usage: python preprocess.py [input_dir]"
        sys.exit(-1)

    # extract argument containing input dir
    main(sys.argv[1])