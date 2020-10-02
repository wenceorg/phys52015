---
title: "Parallel patterns"
weight: 4
---

# Overview of parallel patterns

So far, we've seen that to run large simulations, and exploit all of
the hardware available to us on supercomputers (but even on laptops
and phones), we will need to use parallelism of some kind.

We've also looked at the levels of parallelism exposed by modern
hardware, and noted that there are effectively three levels.

Now we're going to look at the types of parallelism (or _parallel
patterns_) that we might encounter in software. By recognising these
patterns, we'll be able to figure out what an appropriate
parallelisation strategy is.

In general, we can think of parallelism as finding _independent_
operations in the execution of a program. If two (or more) operations
are independent, then we can conceivably restructure our program such
that they execute _at the same time_ (in parallel).

## Data parallelism

Often, and we've already seen an example of this when looking at
vector addition, we have a program that performs the same operation
independently on many different data items.

This is very common in scientific computing. We often need to perform
the same (regular) operation on a large dataset and can parallelise
this by dividing the dataset up across many processors. Each processor
can then work on its own part of the problem.

Recall our simple example of addition of two vectors

```c
for (size_t i = 0; i < n; i++)
  a[i] = b[i] + c[i];
```

here we have three large arrays and each iteration of the loop is
independent. This is therefore a classic example of data parallelism.
In fact, data parallelism is often called _array parallelism_ by some
people.

Since all the iterations of the loop are independent, we could split
them up between processes in any way we like, but those may have
different performance characteristics. The usual choice is to evenly
divide the arrays into chunks and hand those chunks out to processes.

{{< manfig src="vector-addition-data-parallel.svg"
    width="50%"
    caption="Three-process data parallelism for vector addition." >}}


{{< exercise >}}

In the figure, we divided 16 array entries between three processes.
Assume that parallelisation is perfect (with no overheads) and that
computing a single addition takes 1 time unit.

How long would this addition take on one process? 

How long would you predict it to take on three processes?

Given the data decomposition sketched above, how long will the loop
take?

{{< /exercise >}}

If all the code we ever wrote was pointwise array operations like
this, a parallel programming course would be quite short, because now
we're done. Unfortunately (fortunately?) there is more to it than
this, because most interesting programs access data with some
interdependent footprint.

These are often still data parallel, but we typically need to think a
bit harder about how to expose and realise the parallelism.

For a simple example, consider the following loops

```c
while (...) {
  for (size_t i = 1; i < n-1; i++)
     a[i] = a[i+1] + a[i-1] - 2*a[i];
```

