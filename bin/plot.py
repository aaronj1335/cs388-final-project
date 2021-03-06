#!/usr/bin/env python

import os

from os.path import join
from itertools import chain
from math import log

from pylab import plot, show, legend, close, figure, title, xlabel, ylabel, barh, savefig
import matplotlib as mpl
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter


N = 0               # problem size
P1 = 1              # num of level 1 threads
P2 = 2              # num of level 2 threads
T = 3               # running time

# we still store all figures here
out_dir = "report/figures/"

# make the output directory if necessary
if not os.path.exists(out_dir):
    os.makedirs(out_dir)

BLOCK = False

if 'BLOCK' not in globals():
    BLOCK = True

def prettify(func):
    def inner(*args, **kwargs):
        
        # update a few params about the
        mpl.rcParams['font.family'] = 'serif'

        return func(*args, **kwargs)
    return inner

def is_truthy(x):
    return bool(x)

def partition_by(data, key_fn):
    items = {}
    for i in data:
        key = key_fn(i)
        if key not in items:
            items[key] = []
        items[key].append(i)
    return items.values()

def read_data(d):
    params = lambda f: tuple(map(int, f.split('.')[0].split('_')))
    time = lambda f: (float(open(join(d, f)).read()),)
    return sorted(params(f) + time(f) for f in os.listdir(d))

def weak_scaling_data(data):
    wsdata = lambda d: [(i[P1] * i[P2], i[T], i) for i in d]
    key_fn = lambda i: i[N] / (i[P1] * i[P2])
    partitioned = partition_by(data, key_fn)

    # select the fastest numbers
    sets = []
    ds = filter(is_truthy, [wsdata(d) for d in partitioned])
    for d in ds:
        seen = {}
        for i in d:
            if i[0] not in seen or seen[i[0]][1] > i[1]:
                seen[i[0]] = i
        sets.append(sorted(seen.values()))

    max_len = max(map(len, sets))
    sets = filter(lambda s: len(s) == max_len, sets)

    return sets

def strong_scaling_data(data):
    ssdata = lambda d: [(i[P1] * i[P2], i[T], i) for i in d]
    key_fn = lambda i: i[N]
    partitioned = partition_by(data, key_fn)

    # select the fastest numbers
    sets = []
    ds = filter(is_truthy, [ssdata(d) for d in partitioned])
    for d in ds:
        seen = {}
        for i in d:
            if i[0] not in seen or seen[i[0]][1] > i[1]:
                seen[i[0]] = i
        sets.append(sorted(seen.values()))

    return sets

@prettify
def plot_scaling(data, the_title, munger, labeler):
    figure()
    
    for d in munger(data):
        zippd = zip(*d)

        # special case
        if 'Intel Weak' in the_title:
            if str(2048*24) not in labeler(d):
                continue

        plot(zippd[0], zippd[1], 'o-', label=labeler(d))
    legend()
    xlabel('Threads')
    ylabel('Time (seconds)')

    ax = plt.gca()
    current = map(int, ax.get_xticks())

    # just to wash out dups
    padding = sorted(set(current))

    # put one tick at the end
    padding += [max(padding) + padding[1] - padding[0]]

    # ensure we start from zero
    if padding[0] != 0:
        padding = [0] + padding

    # finalize xticks
    ax.set_xticks(padding)

    if 'Intel Weak' in the_title:
        # force y axis to be int
        yticks = ax.get_yticks()

        # ensure these are ints
        bounds = map(int, (min(yticks), max(yticks)))
        ax.set_yticks(range(bounds[0]-2, bounds[1]+3))

    t = "_".join(the_title.lower().split()) + ".pdf"
    savefig(out_dir + t, dpi=100, format='pdf')
    print t

def plot_weak_scaling(data, dataset=''):
    labeler = lambda d: 'Ratio: ' + str(d[0][2][0] / (d[0][2][1] * d[0][2][2]) * 24)

    plot_scaling(data, the_title=(dataset + ' Weak Scaling'),
            munger=weak_scaling_data, labeler=labeler)

def plot_strong_scaling(data, dataset=''):
    # need to multiply by 24 to find true problem size
    labeler = lambda d: 'Problem size: ' + str(d[0][2][N] * 24)

    plot_scaling(data, the_title=(dataset + ' Strong Scaling'),
            munger=strong_scaling_data, labeler=labeler)

@prettify
def plot_parallelization_levels(data, n, p, dataset=''):
    figure()
    t = 'Coarse versus fine-grained parallelism'
    if dataset:
        t += ' (' + dataset + ')'
    
    d = [(i[T], '%d X %d' % (i[P1], i[P2]))
            for idx, i in enumerate(data)
                if i[N] == n and i[P1] * i[P2] == p]
    zippd = zip(*d)
    xs = range(len(zippd[0]))
    plot(xs, zippd[0], 'o-', label='Problem size: ' + str(n))
    plt.xticks(xs, zippd[1])
    legend()
    xlabel('Coarse grained threads X fine grained threads')
    ylabel('Time (seconds)')
    
    t = "_".join(t.lower().split()) + ".pdf"
    savefig(out_dir + t, dpi=100, format='pdf')
    print t

@prettify
def plot_compiler_difference(gcc, intel):
    n = max(i[N] for i in gcc)
    gcc = [i for i in gcc if i[N] == n and i[P2] == 1]
    intel = [i for i in intel if i[N] == n and i[P2] == 1]
    d = [(i[P1] - 0.5, (i[T] - g[T]) / min(g[T], i[T]) * 100.)
            for i, g in zip(intel, gcc)]
    zippd = zip(*d)

    figure()
    plt.gca().xaxis.set_major_formatter(
            FuncFormatter(lambda v, p: str(v) + ' %'))
    t = 'Comparison of Intel and GNU comiler performance'
    barh(zippd[0], zippd[1])
    ylabel('Threads')
    xlabel('Speedup')
    
    t = "_".join(t.lower().split()) + ".pdf"
    savefig(out_dir + t, dpi=100, format='pdf')
    print t

data = wdata = sdata = intel_total_time = gcc_total_time = gcc_data = intel_data = None

if __name__ == '__main__':
    close(); close(); close(); close(); close(); close(); close(); close(); close(); # lololol

    data = gcc_data = read_data('goodtokno/tacc_gcc47_O3_2048')
    wdata = weak_scaling_data(data)
    sdata = strong_scaling_data(data)
    gcc_total_time = sum(map(lambda i: i[T], data))

    plot_strong_scaling(data, dataset='GCC')
    plot_weak_scaling(data, dataset='GCC')

    data = intel_data = read_data('goodtokno/tacc_intel_O3_8192')
    wdata = weak_scaling_data(data)
    sdata = strong_scaling_data(data)
    intel_total_time = sum(map(lambda i: i[T], data))

    plot_strong_scaling(data, dataset='Intel')
    plot_weak_scaling(data, dataset='Intel')

    plot_parallelization_levels(intel_data, 8192, 8, dataset='Intel')
    plot_parallelization_levels(gcc_data, 2048, 8, dataset='GCC')

    plot_compiler_difference(gcc_data, intel_data)

    data = read_data('goodtokno/tacc_gcc47_O3_coarsevsfineperf')
    plot_strong_scaling(data, dataset='GCC Without Nested Parallelism -')

    if BLOCK:
        raw_input()
