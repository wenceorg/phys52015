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
