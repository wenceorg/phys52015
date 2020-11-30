import argparse

import numpy
from matplotlib import pyplot

parser = argparse.ArgumentParser()
parser.add_argument("output", type=str)

args, _ = parser.parse_known_args()

nthread = numpy.asarray([1, 2, 4, 6, 8, 16, 24])
static = numpy.asarray([
    0.153103,
    0.076727,
    0.039779,
    0.029135,
    0.020972,
    0.013154,
    0.013078
])
dynamic = numpy.asarray([
    0.115912,
    1.076026,
    1.928175,
    1.770699,
    1.650900,
    1.179847,
    1.113721
])
static_100 = numpy.asarray([
    0.116132,
    0.072398,
    0.040828,
    0.028432,
    0.022809,
    0.017262,
    0.013019
])
dynamic_100 = numpy.asarray([
    0.116788,
    0.091186,
    0.042430,
    0.030224,
    0.023028,
    0.013193,
    0.017787
])
guided = numpy.asarray([
    0.115694,
    0.061875,
    0.034982,
    0.023070,
    0.018557,
    0.009939,
    0.007297
])
guided_100 = numpy.asarray([
    0.116037,
    0.063980,
    0.034418,
    0.022897,
    0.018713,
    0.009824,
    0.010278
])

fig, axes = pyplot.subplots(1)


def speedup(time, mintime=None):
    if mintime is None:
        mintime = time[0]
    return mintime / time


mintime = min(x[0] for x in [static, static_100, dynamic, dynamic_100, guided, guided_100])

axes.plot(nthread, speedup(static, mintime), ".-", label="static")
axes.plot(nthread, speedup(static_100, mintime), "o-", label="static,100")
axes.plot(nthread, speedup(dynamic, mintime), "v-", label="dynamic")
axes.plot(nthread, speedup(dynamic_100, mintime), "^-", label="dynamic,100")
axes.plot(nthread, speedup(guided, mintime), "<-", label="guided")
axes.plot(nthread, speedup(guided_100, mintime), ">-", label="guided,100")

axes.plot(nthread, nthread, "k--", label="Linear speedup")

axes.legend(loc="best")

axes.set_xlabel("Number of threads")

axes.set_ylabel("Speedup (relative to best serial code)")

fig.savefig(args.output,
            orientation="landscape",
            transparent=True,
            bbox_inches="tight")
