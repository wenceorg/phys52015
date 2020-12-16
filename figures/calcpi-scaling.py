import argparse

import numpy
from matplotlib import pyplot

parser = argparse.ArgumentParser()
parser.add_argument("output", type=str)

args, _ = parser.parse_known_args()

data = numpy.asarray([1.97,
                      0.95,
                      0.52,
                      0.28,
                      0.16])
ns = numpy.asarray([1, 2, 4, 8, 16])


fig, axes = pyplot.subplots(1)

axes.plot(ns, data, "o-", label=r"Time to solution (fixed number of points)")
axes.plot(ns, data[0]/ns, "r--", label=r"Perfect scaling")
axes.set_xlabel("Number of processes")
axes.set_ylabel("Time [s]")
axes.legend(loc="best")

fig.savefig(args.output,
            orientation="landscape",
            transparent=True,
            bbox_inches="tight")
