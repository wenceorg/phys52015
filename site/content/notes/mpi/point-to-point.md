---
title: "Point-to-point messaging in MPI"
weight: 7
---

# Pairwise message exchange

The simplest form of communication in MPI is a pairwise exchange of a
message between two processes.

In MPI, communication via messages is _two-sided_[^1]. That is, for every
message one process sends, there must be a matching receive call by
another process.

Let's look at an example. Suppose we have two processes, and we want
to send a message from rank 0 to rank 1.

{{< code-include "mpi-snippets/send-message.c" "c" >}}

[^1]: MPI does have some facility for one-sided message passing, but
      we won't cover it in this course.

Motivation

What is it

How does it work

Some API

Point to point basics
