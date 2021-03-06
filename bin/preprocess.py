#!/usr/bin/python

import sys
import fnmatch
import os

from multiprocessing import Pool

def convert((file_name, output_dir), eos_tag="."):
    """Convert POS files to a format that can be easily parsed"""

    # place to save the newly converted file
    output_file = os.path.join(output_dir, os.path.basename(file_name)) + ".tmp"

    # counter for number of lines output
    line_counter = 0

    # list of special tags that mark the end of enclosed sentences
    special_tags = [
        "''", ")"
    ]

    with open(output_file, "w") as fout:
        with open(file_name, "r") as fin:

            # this is our line buffer which will contain a sentence
            line_buffer = []

            # this boolean should handle enclosed sentences
            write_next = False

            # iterate over the lines in fin
            for line in fin:
                # get rid of excess whitespace
                line = line.strip()

                # skip blank lines and the line breaks
                if line == "" or (line.startswith("=") and line.endswith("=")):
                    continue

                # if the line is enclosed with brackets, split tokens
                if line.startswith("[") and line.endswith("]"):
                    tokens = line[1:-1].strip()
                else:
                    tokens = line

                # otherwise, split and append tokens
                for token in tokens.split():

                    if write_next:
                        # check if this is a special tag            
                        word, tag = tuple(token.rsplit("/", 1))
                        # if this tag is special, add it to the buffer

                        # did we append the token?
                        append = False

                        if tag in special_tags:
                            line_buffer.append(token)
                            append = True

                        # write buffer to fout
                        fout.write(" ".join(line_buffer) + "\n")
                        line_counter+=1

                        # clear the buffer
                        line_buffer = []

                        # if we did not append it, append it now
                        if not append:
                            line_buffer.append(token)

                        # reset the boolean
                        write_next = False
                    else:
                        line_buffer.append(token)
                    
                        # check if the tag is an end-of-sentence tag
                        if token.endswith(eos_tag):
                            word, tag = tuple(token.rsplit("/", 1))
                            # confirming that the tag is correct
                            if tag == eos_tag:
                                # write to buffer on next token
                                write_next = True             

            # check the buffer before leaving the file
            if line_buffer:
                fout.write(" ".join(line_buffer) + "\n")

    # return number of lines written
    return (output_file, line_counter)

def partition(results, num_files):
    """Partitions the results into num_files buckets"""

    # total lines 
    line_counts = map(lambda (x,y): y, results)

    # maximum lines per bucket
    max_lines = float(sum(line_counts)) / num_files

    # init each bucket with an empty array
    buckets = dict(map(lambda n: (n, []), range(num_files)))

    # fill buckets sequentially
    current_total = 0 
    index = 0
    for name, count in results:
        current_total += count
        buckets[index].append(name)

        if current_total > max_lines:
            index+=1
            current_total = 0

    return buckets

def merge((section, file_names, output_dir), delete_tmp=True):
    """Merge the contents of the files"""

    out_name = "section_%d.pos" % (section)
    out_path = os.path.join(output_dir, out_name)

    with open(out_path, "w") as fout:
        for file_name in file_names:
            # read each file in and write it to fout
            with open(file_name, "r") as fin:
                fout.write(fin.read())

    if delete_tmp:
        # delete the temporary files we created earlier
        for tmp_file in file_names:
            try:
                os.remove(tmp_file)
            except OSError:
                # not much to do, the file is already gone
                pass

    return True

def main(input_dir, output_dir, num_files, extension=".pos", num_process=10):

    # only search for files with specific extension
    search = "*%s" % (extension)

    # grab filenames from the input directory
    matches = []
    for root, dirnames, filenames in os.walk(input_dir):
       for filename in fnmatch.filter(filenames, search):
           matches.append(os.path.join(root, filename))

    # ensure that the output directory exists
    if matches and not os.path.exists(output_dir):
        os.makedirs(output_dir)

    # add additional information for the function call
    matches_tuple = map(lambda f: (f, output_dir), matches)

    # create multiprocessing pool
    pool = Pool(processes=num_process)
    # call convert for each file name
    results = pool.map(convert, matches_tuple)
    
    # everything should come back as True
    assert all(results)
    assert len(matches) == len(results)

    buckets = partition(results, num_files)
    
    # add additional information for the function call
    buckets_tuple = map(lambda (i,b): (i,b,output_dir), buckets.items())

    # create new multiprocessing pool
    pool = Pool(processes=num_files)
    # call merge for each group
    merged = pool.map(merge, buckets_tuple)

    assert all(merged)
    assert len(merged) == num_files

if __name__ == '__main__':

    if len(sys.argv) < 4:
        print "Usage: python preprocess.py [input_dir] [output_dir] [num_files]"
        sys.exit(-1)

    # extract applicable arguments
    main(sys.argv[1], sys.argv[2], int(sys.argv[3]))