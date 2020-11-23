---
title: "Loop parallelism"
weight: 2
---

# OpenMP loop parallelism

With a parallel region and identification of individual threads, we
can actually parallelise loops "by hand".

Suppose we wish to divide a loop approximately equally between all the
threads, by assigning consecutive blocks of the loop iterations to
consecutive threads.

{{< manfig
    src="loop-chunking.svg"
    width="75%"
    caption="Distribution of 16 loop iterations across five threads." >}}

Notice here that the number of iterations is not evenly divisible by
the number of threads, so we've assigned one extra iteration to
thread0.

The code that distributes a loop in this way looks something like the below.

{{< code-include "openmp-snippets/hand-loop.c" "c" >}}

{{< exercise >}}
Run this code for a number of threads between 1 and 8. Convince
yourself that it correctly allocates all the loop iterations!
{{< /exercise >}}


UGH!

Fortunately, there is a better way[^1].

[^1]: Counterpoint, if you write large libraries using OpenMP you will
    probably end up managing the loop parallelism and distribution by
    hand in this way anyway.

## Worksharing constructs

Suppose we are in a parallel region, to distribute the work in a loop
amongst the thread team, we use the [`#pragma omp
for`](https://computing.llnl.gov/tutorials/openMP/#DO) directive.


```c
void foo(/* Some arguments */)
{
#pragma omp parallel default(none) ...
  {
    ...;

    /* Loop to parallelise */
    #pragma omp for
    for (int i = 0; i < N; i++) {
      ...;
    }
  }
}

```

This directive does the job of dividing the loop iterations between
the currently active threads. Without the directive, all threads in
the team would execute all iterations.

{{< details "Shorthand" >}}

This pattern of parallel region + loop is so common that there is a
separate directive that merges the two
```c
#pragma omp parallel for ...
for (...)
   ;
```
Is equivalent to
```c
#pragma omp parallel ...
{
  #pragma omp for
  for (...)
     ;
}
```

{{< /details >}}

{{< hint warning >}} 

For loop parallelism to be possible, these loops must obey many of
the same constraints we saw when looking [at vectorisation]({{< ref
"vectorisation.md" >}}). Formally, we need the loop to be of the form.

```c
for (var = init; var logical_op end; incr_expr)
  ...
```

Where the `logical_op` is one of `<`, `<=`, `>`, or `>=` and
`incr_expr` is an increment expression like `var = var + incr` (or
similar).

We are also not allowed to modify `var` in the loop body.

The compiler will complain about some of these issues, but maybe not
about others.


{{< /hint >}}


{{< exercise >}}

Try compiling and running the bad loop in the code below

Do you always get the same results on different numbers of threads?
What is wrong with the parallel loop?

{{< details Solution >}}
We don't see the same values printed independent of the number of
threads, indicating that we did something wrong.

The reason is that the loop body modifies the iteration variable `i`.
This is explicitly not allowed by the OpenMP standard (although the
compiler does not complain), because if we modify the iteration
variable, the compiler cannot figure out how to hand out the loops
between threads.
{{< /details >}}
{{< /exercise >}}
{{< code-include "openmp-snippets/bad-loop.c" "c" >}}

### Synchronisation

There is no synchronisation at the start of a `for` work-sharing
construct. By default, however, there _is_ a synchronisation at the
end.

To remove the synchronisation at the end, we can use an additional
`nowait` clause.

```c
#pragma omp parallel
{
#pragma omp for
  for (...) {
    ...;
  } /* All threads synchronise here */

#pragma omp for nowait
  for (...) {
    ...;
  } /* No synchronisation in this case */
}
```

### Doling out iterations {#loop-schedules}

The `for` directive takes a number of additional clauses that allow us
to control _how_ the loop iterations are divided between threads.

We do this with

```c
#pragma omp for schedule(KIND[, CHUNKSIZE])
```

Where `KIND` is one of

1. `static`
2. `dynamic`
3. `guided`
4. `auto`
5. `runtime`

and `CHUNKSIZE` is an expression returning a positive integer.

If you don't specify anything, this is equivalent to

```c
#pragma omp parallel for schedule(static)
```

In most cases, the static schedule is the most useful. We'll list the
properties of all of the choices below.

#### `static` schedules

`schedule(static)` is a block schedule. We divide the total iterations
into (approximately) equal chunks, one for each thread in the team,
and assign the chunks in order to threads.

{{< manfig
    src="static-schedule.svg"
    width="75%"
    caption="Iteration allocation for a static schedule with default chunksize." >}}

{{< details "What if there are leftover iterations?" >}}

If the number of iterations is not evenly divisible by the number of
threads, the OpenMP runtime is supposed to allocate the extra
iterations "approximately evenly", but it is not specified how this is
to be done. If you need a _specific_ distribution you need to write
code by hand.

{{< /details >}}

If specifying a chunk size (for example `schedule(static, 3)`) then
we have a block cyclic schedule. Iterations are doled out to threads
in order `chunksize` iterations at a time.

{{< manfig
    src="static-schedule-chunksize.svg"
    width="75%"
    caption="Iteration allocation for a static schedule with chunksize of three." >}}

`static` schedules deterministically allocate loop iterations to
threads. Two loops of the same length with the same static schedule
will dole out iterations in the same way.

#### `dynamic` schedules

`schedule(dynamic)` is a block cyclic schedule with a block size of
one. Iterations of the loop are doled out on a first-come-first-served
basis. That is as a thread becomes available, it is given the next
iteration.

`schedule(dynamic, chunksize)` is the same, only now the block size is
specified by `chunksize`.

{{< manfig
    src="dynamic-schedule-chunksize.svg"
    width="75%"
    caption="Example iteration allocation for a dynamic schedule with chunksize of three." >}}


`dynamic` schedules **do not** provide deterministic allocation of
loop iterations to threads loop iterations to
threads. Two loops of the same length with the same dynamic schedule
will **not necessarily** dole out iterations in the same way.

#### `guided` schedules

`schedule(guided)` is a block cyclic schedule where the blocks start
out large and get exponentially smaller, until they reach a minimum
size of one iteration. The size of a block is proportional to the
number of _remaining_ iterations divided by the number of threads.

`schedule(guided, chunksize)` is the same, only now the minimum size
of the chunks is specified by `chunksize`.

Chunks are doled out to threads on a first-come-first-served basis
(like the `dynamic` schedule).

#### `auto` and `runtime` schedules

The `auto` schedule leaves it up to the OpenMP runtime to decide how
to dole out iterations. It may implement some auto-tuning framework
where it measures performance of the loop with a given schedule and
then adjusts that in some clever way. Probably, the implementation
does not do anything clever.

The `runtime` schedule allows you to specify the schedule to use at
runtime (rather than compile time) by setting the `OMP_SCHEDULE`
environment variable to a valid schedule string. For example

```c
#pragma omp for schedule(runtime)
...
```

```
$ gcc -fopenmp -o code code.c
$ export OMP_SCHEDULE="static, 5"
$ OMP_NUM_THREADS=4 ./code # uses "static, 5" as the schedule
```

This is kind of pointless except for some small experiments because
there is only one value of the `OMP_SCHEDULE` variable, so _all_
`schedule(runtime)` loops must use the same value.

{{< exercise >}}

You now know enough (probably more than enough) to attempt the OpenMP
[loop exercise]({{< ref "/exercises/openmp-loop.md" >}}).

{{< /exercise >}}

### Controlling execution in a parallel region

Sometimes, we might want to serialise some part of the code in a
parallel region. OpenMP offers us two ways of doing this. If we don't
care about which thread executes some code, we can use [`#pragma omp
single`](https://computing.llnl.gov/tutorials/openMP/#SINGLE)

```c
#pragma omp parallel
{
  ...; /* Some stuff in parallel */
#pragma omp single
  {
    ...; /* Only one thread should do this */
  } /* All threads synchronise here unless nowait is specified. */
}
```

For example, we could use this construct to read in an input file
after some parallel setup on a single thread.

Alternatively, if we want thread0 to execute something, we can use
[`#pragma omp
master`](https://computing.llnl.gov/tutorials/openMP/#MASTER)

```c
#pragma omp parallel
{
  ...; /* Some stuff in parallel */
#pragma omp master
  {
    ...; /* Only thread0 does this */
  } /* No synchronisation */
}

/* Kind of pointless, because it's equivalent to */
#pragma omp parallel
{
  ...;
  if (omp_get_thread_num() == 0) {
    ...;
  }
}
```

## Summary

OpenMP provides a number of constructs for distributing work (in the
form of loop iterations) between threads in a team. This is achieved
with the `#pragma omp for` directive (inside an existing parallel
region).

Completely general for-loops are not allowed, because the runtime must
be able to determine the number of iterations (and how to get from one
iteration to the next) without executing the loop body.

There are a few ways of controlling how the iterations of the loop are
allocated to threads. These are called `schedule`s. Generally a
`static` schedule is best.

[Next]({{< ref "collectives.md" >}}) we'll look at how threads can
communicate with one another, so that we can do slightly more
complicated things than just share loop iterations.
