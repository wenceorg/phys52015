---
title: Introduction
draft: false
weight: 1
---

# PHYS52015: Introduction to High Performance Computing

This is the course webpage for the High Performance Computing part of
[PHYS52015]({{< modulepage >}}). It collects the exercises, syllabus,
and notes. The source repository is [hosted on GitHub]({{< repo >}}).

## Course organisation

The course will run over four weeks starting on 9th November 2020.
Each week there will be two sessions run via Teams, scheduled at 4pm
UK time on Mondays and Thursdays. These will start with a live summary
and comments on previous exercises, followed by an exercise session
where you can work through the exercises and get help from the
lecturers and tutors.

For each session, there will be some assigned reading from the
course notes. You should read through these notes and attempt the
exercises and questions.

{{< exercise >}}
Exercises look like this.
{{< /exercise >}}

You should make a list of questions you have on the notes (or from
doing the exercises). Where the exercises ask you to produce plots or
models, be prepared to show these to your group or the class (there
are no wrong answers here!).

In the live slots, I'll provide some high-level commentary and discuss
any particular knotty points that have come up, we'll then also go
through questions you have on the latest reading.

In the second half (approximately) of the session, we'll work on the
programming exercises. You're encouraged to collaborate in groups and
ask questions of the lecturers and tutors if you get stuck, or want
some clarification.

### Reading for 2020-11-23

This week we'll start looking at MPI. For the Monday session we will
look at basic concepts and point-to-point messaging with blocking
communication. Please therefore look at

1. [The MPI overview]({{< ref "mpi.md" >}})
1. [Point to point messaging]({{< ref "notes/mpi/point-to-point.md"
   >}})

In the live session, we'll look at the mini exercises from those notes
and then work on the exercise sending [messages round a ring]({{< ref
"mpi-ring.md" >}}) and [calculating π]({{< ref "mpi-pi.md" >}}) using
a very simple to parallelise Monte Carlo method.

### Reading for 2020-11-16 & 2020-11-19

This week, we will cover OpenMP. For the Monday session, please look
through the introductory sections on OpenMP and on loop parallelism:

1. [Overview of shared memory programming]({{< ref "openmp.md" >}})
1. [What OpenMP is]({{< ref "notes/openmp/intro.md" >}})
1. [Loop parallelism in OpenMP]({{< ref "notes/openmp/loop-parallelism.md"
   >}})

and again have a go at the boxed exercises. In the live session we'll
work through the first two OpenMP exercises on [parallel loops]({{<
ref "openmp-loop.md" >}}) and [stencils]({{< ref "openmp-stencil.md"
>}}). Both of these exercises look at some strong scaling, so you may
wish to remind yourself of [that]({{< ref "scaling-laws.md" >}})

For the Thursday session, we'll look at synchronisation constructs:
how multiple threads can collaborate when producing a shared result.
This type of operation can broadly be termed ["collective"]({{< ref
"notes/openmp/collectives.md" >}}). In the live session we will
continue with the first two OpenMP exercises (if you still have
questions) and also look at the [synchronisation exercise]({{< ref
"openmp-reduction.md" >}}).

### Reading for 2020-11-12

Please look through the two sections on vectorisation:

1. [overview]({{< ref "vectorisation.md" >}}); and
2. [compiler autovectorisation]({{< ref "compiler.md" >}}).

and attempt the boxed exercises. To see where this fits in the levels
of parallelism we will encounter in the course, you could also look at
the background on [parallelism in hardware]({{< ref
"hardware-parallelism.md" >}}).

In the live session we will work through the two vectorisation
exercises on [loops]({{< ref "vectorisation-loop.md" >}}) and
[stencils]({{< ref "vectorisation-stencil.md" >}}), as well as
covering any questions or comments you had about the notes and your
observations and answers to the boxed exercises.

## Syllabus

- A brief [introduction to parallel computing]({{< ref
  "introduction.md" >}}), and its necessity
- [Scaling laws]({{< ref "scaling-laws.md" >}})
- Available parallelism in [modern supercomputers]({{< ref
  "hardware-parallelism.md" >}})
- [Vectorisation and loop parallelism]({{< ref "vectorisation.md" >}})
- Shared memory parallelism, with [OpenMP]({{< ref
  "openmp.md" >}})
- Distributed memory parallelism, with [MPI]({{< ref  "mpi.md" >}})

### Assessment

Via a single piece of [summative coursework]({{< ref "coursework.md"
>}}).


## Lecturers

- [Lawrence Mitchell](mailto:lawrence@wence.uk)

- [Christian Arnold](https://www.dur.ac.uk/physics/staff/profiles/?mode=staff&id=16712)
