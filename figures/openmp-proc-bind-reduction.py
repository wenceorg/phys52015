import argparse

import numpy
from matplotlib import pyplot

parser = argparse.ArgumentParser()
parser.add_argument("output", type=str)

args, _ = parser.parse_known_args()

nthread = numpy.asarray([1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24])

spread = numpy.asarray([
    0.601149,
    0.303316,
    0.159813,
    0.116568,
    0.0934739,
    0.0800869,
    0.0714741,
    0.0688419,
    0.0678229,
    0.068275,
    0.0679371,
    0.0678489,
    0.100209])

close = numpy.asarray([
    0.601832,
    0.316706,
    0.177091,
    0.138314,
    0.133621,
    0.13301,
    0.132252,
    0.113409,
    0.099298,
    0.088289,
    0.079628,
    0.072396,
    0.100244])

fig, axes = pyplot.subplots(1)


def speedup(time):
    return time[0] / time


axes.semilogy(nthread, spread, ".-", label="OMP_PROC_BIND=spread")
axes.semilogy(nthread, close, "o-", label="OMP_PROC_BIND=close")

axes.legend(loc="best")

axes.set_xlabel("Number of threads")

axes.set_ylabel("Time to compute reduction [s]")

fig.savefig(args.output,
            orientation="landscape",
            transparent=True,
            bbox_inches="tight")
