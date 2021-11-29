---
title: "MPI: more point to point"
---

# MPI: more point to point

<div class="center">

[Notes](/phys52015/notes/mpi/point-to-point/) and
[more](/phys52015/notes/mpi/point-to-point-nb/)

</div>

------

## Reminder

- Two-sided communication (sender and receiver)
- Processes identified by `(communicator, rank)` pair
- Data passed as void pointer
- Data described by `count` and `MPI_Datatype`.
```c
int *data = malloc(...);
/* Send the first ten integers in data to rank 4 in COMM_WORLD */
MPI_Send(data, 10, MPI_INT, 4, 0, MPI_COMM_WORLD);
```

------

## Deadlocks

- `MPI_Send` is _blocking_
  - That means it waits until the buffer you pass in is safe to reuse

![Sketch of `MPI_Send`](/phys52015/images/manual/mpi-ssend-cartoon.svg)


--->

### What happens if the receive never arrives?

- Sender waits _forever_.

![MJ](https://i.kym-cdn.com/photos/images/original/001/762/214/ff4.jpg)

--->

### Why didn't that happen with my code?

- MPI has three blocking sends 
  1. `MPI_Send`
  2. `MPI_Ssend` (synchronous send)
  3. `MPI_Bsend` (buffered send)

- `MPI_Ssend` _always_ waits 
- `MPI_Bsend` we provide a buffer for MPI to copy into


--->

### `MPI_Bsend`

![Sketch of `MPI_Bsend`](/phys52015/images/manual/mpi-bsend-cartoon.svg)


--->

### `MPI_Send`

- MPI manages buffer
  - Behaves like `MPI_Bsend` if message is small enough 
  - Switches to `MPI_Ssend` if not 
- **No minimum buffer size is guaranteed**

[Example: ptp deadlock](/phys52015/notes/mpi/point-to-point/#deadlock-exercise)

--->

### Avoiding deadlocks

1. I just have pairwise exchanges

    - Can use `MPI_Sendrecv`, which merges a send and receive (the
    source and destinations don't have to match).
     

2. My communication pattern is more complicated

    - [Non-blocking messages](/phys52015/notes/mpi/point-to-point-nb/)


------

## Non-blocking messages

- Idea: separate _sending_ (receiving) message from waiting on completion
  
```c [4,8]
int MPI_Isend(const void *buffer, 
              int count, MPI_Datatype dtype, 
              int dest, int tag, MPI_Comm comm,
              MPI_Request *request);
int MPI_Irecv(void *buffer, 
              int count, MPI_Datatype dtype,
              int source, int tag, MPI_Comm comm,
              MPI_Request *request);
```

--->

### Checking for completion

- With a `request` object in hand, can either 
  1. _test_ if a message is completed (`MPI_Test`)
  2. _wait_ for a message to complete (`MPI_Wait`)
  
```c
int *data = ...;
int flag = 0;
MPI_Request req;
MPI_Isend(data, ..., &req);
...;
MPI_Test(req, &flag, MPI_STATUS_IGNORE);
if (flag) {
   /* Message has been sent */
} else {
   /* Message not yet sent */
}
```
--->

### Picture

![Sketch of `MPI_Isend`](/phys52015/images/manual/mpi-issend-cartoon.svg)

--->

### Advantages of this approach

- Can "post" sends (receives) for many messages at once
  - MPI library can "complete" them as the matching pair arrives
- This can lead to less waiting in messages


--->


### Example: blocking receives

- Only one receive ready at any one time.

```c

if (rank == 0) {
  for (int i = 1; i < size; i++) {
    MPI_Recv(..., i /* source */, ...);
  }
} else {
  MPI_Ssend(..., 0 /* dest */, ...);
}
```

--->

### Example: nonblocking receives

- All receives ready at once

```c
if (rank == 0) {
  MPI_Request *reqs = malloc(...);
  for (int i = 1; i < size; i++) {
    MPI_Irecv(..., i, ..., &reqs[i-1]);
  }
  /* Wait for completion of all receives */
  MPI_Waitall(reqs, size-1, MPI_STATUSES_IGNORE);
} else {
  MPI_Ssend(..., 0, ...);
}
```

------

## Communication performance, network latency

- In the [ping-pong exercise](/phys52015/exercises/mpi-ping-pong/)
  build a simple model for message time
  
$$
T(b) = \alpha + \beta b
$$

- Time to send a message depends on the latency $\alpha$ and the
  inverse bandwidth $\beta$.
  
--->

### Last year's results

![Ping pong timing](/phys52015/images/auto/ping-pong-timing-lots.svg)

--->

### Reducing message time

- With blocking messages, minimum simulation time is

  $$T_\text{blocking} = T_\text{compute} + T_\text{message}$$
  
- Non-blocking messages, minimum time is

  $$T_\text{non-block} = \min(T_\text{compute}, T_\text{message})$$

------

## Waiting for ~Godot~ many messages

- Need one request object per non-blocking message
- Three options
  1. Wait for all requests `MPI_Waitall`
  2. Wait for one request `MPI_Waitany`
  3. Wait for at least one request `MPI_Waitsome`
  
--->

### Typical pattern
```c
MPI_Request *requests;

nsend = ...;
nrecv = ...;

requests = malloc((nsend+nrecv)*sizeof(*requests));

for (int i = 0; i < nrecv; i++) {
  MPI_Irecv(..., &requests[i]);
}

for (int i = 0; i < nsend; i++) {
  MPI_Isend(..., &requests[i + nrecv]);
}

/* Some work that doesn't depend on the messages */
```

--->

### Waiting options

- Everything
```c
MPI_Waitall(nsend+nrecv, requests, MPI_STATUSES_IGNORE);
```
- A message
```c
int which;
MPI_Waitany(nsend+nrecv, requests, &which, MPI_STATUSES_IGNORE);
```
- Some messages
```c
int *indices = malloc((nsend+nrecv)*sizeof(*indices));
int nfinished;
MPI_Waitsome(nsend+nrecv, requests, &nfinished, indices,
             MPI_STATUSES_IGNORE);
```

------

## Major point

- Message is not complete until succesfully waited on or tested
- **Not allowed** to look at receive buffer (or modify send buffer)
  until the request has completed
  
```c
MPI_Request *requests;
nsend = ...;
nrecv = ...;
requests = malloc((nsend+nrecv)*sizeof(*requests));
for (int i = 0; i < nrecv; i++) {
  MPI_Irecv(data, ..., &requests[i]);
}
/* Not allowed */
if (data[0]) {
  ...;
}
MPI_Waitall(..., requests);
```

------

## Exercises

- [MPI ping pong](/phys52015/exercises/mpi-ping-pong/)
- [Gather to zero](/phys52015/notes/mpi/point-to-point-nb/)
- [Halo exchanges](/phys52015/exercises/mpi-stencil/) This one is harder
