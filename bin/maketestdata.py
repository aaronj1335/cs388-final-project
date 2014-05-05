#!/usr/bin/env python

import sys
import os

from argparse import ArgumentParser
from shutil import rmtree


HELP = """
%(prog)s -h -n <threads> -l <lines>
""" % {'prog': sys.argv[0]}

EVERYTHING = 'data/converted/1/section_0.pos'


def partition(g, chunk_size):
    lines = []
    for l in g:
        lines.append(l)
        if (len(lines) == chunk_size):
            yield lines
            lines = []


def main():
    p = ArgumentParser()
    p.add_argument('-l', '--lines', type=int, help='total lines to process')
    p.add_argument('-n', '--threads', type=int, help='number of threads')
    args = p.parse_args()

    out_dir = 'data/convertedsubset/' + str(args.threads)

    if os.path.isdir(out_dir):
        rmtree(out_dir)

    os.makedirs(out_dir)

    lines_per_thread = args.lines / args.threads

    with open(EVERYTHING) as everything:
        for i, section in enumerate(partition(everything, lines_per_thread)):
            with open(out_dir + '/section_' + str(i) + '.pos', 'w') as outf:
                outf.write(''.join(section))

            if (i + 1) >= args.threads:
                break


if __name__ == '__main__':
    main()
