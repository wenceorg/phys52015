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
either to increase the resolution of the simulation, run more
replicates (for example more bootstrapping samples in a statistical
test), or obtain more answers faster.

In some cases, we might be analysing or simulating such
high-resolution data that it does not fit on a single computer.

TODO add some examples.

In such a case, our only solution is to find a way to parallelise the
computation, splitting it up so that the pieces fit on a single
computer.

Another reason we might want parallel computing is because we want the
answer to our simulation question faster. Until twenty years ago, the
solution to this problem was often to wait a year until we could buy a
faster computer. Unfortunately, this is no longer possible.
