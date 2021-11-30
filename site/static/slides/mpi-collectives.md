---
title: "MPI: more point to point"
---

# MPI: collectives

<div class="center">

[Notes](/phys52015/notes/mpi/collectives/)

</div>

------

## So far

- Two-sided communication (sender and receiver)
- Processes identified by `(communicator, rank)` pair
- Point to point messages: one sender, one receiver

What if the communication pattern requires "everyone-to-everyone"
communication?

------

## Prototypical example

- Iterative solution of linear equations

    $$ A x = b $$
    
- Richardson point iteration

    $$ x_\text{new} = x_\text{old} - (b - A x_\text{old}) $$
    
- Converged when $\| x \|_2 < \epsilon$ for some tolerance $\epsilon$


--->

### How to compute $\| x \|_2$ ?

$$
\| x \|_2 = \sqrt{\sum_i x_i^2}
$$

- Serial implementation

```c
void norm(double *x, int n, double *result)
{
  double tmp = 0;
  for (int i = 0; i < n; i++) {
    tmp += x[i];
  }
  tmp = sqrt(tmp);
  *result = tmp;
}
```

--->

### Does this work in parallel with MPI?

- No, why not?
  - Each process only has _part_ of the vector
  ![Sketch of parallel
  summation](/phys52015/images/manual/vec-sum-sketch.svg)
  - Need to _combine_ partial results
  
--->

### But I [already did that](/phys52015/exercises/mpi-ring/)!

- Implemented the combine step in the "messages around a ring"
  exercise
  - Only for summation
- Was this an _efficient_ approach?
  1. For 2 processes?
  2. For 20 processes?
  3. For 20,000,000 processes?
  
--->

### How to measure "efficient"

- How much local work we do?
- How many messages need to be sent?
- What is the longest "chain" of messages?
- Suppose we have $P$ processes
- $n$ entries per process (for $N = nP$ total entries)

--->

- Work: $n$ values to be summed
   - independent of $P$ ✅
- Messages: each process sends $P-1$ messages
   - grows linearly with $P$ ❌
   - $\mathcal{O}(P^2)$ total messages ❌
- Longest chain: $P$ messages long

--->

### Consequence

- Recall model for message time is $T(B) = \alpha + \beta B$
- For a single double, on Hamilton, $T = 5\mu s$
- Time for the algorithm we implemented is
$$
T_\text{ring}(B) = (P - 1) T(B)
$$
- If $P = 20,000,000$, this is **100 seconds** 😞
  - Adding 20,000,000 numbers on my laptop with Python takes 8
    microseconds
    
------

## A better way

- Combine using a _tree_
- Classic example of divide-and-conquer turning a linear algorithm
  into a logarithmic one

![Tree reduction](/phys52015/images/manual/mpi-tree-reduce.svg)


--->

### Analysis

- Restrict to $P$ a power of two (simplifies equations...)
- Depth of tree is $\log_2 P$
- Total messages
  - Finest level, send $P$ messages
  - Number of messages reduces by factor of 2 on every level
  $$
  M_\text{total} = \sum_{i=0}^{\log_2 P} \frac{P}{2^i}
  $$

--->

### Large $P$ limit

