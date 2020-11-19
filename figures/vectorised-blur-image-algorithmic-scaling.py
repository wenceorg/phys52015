import argparse

import numpy
from matplotlib import pyplot

parser = argparse.ArgumentParser()
parser.add_argument("output", type=str)

args, _ = parser.parse_known_args()

data = numpy.asarray([0.05, 0.1, 0.19, 0.28, 0.31, 0.46, 0.6,
                      0.79, 0.96, 1.21, 1.45, 1.77, 2.13,
                      2.53, 3.1, 3.47])
ns = numpy.arange(1, 17)
fig, axes = pyplot.subplots(1)
axes.set_xlabel("n")
axes.set_ylabel("Time [s]")
axes.plot(ns, data, "o", label="Timing data")
# Fit a quadratic polynomial to the data
a, b, c = numpy.polyfit(ns, data, 2)
axes.plot(ns, a*ns**2 + b*ns + c, "-", label="Quadratic best fit")
axes.legend(loc="best")
fig.savefig(args.output,
            orientation="landscape",
            transparent=True,
            bbox_inches="tight")