{{< details "Aside" >}} Access patterns like this occur in [finite
difference](https://en.wikipedia.org/wiki/Finite-difference_method)
approximations of <abbr title="partial differential
equations">PDEs</abbr> (of which more in
[COMP52215](https://www.dur.ac.uk/postgraduate.modules/module_description/?year=2020&module_code=COMP52215)
if you take it) or image processing.

This is an example [Gauß-Seidel](https://en.wikipedia.org/wiki/Gauss–Seidel_method) relaxation for the 1D Laplacian.

{{< /details >}}

At first glance, this looks like the previous simple data parallel
loop. But we have to be careful because we write into the same array
we read from. Naively just chunking up the loop will result in us
obtaining the wrong answer.

{{< manfig src="stencil-data-dependencies.svg"
    width="75%"
    caption="Data dependencies for the 1D Laplacian stencil" >}}
    
As we see from the figure above, updating an array entry needs values
from its two neighbours as well. If we were to chunk the loop
iterations up between processes as before, then on the boundaries
between processes we might end up reading the wrong value.

{{< exercise >}}

Can you think of any ways you could parallelise this loop?

Hint: think about colouring neighbouring entries in the array in
different colours. Do you see a pattern?

Does your parallel strategy produce the same answer as the serial
loop?

{{< /exercise >}}

{{< hint info >}}
This example demonstrates the idea that there is a difference between
parallelism _in the algorithm_ and parallelism _in the code_.

In fact, when attempting to implement parallel code it is often a good
idea to step back to pen-and-paper and consider the algorithm, looking
for parallelism there, rather than starting from a serial code and
looking for parallelisation opportunities.

{{< /hint >}}

### Summary

As mentioned, data parallelism is well suited to many parts of
scientific computing. It is easiest to implement when we can
statically decompose the data. That is, we can write a (possibly
data-dependent) algorithmic decomposition and assign work to processes
"up front".

When writing programs that are data parallel, it is a good idea to
think about using high-level constructs that make the data parallelism
explicit. For example, rather than writing stencil operations as
above, we might wrap them up in a library interface that explicitly
describes the data access.

The rationale for this is (at least) threefold:

1. It makes the parallelism explicit to a reader of the program;
1. We can usually design the library in such a way that the programmer
   can pretend they are writing serial code (which is easier to think
   about);
1. By capturing the data access patterns in code, we have a chance of
   reasoning about them algorithmically. This moves the
   task of parallelisation from the individual programmer to a
   library, and gives us an opportunity to apply more complicated
   optimisations.


## Task parallelism

Some algorithms do not lend themselves to the data parallel approach,
because the amount of available parallelism waxes and wanes through
the program. Many graph algorithms fall into this category.

For example, one can write a program to solve
[mazes](https://en.wikipedia.org/wiki/Maze) by converting the maze
into a graph where each decision point in the maze becomes a vertex
and decision points with paths between them become edges. We can now
visit the maze, trying to find the centre, by [breadth-first
search](https://en.wikipedia.org/wiki/Breadth-first_search) of the
graph. Mark Handley has a [nice
video](https://www.youtube.com/watch?v=pNLuKoTwMEE), although he is
doing [depth-first
search](https://en.wikipedia.org/wiki/Depth-first_search).

Imagine that you are visiting the maze but with a huge group of
friends. You start off all entering the maze at the same point. Every
time you come to a decision point, you split your group set off down
the different paths. If you encounter other members of your party
later, you merge groups. If you keep track of which points you've
visited (some you don't turn back on yourselves), some of you will
eventually reach the centre of the maze.

We can think of each of the groups walking through the maze as tasks.
When we come to a decision point, we create new tasks, and when a
group (task) reaches a point that has already been visited we remove
the task.

It is clear that the number of separate groups moving at any one time
in the maze changes, so if we're thinking of parallelism, we don't
have a static decomposition of data or work. It is instead dynamic.

This kind of parallelism where the workload changes dynamically can be
addressed with task parallelism. It is often more complicated to
handle than data parallelism. For example, we often need to change our
data structures, or design new ones: high-performance parallel data
structures for task parallelism are a [subject of ongoing
research](https://iss.oden.utexas.edu/?p=projects/galois).

{{< exercise >}}

Pseudo-code for breadth-first visit of a graph is

```python {linenos=table}
def bfs(G, root):
    """
    Visit all vertices in a graph G breadth-first reachable from root
    """
    seen = set()
    queue = Queue() # First-in first-out queue
    queue.add(root)
    seen.add(root)
    while not queue.empty():
        v = queue.pop()
        for w in G.edges(v): # edges in G from v to w
            if w not in seen:
                seen.add(w)
                queue.add(w)
```

Where do you think tasks are created?

Where are they removed?

Where (or why) might we have to be careful if using parallelism for
this algorithm?

{{< /exercise >}}

It turns out that the critical thing here for high-performance
implementations is the design of appropriate parallel data structures
that can handle irregular access. Along with sometimes rethinking the
way we implement the algorithms (by doing the moral equivalent of loop
reordering). If you are interested in finding out more on this kind of
irregular parallelism, and
for many more details than you need for this course, I recommend
starting with the homepage of the
[Galois](https://iss.oden.utexas.edu/?p=projects/galois) system.

