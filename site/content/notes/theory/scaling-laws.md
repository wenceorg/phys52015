---
title: "Parallel scaling laws"
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

## Some simple examples[^3]

[^3]: These examples are adapted from section 2.1 of Victor Eijkhout's
    [Introduction to High Performance Scientific
    Computing](https://pages02.tacc.utexas.edu/istc/istc.html),
    reproduced under [CC-BY
    3.0](https://creativecommons.org/licenses/by/3.0/).

The type of parallelism we will cover in this course is that where we,
as programmers, are explicitly in control of what is going on. That
is, we decide how to divide up work between processes, and how to
schedule that work. We will generally focus on programming models that
enable "tightly coupled" parallelism. That is, we have a single task
that we want to split up (so that we can get the answer faster, or ask
a larger question, or both).

Here's a very simple example, adding together two vectors of length
\\(n\\). The core loop of this operation written in C looks like

```c
for (size_t i = 0; i < n; i++)
  a[i] = b[i] + c[i];
```

We can carry out this loop on up to \\(n\\) processors [^2]. A more
general strategy would be to chunk the loop up into \\(p\\) pieces (if
we have \\(p\\) processors, and let each processor handle part of the
loop

```c
size_t start = figure_out_start(...)
size_t end = figure_out_end(...)
for (size_t i = start; i < end; i++)
  a[i] = b[i] + c[i];
```

[^2]: It is probably not efficient to use that many!

So the execution time is linearly reduced with the number of
processors. Suppose that each addition takes one time unit. In serial,
our code takes time \\(n\\), and parallel execution takes time
\\(\frac{n}{p}\\), and is a factor of \\(p\\) faster.

This algorithm didn't really require any reworking to run in parallel.
Let's now look at example that does.

Consider, instead of adding two vectors pointwise, computing the dot
product of two vectors

$$
s = a \cdot b =: \sum_{i=1}^n a_i b_i
$$

In serial, the C code looks familiar and straightforward

```c
s = 0;
for (size_t i = 0; i < n; i++)
  s += a[i] * b[i];
```

Now parallelisation does not look as straightforward, since there is a
[loop carried
dependency](https://en.wikipedia.org/wiki/Loop_dependence_analysis).
Iteration 1 requires that iteration 0 is finished, iteration 2
requires that iteration 1 is finished, and so forth.

However, if we are willing to pay some more storage, we can rework
this single loop into the following.

```c {linenos=table}
double tmp[n];
/* We can parallelise this as before */
for (size_t i = 0; i < n; i++)
  tmp[i] = a[i] * b[i];

for (size_t s = 2; s < 2*n; s *= 2)
  for (size_t i = 0; i < n - s/2; i += s)
    tmp[i] += tmp[i + s/2];

s = tmp[0]
```

First we compute the pointwise product, then we perform some partial
sums. The inner loop on line 7 is now a loop with \\(\frac{n}{s}\\)
iterations that can be carried out in parallel. The outer loop now has
\\(\log_2 n\\) iterations. So if we parallelise the inner loop, we
have a parallel runtime of \\(\frac{n \log_2 n}{p}\\). So the speedup
is \\(frac{p}{\log_2 n}\\).


Even for these two very simple examples we can make some
observations.

1. Sometimes, we might need to rewrite an algorithm slightly to expose
   parallelism;
2. A parallel algorithm may not exhibit perfect speedup. That is, we
   may not get a performance increase of \\(p\\) when we employ
   \\(p\\) processors.

In these examples we note that we've rewritten them to expose
potential parallelism, but haven't looked yet at how to realise that
parallelism. We'll do so when we introduce programming models.

## A thought experiment

In the previous examples, we set up a problem where we had some piece
of code and needed to parallelise it. We saw that even if the code is
perfectly parallel, the potential speedup may be less than perfect.

The examples we showed were not whole programs, and the real world is
far from perfect, so we might expect that more realistic performance
models are more complicated. Before we look at two of them, let's see
if we can construct a model for ourselves.

The setting we have in mind is that we have some computation that
requires \\(N\\) units of work, taking \\(T\\) seconds on a single
process.

{{< exercise >}}

Write down a formula (model) for the time to solution on \\(P\\) processes,
given that a program takes \\(T\\) seconds on one computer.

{{< question >}}

Given your model, how would you define _speedup_ and _efficiency_?

What assumptions did you make in constructing your model?

Can you think of any reasons why the observed speedup in "real life"
might be lower than predicted by your model?

{{< /question >}}

{{< /exercise >}}

## Amdahl's law and strong scaling

This model considers a fixed problem size. That is, we fix the \\(N\\)
units of work that we want to perform. It then makes an assumption
about how much of that work is (potentially) parallelisable and
attempts to answer the question of what speedup we might obtain if we
were able to parallelise that part of the code perfectly.

Let \\(F_s\\) be the _serial fraction_ and \\(F_p := 1 - F_s\\) the
_parallel fraction_ of a given code and suppose that a problem takes
\\(T_1\\) seconds on a single process.

{{< manfig src="serial-fraction.svg"
    width="75%"
    caption="Division of a code into serial and parallelisable parts." >}}

We assume that we can perfectly parallelise the parallel fraction, and
so the total parallel execution time on \\(P\\)
processors is

$$
T_P := T_1 \left(F_s + \frac{F_p}{P}\right).
$$

Defining the speedup as the ratio
$$
S_P := \frac{T_1}{T_P}
$$

we observe that the maximum speedup we can obtain is

$$
S_\infty = \lim_{P \to \infty} \frac{T_1}{T_1\left(F_s +
\frac{F_p}{P}\right)} = \frac{1}{F_s}.
$$

This phenomenon, where the maximum speedup is limited by the serial
fraction of the code, is known as [Amdahl's
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
more compute resources, is termed _strong scaling_.  That is, fix have
a fixed total work \\(N\\) and each process performs a decreasing
fraction of the work \\(\frac{N}{P}\\) as we add more processes.

This is an appropriate metric to consider when what we want to do is
take a fixed size problem and ask ourselves how fast we might possibly
run it.

Amdahl's law seems to paint a rather disappointing picture. If I have
even 1% serial fraction in my code, the best possible speedup I can
expect from applying parallelism is 100.

{{< manfig src="strong-scaling.svg"
           width="75%"
           caption="Strong scaling eventually runs into Amdahl's law. The serial fraction eventually dominates the computation time." >}}

Fortunately, there is an implicit assumption hiding in Amdahl's model,
which is that we are only ever interested in _fixed_ problem sizes. In
typical use, this is not the case, since there is often a way we can
increase the size of a problem. For example, we might add more
resolution to a weather forecast so that we have more accurate
predictions of where _exactly_ in Durham it will be raining[^1].

[^1]: Some might claim this is easy, namely that it is always raining
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
scenario we fix the problem size _per process_. In this scenario, the
total work \\(N\\) increases when we add more processes, since we
require that the ratio \\(\frac{N}{P}\\) is constant as we change \\(P\\).


This metric is an appropriate one to consider when we have a fixed
runtime we are targetting and are asking how large a problem we could
run (by adding more compute resources).

{{< manfig src="weak-scaling.svg"
    width="75%"
    caption="Perfect weak scaling matches the number of processes to the problem size, giving constant runtime for increasing problem sizes by adding more processes." >}}


{{< exercise >}}
Produce plots of the normalised speedup as given by the two approaches
for a range of serial fractions from \\( F_s = 0.01 \\) to \\( F_s =
0.5 \\). 

Compare the behaviour of the speedup curve over a range of processes
from 1 to 1000.

What observations can you make?
{{< /exercise >}}

## Pitfalls and shortcomings

These two speedup models are our first encounter with _performance
models_. They can be useful, but we should note some of their
shortcomings.

First, they are very simplistic. They suppose that a (potentially
complicated) program can be divided into two separate parts which may
then be treated independently. In fact, it is very likely that
changing a program such that it runs in parallel will effect the
execution of serial parts of that same program on real hardware. This
might be, for example, due to [frequency
scaling](https://travisdowns.github.io/blog/2020/08/19/icl-avx512-freq.html)
or [cache
effects](https://igoro.com/archive/gallery-of-processor-cache-effects/).

Another issue is that these models do not consider any overhead that
comes from introducing parallelism. We will see some examples of this
later in the course.

Some of these problems, especially in relation to Amdahl's law, are
discussed in more detail by Michael Wolfe in an [article on
hpcwire](https://www.hpcwire.com/2015/01/22/compilers-amdahls-law-still-relevant/).

We should also note that speedup in these models is reported relative
to the serial performance. Can you think of why that might be a bad idea?

{{< exercise >}}

Can you think of any ways that you could "cheat" these scaling models?

For example, how could you improve the scalability without actually
improving the time to solution?

Hint: is scalable code good code? What information does a scaling plot
_not_ give you?

{{< /exercise >}}

## Summary

We considered two different ways of measuring the speedup of a
parallel program. Which one is most relevant depends on what the
properties of the problem you are trying to parallelise are.
