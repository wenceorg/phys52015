import argparse

import numpy
from matplotlib import pyplot

parser = argparse.ArgumentParser()
parser.add_argument("output", type=str)

args, _ = parser.parse_known_args()

nthread = numpy.asarray([1, 2, 4, 6, 8, 16, 24, 32, 48])
staticn1 = numpy.asarray([0.140788,
                          0.070999,
                          0.035929,
                          0.025185,
                          0.018498,
                          0.018327,
                          0.012821,
                          0.013209,
                          0.020777])

staticn10 = numpy.asarray([3.533391,
                           1.865965,
                           1.030027,
                           0.723685,
                           0.557848,
                           0.316581,
                           0.254463,
                           0.367480,
                           0.357821])
fig, axes = pyplot.subplots(1)


def speedup(time):
    return time[0] / time


axes.plot(nthread, speedup(staticn1), ".-", label="static ($n=1$)")
axes.plot(nthread, speedup(staticn10), ".-", label="static ($n=10$)")
axes.plot(nthread, nthread, "k--", label="Linear speedup")

axes.legend(loc="best")

axes.set_xlabel("Number of threads")

axes.set_ylabel("Speedup")

fig.savefig(args.output,
            orientation="landscape",
            transparent=True,
            bbox_inches="tight")

