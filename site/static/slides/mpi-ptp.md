---
title: "MPI: point to point"
---

# MPI: point to point messages

<div class="center">

[Notes](/phys52015/notes/mpi/point-to-point)

</div>

------

## Main idea

- Two-sided communication (sender and receiver)
- Processes identified by `(communicator, rank)` pair
- Need to "describe" data to the library
  - How much data to send, where to get it from
  - How much data to receive, where to put it

------

## Pointers and datatypes

```c
int MPI_Send(const void *buffer, int count, MPI_Datatype dtype,
     int dest, int tag, MPI_Comm comm);

int MPI_Recv(void *buffer, int count, MPI_Datatype dtype,
     int src, int tag, MPI_Comm comm, MPI_Status *status);
```

- To pass data to library, pass _pointer_ to first element

```c
/* Sending from an array */
int *data = ...;
MPI_Send(data, ...);

/* Sending a scalar value */
double scalar = 10;
MPI_Send(&scalar, ...);
```

--->

### How does MPI know what to send?

- `count` and `datatype` arguments
  - `MPI_Datatype` describes single "unit"
  - `count` how many copies of the unit described by the datatype?
  
| C type   | MPI_Datatype |
|:---------|:-------------|
| `char`   | MPI_CHAR     |
| `int`    | MPI_INT      |
| `float`  | MPI_FLOAT    |
| `double` | MPI_DOUBLE   |

--->

### Example

```c
/* Single scalar value */
double value = ...;
MPI_Send(&value, 1, MPI_DOUBLE, ...);
```

```c
/* Second entry of array */
double *array = ...;
MPI_Send(&array[1], 1, MPI_DOUBLE, ...)
```

```c
/* Five entries of the array */
double *array = ...;
MPI_Send(array, 5, MPI_DOUBLE, ...)
```

--->

### _No type-checking_

- Datatype description is _independent_ of actual type of data buffer
- If you describe your data incorrectly, MPI won't complain
  - You get to keep both pieces
  
```c
int value = ...;
/* MPI will not complain, buffer overflow */
MPI_Send(&value, 5, MPI_INT, ...);
```

------

## Message matching

- Messages sent/received over a communicator: `(rank, comm)` pair
- Messages with the same `(rank, comm)` pair can be distinguished by
  _tags_
- No matching on count or datatype (can differ on sender and receiver)
- No message overtaking

--->

### Example

```c
if (rank == 0) {
  /* This message is matched ... */
  MPI_Send(..., 1 /* dest */, 0 /* tag */, MPI_COMM_WORLD);
} else if (rank == 1) {
  /* ... here */
  MPI_Recv(..., 0 /* src  */, 0 /* tag */,
           MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}
```


--->


### Example: mismatch

```c
if (rank == 0) {
  /* This message is not matched ... */
  MPI_Send(..., 1 /* dest */, 0 /* tag */, MPI_COMM_WORLD);
} else if (rank == 1) {
  /* ... the tag is different */
  MPI_Recv(..., 0 /* src  */, 1 /* tag */,
           MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}
```

------

## Deadlocks

- Ordering of sends and receives is important

```c
if (rank == 0) {
  MPI_Send(sendbuf, nentries, MPI_INT, 1, 0, ...);
  MPI_Recv(recvbuf, nentries, MPI_INT, 1, 0, ...);
} else if (rank == 1) {
  MPI_Send(sendbuf, nentries, MPI_INT, 0, 0, ...);
  MPI_Recv(recvbuf, nentries, MPI_INT, 0, 0, ...);
}
```

- Deadlock (both waiting for a receive to appear)

- Exercise:
  [deadlocks](/phys52015/notes/mpi/point-to-point/#deadlock-exercise)
  
---

## Over to you

- MPI [hello world](/phys52015/exercises/hello/#mpi)
- Exercises in [point to point
  notes](/phys52015/notes/mpi/point-to-point/)
- Messages [around a ring](/phys52015/exercises/mpi-ring/)
