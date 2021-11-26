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

The notes contain exercises, please do attempt them as you go through.

{{< exercise >}}

Exercises look like this.

{{< /exercise >}}


## Slides/recordings

### 2021-11-26

We had an impromptu online session due to storm Arwen. We continued
with point to point messaging, and discussed a bit more about how
messages traverse through the network. Then we looked at [nonblocking
messages]({{< ref "notes/mpi/point-to-point-nb.md" >}}). I fixed some
of the typos from the live slides.

- [Slides]({{< static-ref "slides/mpi-nb.html" >}})
- [Video](https://durham.cloud.panopto.eu/Panopto/Pages/Viewer.aspx?id=5305d970-2a86-49f4-aa9d-adec012cf2bf)

### 2021-11-23

We started looking at programming with _distributed memory_
parallelism, and introduced the [MPI]({{< ref "mpi.md" >}})
library.

The MPI library has a lot of functions, and can be a bit overwhelming,
but please read through the [overview]({{< ref "mpi.md" >}}),
and [point-to-point messaging]({{< ref "notes/mpi/point-to-point.md"
>}}) notes.

- [Intro slides]({{< static-ref "slides/mpi-intro.html" >}})
- [Point to point slides]({{< static-ref "slides/mpi-ptp.html" >}})
- [Video](https://durham.cloud.panopto.eu/Panopto/Pages/Viewer.aspx?id=0f8a5e4f-cb37-45b3-bbd8-ade901493e85)

Do have a go at the exercises linked at the end of the point to point
slides before the next session. I will go through some solutions then.
### 2021-11-19

We introduced the concept of [parallel scaling]({{< ref
"notes/theory/scaling-laws.md" >}}), and looked at some examples of
[Amdahl's law]({{< ref "notes/theory/scaling-laws.md#amdahl" >}}).

- [OpenMP slides 4](https://rpubs.com/iamholger/837106)
- [OpenMP exercise 4](https://rpubs.com/iamholger/837117)
- [Video](https://durham.cloud.panopto.eu/Panopto/Pages/Viewer.aspx?id=aa88594b-79f9-41ba-b1af-ade50130f10a)

Hopefully you were able to produce some plots of the parallel
performance of the example code. What did you observe?

### 2021-11-16

We talked about [collectives]({{< ref "notes/openmp/collectives.md"
>}}), and in particular _reductions_. We also touched on [_data
races_]({{< ref "openmp.md#sync-data-race" >}}), and
[synchronisation]({{< ref
"notes/openmp/collectives.md#inter-thread-synchronisation" >}})
constructs you can use to avoid them.

- [OpenMP slides 3](https://rpubs.com/iamholger/836014)
- [OpenMP exercise 3](https://rpubs.com/iamholger/835619)
- [Video](https://durham.cloud.panopto.eu/Panopto/Pages/Viewer.aspx?id=b1ebdacf-6cf1-4634-b7ce-ade20145956d)

{{< hint info >}}

Understanding data races, and how to rework code to avoid them, is of
critical importance for writing correct OpenMP code. So I recommend
working through the [notes]({{< ref "notes/openmp/collectives.md" >}})
and [exercises]({{< ref "exercises/openmp-reduction.md" >}}) them to
check that you really understand what is going on.

{{< /hint >}}

### 2021-11-12

We continued with OpenMP, starting to introduce the concept of
[parallel regions]({{< ref "notes/openmp/intro.md" >}}) and [loop
parallelism]({{< ref "notes/openmp/loop-parallelism.md" >}}). We saw how to
control the number of threads in a parallel region using the
`OMP_NUM_THREADS` environment variable, as well as runtime control
with on the directives.

We then did some more OpenMP exercises.

- [OpenMP slides 2](https://rpubs.com/iamholger/834246)
- [OpenMP exercise 2](https://rpubs.com/iamholger/834247), [sketch solutions](https://rpubs.com/iamholger/833883)
- [Video](https://durham.cloud.panopto.eu/Panopto/Pages/Viewer.aspx?id=315b6af3-d5c7-4e49-864c-adde013eaa04)

{{< hint info >}}

A reminder that to transfer data to Hamilton, your best bet is to use
[`scp`]({{< ref "hamilton-quickstart.md" >}}) or similar (rather than
trying to copy and paste into a terminal editor). It is also
worthwhile getting [remote editing]({{< ref "remote.md" >}}) set up.

{{< /hint >}}

### 2021-11-09

We briefly introduced the course as a whole, then Holger provided an
introduction to the concepts of [shared memory]({{< ref "openmp.md"
>}}), and the OpenMP programming model.

We then spent the second half of the session starting to run some
things on Hamilton (with varying success). Most of you were able to
log in and run a very simple hello world example. Some people's
accounts had not been set up (sorry!), and there are instructions in
the slack channel about what to do in this case.

- [Intro slides]({{< static-ref "slides/intro.html" >}})
- [OpenMP slides 1](https://rpubs.com/iamholger/834248), [sketch solutions](https://rpubs.com/iamholger/831650)
- [OpenMP exercise 1](https://rpubs.com/iamholger/832782)
- [Video](https://durham.cloud.panopto.eu/Panopto/Pages/Viewer.aspx?id=600b4905-9539-4e5f-b849-adda0095585a)

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
