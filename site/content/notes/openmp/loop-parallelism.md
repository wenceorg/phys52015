---
title: "Loop parallelism"
weight: 2
---

# OpenMP loop parallelism

With a parallel region and identification of individual threads, we
can actually parallelise loops "by hand".

{{< code-include "openmp-snippets/hand-loop.c" "c" >}}

UGH!

Fortunately, there is a better way.
