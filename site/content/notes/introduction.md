---
title: "Introduction and motivation"
weight: 1
katex: false
---

# Introduction

This course provides a brief introduction to parallel computing. It
focuses on those paradigms that are prevalent in scientific computing
in academia. There are other parallel programming models which we will
mention in passing where appropriate.

## Why parallel computing?

Computing in parallel is _more difficult_ than computing in serial. We
often have to write more code, or use more complicated libraries. If
we are going to commit to doing it, there must be some advantage.

Our goal when running scientific simulations or data analyses is often
to increase the resolution of the simulation, run more
replicates (for example more bootstrapping samples in a statistical
test), obtain answers faster, or some combination thereof.

In some cases, we might be analysing or simulating such
high-resolution data that it does not fit in the memory of a single
computer. Or, the problem may take so long that running it one a
single computer would require us to wait a week or longer for the
result.

TODO add some examples.

In such a case, our only solution is to find a way to parallelise the
computation, splitting up the work (and data) so that we can run parts
of our analysis on different computers and then combine these parts to
obtain the result we want.


## [Moore's Law](https://en.wikipedia.org/wiki/Moore%27s_law)

> The number of transistors in an integrated circuit doubles about
> every two years.

This was an observation Gordon Moore made about the _complexity_ of
chips in the
[mid-60s](https://newsroom.intel.com/wp-content/uploads/sites/11/2018/05/moores-law-electronics.pdf).
That is, every 18-24 months, the computational
performance available at a fixed price doubles.

This trend can be observed when we look at the performance of systems
in the [Top 500](https://www.top500.org), which has been measuring the
floating point performance of the largest 500 supercomputers in the
world since 1993.

FIXME: Add figure

This seems great, however, there is a catch. If we zoom in and look
at a history of CPU trends we see a slightly different picture.

FIXME: Add figure

Although the transistor counts still follow an exponential growth
curve, CPU clock speed stopped increasing in around 2004. As a
consequence, the rate at which the CPU executes instructions has not
improved. Herb Sutter makes some nice observations about this in [_The
Free Lunch is
Over_](http://www.gotw.ca/publications/concurrency-ddj.htm).

## Consequences

If the number of transistors continues to increase, but the effective
single-thread performance of CPUs does not (or at least, does not at
the same rate), what are all these extra transistors being used for?

The answer is that they are used to provide parallelism. Rather than
getting a single thread of execution on a CPU, we might have two,
four, or even 32 or 64.

The consequence for us as programmers is that to use all the performance
that chips provide, we must write code that exposes and uses
parallelism.

## Where next?

We'll introduce the different types of parallelism offered by modern
supercomputers, and look at potential programming models and
parallelisation strategies.
