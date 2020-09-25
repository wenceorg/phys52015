---
title: "Scaling laws: how fast might my program go?"
weight: 2
katex: true
chartjs: false
---

# Scaling laws (models)

Suppose we have a simulation that has been parallelised. What should
we expect of its performance when we run it in parallel? Answering
this question will allow us to determine an _appropriate_ level of
parallelism to use when running the simulation. It can also help us
determine how much effort to put into parallelising a serial code, or
improving an existing parallel code.

## A thought experiment

Before we look at what other people have come up with, let's see if we
can construct a simple model of parallel performance.

{{< exercise >}}

Write down a formula (model) for the time to solution on \\(P\\) computers,
given that a program takes \\(T\\) seconds on one computer.

{{< question >}}

Given your model, how would you define _speedup_ and _efficiency_?

What assumptions did you make in constructing your model?

Can you think of any reasons why the observed speedup in "real life"
might be lower than predicted by your model?

{{< /question >}}

{{< /exercise >}}

## Amdahl's law and strong scaling

To construct some models of scaling, we first need some notation. We
also need to decide what we mean by "performance". We will generally
use "useful work per second" as the unit of performance.

Let \\(F_s\\) be the _serial fraction_ and \\(F_p := 1 - F_s\\) the
_parallel fraction_ of a given code and suppose that a problem takes
\\(T_1\\) seconds on a single process.

We can write the total parallel execution time on \\(P\\) processors
as

$$
T_P := T_1 \left(F_s + \frac{F_p}{P}\right).
$$

Writing the speedup as
$$
S_P := \frac{T_1}{T_P}
$$

we observe that the maximum speedup we can obtain is

$$
S_\infty = \lim_{P \to \infty} \frac{T_1}{T_1\left(F_s +
\frac{F_p}{P}\right)} = \frac{1}{F_s}.
$$

This phenomenon is known as [Amdahl's
law](https://dl.acm.org/doi/10.1145/1465482.1465560) after [Gene
Amdahl](https://en.wikipedia.org/wiki/Gene_Amdahl).

{{< exercise >}}

Consider a code that has a 2 second serial setup phase, and then
carries out a parallelisable computation for 1200 seconds on a single
processor.

What does Amdahl's law predict the speedup and efficiency to be when
executed on

1. 100 processors;
2. 500 processors?

{{< /exercise >}}

{{< exercise >}}

Consider the implications of Amdahl's law. Suppose we wish to maintain
a fixed parallel efficiency of 80%. How does the parallel fraction of
the code have to increase as a function of the number of processors?

{{< /exercise >}}

This type of parallel scaling, where we _fix_ the problem size and add
more compute resources, is termed _strong scaling_. It is appropriate
to consider when what we want to do is take a fixed size problem and
ask ourselves how fast we might possibly run it.

Amdahl's law seems to paint a rather disappointing picture. If I have
even 1% serial fraction in my code, the best possible speedup I can
expect from applying parallelism is 100.

Fortunately, there is an implicit assumption hiding in Amdahl's model,
which is that we are only ever interested in _fixed_ problem sizes. In
typical use, this is not the case, since there is often a way we can
increase the size of a problem. For example, we might add more
resolution to a weather forecast (so that we have more accurate
predictions of where _exactly_ in Durham it will be raining [^1]).

[^1] Some might claim this is easy, namely that it is always raining
everywhere in Durham.

This leads us to a different way of thinking about parallel scaling
and efficiency.

## Gustafson's law and weak scaling

Rather than starting with sequential code and asking how it will
speedup when parallelising it, here we start with a problem that runs
in parallel, again with known serial and parallel fractions. Now we
ask ourselves if we had run the code in serial, how long would it have
taken?

That is, we know \\(T_P\\), the time on \\(P\\) processors, and we
derive the time on one processor

$$
T_1 = T_P F_s + P F_p T_P.
$$

Here, we used that the parallel splits into a serial part and a
parallelisable part. The serial part contributes the same to the runtime in
serial or parallel, in serial the parallelisable part takes \\(P\\)
times as long one one process as on \\(P\\) processes.

Again, we can write the speedup

$$
S_P = \frac{T_1}{T_P} = F_s + P F_p = P + (1 - P)F_s.
$$

This approach models the setup where we have a serial fraction that is
independent of the problem size, and a parallel fraction that we can
replicate arbitrarily.

This type of parallel scalability is called _weak scaling_. In this
scenario we fix the problem size _per process_. It is an appropriate
thing to consider when we have a fixed runtime we are targetting and
are asking how large a problem we could run.