- [Geometric
  series](https://en.wikipedia.org/wiki/Geometric_series#Closed-form_formula)
  $$
  \lim_{P \to \infty} M_\text{total} = 2 P
  $$
- Longest chain $\log_2 P$ ✅
- Average number of messages per process $2$
  - Independent of $P$ ✅
  - Total messages $\le 2P$ ✅
  
--->

### Consequence

- Time for a tree reduction is
  $$
  T_\text{tree}(B) = 2 T(B) \log_2 P
  $$
- Factor of two because we go down and back up the tree
- If $P = 20,000,000$, this is ~0.2 microseconds
  - Factor of 500,000 improvement! 😊
  
------

## How do I do this?

- Simple case not too difficult
  - This is what [the
    coursework](/phys52015/coursework//#task-implement-tree_allreduce)
    asks you to do
- General cases harder
  - Depending on network, a hypercube reduction might be faster
- Use MPI collectives

--->

### Many collectives in MPI

- Data described in same way as before
- **No source/destination**
- Every process in the communicator participates
- Combination specified through `MPI_Op` operator
  - Can define your own


--->
```c
int MPI_Allreduce(const void *sendbuf, void *recvbuf, 
                  int count, MPI_Datatype datatype,
                  MPI_Op op, MPI_Comm comm);
```

![Sketch of allreduce](/phys52015/images/manual/mpi-allreduce.svg)

--->

```c
int MPI_Scan(const void *sendbuf, void *recvbuf, 
             int count, MPI_Datatype datatype,
             MPI_Op op, MPI_Comm comm);
```

![Sketch of scan](/phys52015/images/manual/mpi-scan.svg)

--->

### Combining operators

- Provide flag telling MPI how to combine
- Here are some useful builtin ops

| Description    | Operator                                                  | Identity element                   |
|----------------|-----------------------------------------------------------|------------------------------------|
| Addition       | [`MPI_SUM`](https://rookiehpc.com/mpi/docs/mpi_sum.php)   | 0                                  |
| Multiplication | [`MPI_PROD`](https://rookiehpc.com/mpi/docs/mpi_prod.php) | 1                                  |
| Minimum        | [`MPI_MIN`](https://rookiehpc.com/mpi/docs/mpi_min.php)   | Most positive number of given type |
| Maximum        | [`MPI_MAX`](https://rookiehpc.com/mpi/docs/mpi_max.php)   | Most negative number of given type |

------

## One to all communication

- Example: one process reads information from a file

```c
int MPI_Bcast(void *buffer, 
              int count, MPI_Datatype datatype, 
              int root, MPI_Comm comm);
```

![Sketch of broadcast](/phys52015/images/manual/mpi-bcast.svg)

--->

### Memory management

- Value in buffer only important on `root` rank
- But everyone must provide space!

```c
int *buffer = malloc(10*sizeof(*buffer));
if (rank == 0) {
   /* Initialise values from somewhere (e.g. read from file) */
   ...
}
MPI_Bcast(buffer, 10, MPI_INT, 0, comm);
```

--->

### Scatters

- Generalises broadcast (each rank gets its own information)
- `sendcount` is count _per process_

```c
int MPI_Scatter(const void *sendbuf, 
                int sendcount, MPI_Datatype sendtype,
                void *recvbuf, 
                int recvcount, MPI_Datatype recvtype,
                int root, MPI_Comm comm);
```

--->

![Sketch of scatter](/phys52015/images/manual/mpi-scatter.svg)

--->

### Gathers

- Inverse of `MPI_Scatter`, `recvcount` is count _per process_
- Everyone sends, one process receives

```c
int MPI_Gather(const void *sendbuf, 
               int sendcount, MPI_Datatype sendtype,
               void *recvbuf, 
               int recvcount, MPI_Datatype recvtype,
               int root, MPI_Comm comm);
```

--->

![Sketch of gather](/phys52015/images/manual/mpi-gather.svg)

------

## And more ...

- Additional collectives for all-to-all communication
- See [notes](/phys52015/notes/mpi/collectives/) for more details
- Not touched on
  - "Neighbourhood collectives"
  - "Vector" versions of Scatter/Gather where everyone sends a
    different amount of data: [image reconstruction
    exercise](/phys52015/exercises/mpi-stencil/)
    
------

## Summary

- MPI offers a rich array of _collectives_
- "Symmetric": every process in the provided communicator participates
- Can [manipulate
    communicators](/phys52015/notes/mpi/advanced/#communicators) to
    split/combine

Exercises:
- [Simple collectives](/phys52015/exercises/mpi-collectives/)
- [Domain decomposition](/phys52015/exercises/mpi-stencil/)

------

## Bonus: exercise solutions, coursework

- Repository contains code solutions for many of the exercises
- Added links to solutions for Holger's OpenMP exercises
- Will add some sketch textual solutions to (most of) the exercises in
  the notes
- I still encourage you to try things for yourself (even with
  solutions in hand)
  
--->

### Coursework

- Has been available [for a while](/phys52015/coursework/)
- Submission deadline 10th January
- Two implementation parts
  1. OpenMP (parallel Gauss-Seidel)
  2. MPI (tree reduction)
- MPI part then asks you to do benchmarking
  - **DO NOT** use the frontend node, submit batch jobs
  - Leave yourself enough time to get the runs through

--->

### Writing up

- As well as code, I ask you to write up your experiments
- [Generic
  descriptors](https://durhamuniversity.sharepoint.com/teams/MScScientificComputingandDataAnalysis/SitePages/Written-Work-Descriptors-%28Non-Dissertation%29.aspx)
  for reports apply
- I will upload (on blackboard) some anonymised reports from last
  year, along with my marks/feedback
  
