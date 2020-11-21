---
title: "MPI: ping-pong latency"
weight: 9
katex: true
---

# Measuring point-to-point message latency with ping-pong

In this exercise we will write a simple code that does a message
ping-pong: sending a message back and forth between two processes.

We can use this to measure both the _latency_ and _bandwidth_ of the
network on our supercomputer. Which are both important measurements
when we're looking at potential parallel performance: they help us to
decide if our code is running slowly because of our bad choices, or
limitations in the hardware.

## A model for the time to send a message

We care about the total time it takes to send a message, our model is
a linear model which has two free parameters:

1. $\alpha$, the message latency, measured in seconds;
2. $\beta$, the network bandwidth, measured in bytes/second.

With this model, the time to send a message with $b$ bytes is

$$
T(b) = \alpha + \beta b
$$

