import argparse

import numpy
from matplotlib import pyplot

parser = argparse.ArgumentParser()
parser.add_argument("output", type=str)

args, _ = parser.parse_known_args()

data = numpy.asarray([
    4.84831e-07,
    4.51413e-07,
    3.91578e-07,
    4.06072e-07,
    4.04786e-07,
    4.33078e-07,
    4.13709e-07,
    5.56903e-07,
    5.65616e-07,
    7.86378e-07,
    8.76273e-07,
    1.09138e-06,
    1.27414e-06,
    1.66294e-06,
    3.21483e-06,
    6.00854e-06,
    1.08416e-05,
    2.02359e-05,
    4.20334e-05,
    7.71259e-05,
    0.000492274,
    0.000964534,
    0.00195223,
    0.00375229,
    0.00751039])
ns = numpy.logspace(0, 24, 25, base=2, dtype=int)


alpha, _ = numpy.polyfit(ns[-6:], data[-6:], 1)
_, beta = numpy.polyfit(ns[:6], data[:6], 1)

alpha1, beta1 = numpy.polyfit(ns[:-6], data[:-6], 1)

fig, axes = pyplot.subplots(1)

axes.plot(ns, data, "o-", label=r"Ping-pong time")
axes.plot(ns, alpha*ns + beta, "r--",
          label="Linear model (fit to large messages)")
axes.plot(ns, alpha1*ns + beta1, "b-.",
          label="Linear model (fit to small messages)")
axes.set_xscale("log", base=2)
axes.set_yscale("log", base=10)
axes.set_xlabel("Size of message in bytes")
axes.set_ylabel("Time to send message [s]")
axes.set_title("Ping-pong timing on Hamilton par6 queue")
axes.legend(loc="best")

fig.savefig(args.output,
            orientation="landscape",
            transparent=True,
            bbox_inches="tight")
