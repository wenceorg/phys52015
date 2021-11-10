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

The course will run over four weeks starting on 9th November 2021.
Each week there will be two sessions, scheduled at 4pm UK time on
Tuesdays and Fridays in TLC025.
{{< hint info >}}
You can attend remotely over
[zoom](https://durhamuniversity.zoom.us/j/97932635844?pwd=aG9JNThyYTVBSkJKMlVRVUxKNzM2QT09),
and will need to be authenticated with your Durham account.

Meeting ID: 979 3263 5844
Passcode: 371456
{{< /hint >}}

The sessions will be a combination of short lectures, discussion, and
practical work on the exercises in small groups, with tutors to help.
Please bring a laptop along to the sessions if you can.

We will use the same [slack
channel](https://blackboard.durham.ac.uk/ultra/courses/_5721_1/outline)
that you have been using for the scientific computing part of the
course to facilitate discussion.

The notes contain
{{< exercise >}}
Exercises which look like this.
{{< /exercise >}}
please do attempt them as you go through.

## Slides/recordings

### 2021-11-09

We [briefly introduced]({{< static-ref "slides/intro.html" >}}) the
course as a whole, then Holger provided an introduction to the
concepts of [shared memory]({{< ref "openmp.md" >}}), and the OpenMP
programming model. This part of the session is available as a
[video](https://durham.cloud.panopto.eu/Panopto/Pages/Viewer.aspx?id=600b4905-9539-4e5f-b849-adda0095585a)
on panopto.

We then spent the second half of the session starting to run some
things on Hamilton (with varying success). Most of you were able to
log in and run a very simple hello world example. Some people's
accounts had not been set up (sorry!), and there are instructions in
the slack channel about what to do in this case.

## Syllabus

- A brief [introduction to parallel computing]({{< ref
  "introduction.md" >}}), and its necessity
- [Scaling laws]({{< ref "scaling-laws.md" >}})
- Available parallelism in [modern supercomputers]({{< ref
  "hardware-parallelism.md" >}})
- Shared memory parallelism, with [OpenMP]({{< ref
  "openmp.md" >}})
- Distributed memory parallelism, with [MPI]({{< ref "mpi.md" >}})

### Assessment

Via a single piece of [summative coursework]({{< ref "coursework.md" >}}).


## Lecturers

- [Lawrence Mitchell](mailto:lawrence.mitchell@durham.ac.uk)
- [Holger Schulz](https://www.ippp.dur.ac.uk/~hschulz/)
