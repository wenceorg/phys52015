import argparse

import numpy
from matplotlib import pyplot

parser = argparse.ArgumentParser()
parser.add_argument("output", type=str)

args, _ = parser.parse_known_args()

data = numpy.asarray([0.05840734641020706164,
                      0.01840734641020702611,
                      0.07440734641020707585,
                      0.02920734641020672484,
                      0.01428734641020668050,
                      0.00088334641020670901,
                      0.00037265358979299279,
                      0.00008662641020684703])
ns = numpy.logspace(1, 8, 8, dtype=int)


fig, axes = pyplot.subplots(1)

axes.loglog(ns, data, "o-", label=r"Error $|\pi - \tilde{\pi}|$")
axes.loglog(ns, 0.5*ns**(-0.5), "r--", label=r"$\mathcal{O}\left(\frac{1}{\sqrt{N}}\right)$")

axes.set_xlabel("Number of points (N)")
axes.set_ylabel("Absolute error")
axes.legend(loc="best")

fig.savefig(args.output,
            orientation="landscape",
            transparent=True,
            bbox_inches="tight")
