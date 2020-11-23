import argparse

import numpy
from matplotlib import pyplot

parser = argparse.ArgumentParser()
parser.add_argument("output", type=str)

args, _ = parser.parse_known_args()

nthread = numpy.asarray([1, 2, 4, 8, 12, 16, 20, 24])
static = numpy.asarray(
    [
        0.0517831,
        0.0257339,
        0.0136762,
        0.00711703,
        0.00454402,
        0.00369906,
        0.00340796,
        0.00258207,
    ]
)
dynamic = numpy.asarray(
    [
        0.0311892,
        0.157997,
        0.0817611,
        0.0907531,
        0.0811329,
        0.0881069,
        0.0791368,
        0.0725932,
    ]
)
guided = numpy.asarray(
    [
        0.0343139,
        0.0196462,
        0.01122,
        0.00644207,
        0.00412297,
        0.00294399,
        0.00245595,
        0.00313997,
    ]
)
static_100 = numpy.asarray(
    [
        0.0245581,
        0.0265191,
        0.014328,
        0.00764894,
        0.00535202,
        0.00382209,
        0.00317717,
        0.002707,
    ]
)
dynamic_100 = numpy.asarray(
    [
        0.0350111,
        0.019367,
        0.0113392,
        0.00618887,
        0.00425696,
        0.00315094,
        0.00234914,
        0.00201106,
    ]
)
guided_100 = numpy.asarray(
    [
        0.026664,
        0.0200701,
        0.011353,
        0.00593305,
        0.00399494,
        0.00303006,
        0.00237298,
        0.00205898,
    ]
)

fig, axes = pyplot.subplots(1)


def speedup(time):
    return time[0] / time


axes.plot(nthread, speedup(static), ".-", label="static")
axes.plot(nthread, speedup(static_100), "o-", label="static,100")
axes.plot(nthread, speedup(dynamic), "v-", label="dynamic")
axes.plot(nthread, speedup(dynamic_100), "^-", label="dynamic,100")
axes.plot(nthread, speedup(guided), "<-", label="guided")
axes.plot(nthread, speedup(guided_100), ">-", label="guided,100")

axes.plot(nthread, nthread, "k--", label="Linear speedup")
axes.legend(loc="best")

axes.set_xlabel("Number of threads")

axes.set_ylabel("Speedup")

fig.savefig(args.output,
            orientation="landscape",
            transparent=True,
            bbox_inches="tight")
