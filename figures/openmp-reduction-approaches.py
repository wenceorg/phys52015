import argparse

import numpy
from matplotlib import pyplot

parser = argparse.ArgumentParser()
parser.add_argument("output", type=str)

args, _ = parser.parse_known_args()

nthread = numpy.asarray([1, 2, 4, 6, 8, 12, 16, 24])

reduction = numpy.asarray([
    0.598447,
    0.306226,
    0.161568,
    0.118274,
    0.097393,
    0.080169,
    0.0912302,
    0.0991049
])

hand = numpy.asarray([0.619519,
                      0.302236,
                      0.159773,
                      0.111815,
                      0.096523,
                      0.078598,
                      0.090513,
                      0.0886431,
                      ])

critical = numpy.asarray([
    0.606584,
    0.306043,
    0.16057,
    0.111208,
    0.0939231,
    0.0784519,
    0.0906601,
    0.102683,
])

atomic = numpy.asarray([
    0.600335,
    0.302509,
    0.160946,
    0.111444,
    0.0940142,
    0.0784252,
    0.0904081,
    0.0952609,
])


fig, axes = pyplot.subplots(1)


def speedup(time):
    return time[0] / time


axes.semilogy(nthread, reduction, ".-", label="Reduction clause")
axes.semilogy(nthread, hand, "o-", label="By hand")
axes.semilogy(nthread, critical, "v-", label="Critical section")
axes.semilogy(nthread, atomic, "^-", label="Atomic")
axes.semilogy(nthread, 0.6/nthread, "k--", label="Perfect scaling")

axes.legend(loc="best")

axes.set_xlabel("Number of threads")

axes.set_ylabel("Time to compute reduction [s]")

fig.savefig(args.output,
            orientation="landscape",
            transparent=True,
            bbox_inches="tight")
