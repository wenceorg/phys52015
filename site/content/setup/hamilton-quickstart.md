---
title: "Hamilton access & quickstart"
---

# Accessing Durham Supercomputing facilities

Most of the exercises in this course will require that you use one of
Durham's supercomputing systems. This will either be Hamilton, or (for
some Physics students) COSMA. You don't automatically get an account
on Hamilton, so you'll need to register for one.

## Access to Hamilton

For many of the exercises in the course, we will be using the
[Hamilton](https://www.dur.ac.uk/cis/local/hpc/) supercomputer. You
should obtain an account on Hamilton by [following their
instructions](https://www.dur.ac.uk/cis/local/hpc/hamilton/account/#getting_account).

When requesting an account for this course, please put the course
leader (Lawrence Mitchell) as the approver, and mention that the
access is for the MISCADA course PHYS52015.

Since we'll be logging in a lot, I also provide some [tips]({{< ref
configuration.md >}}) on how to configure `ssh` for swifter login.

## Access to COSMA

Some of you may have access to the Physics-run
[COSMA](https://www.dur.ac.uk/icc/cosma/) system. You can also use
this system, the same information about setting up ssh login also
applies.

The rest of the guide is a quick start on using supercomputing systems
where you must compile code and then submit it via a batch scheduler.
The focus is on Hamilton, but COSMA uses the same scheduling system,
so most things will work with only minor changes.

# Supercomputing Durham: Hamilton Quick Start Guide

This is adapted from a quickstart guide from [Tobias
Weinzierl](http://www.peano-framework.org/index.php/tobias-weinzierl/).

It is intended to get you up and running on the Hamilton supercomputer
quickly. It is not replacement of any of the [official
documentation](https://www.dur.ac.uk/cis/local/hpc/hamilton/).

## Logging in and transferring code

You access Hamilton via ssh with

```sh
$ ssh USERNAME@hamilton.dur.ac.uk
```

{{% hint info %}}
When writing commands to execute in the shell, I use a `$` to indicate
the prompt. You should not type this character.
{{% /hint %}}

Where `USERNAME` is your CIS username. Since we'll do this a lot, you
can also see some [tips]({{< ref configuration.md >}}) on how to
configure `ssh` for swifter logins.

Hamilton doesn't mount any of the Durham shared drives, so you have to
manually transfer any files you want. You can do this with 
[`scp`](https://linux.die.net/man/1/scp). For example, if you are on
your local machine then

```sh
$ scp somefile.c USERNAME@hamilton.dur.ac.uk:
```

copies `somefile.c` into your home directory on Hamilton. The other
option is to directly download files when you are logged in. Some of
the exercises in the course will provide more details on how to do
this.

## Compilation environment

As is common with supercomputers, there are many different compiler
versions available on Hamilton. These are managed with [environment
modules](https://modules.readthedocs.io/en/latest/) so that different
Hamilton users can control which compilers and tools they get.

Often in this course we'll use the Intel compiler for which we need to
load two modules

```sh
$ module load intel/xe_2018.2
$ module load gcc/9.3.0
```

This makes the Intel compiler tools available and loads a recent
version of gcc. After executing these commands you can check the
versions you have
```sh
$ icc --version
icc (ICC) 18.0.2 20180210
Copyright (C) 1985-2018 Intel Corporation.  All rights reserved.

$ gcc --version
gcc (GCC) 9.3.0
Copyright (C) 2019 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

The exercises will typically enumerate the modules you need.

## Running code

When we log in to the Hamilton system, we access the "login" node. You
can use this to compile code and run some profile analysis programs,
however **DO NOT** use it for running your simulations. In common with
most supercomputers, Hamilton consists of two parts.

1. Login nodes (this is where we've been so far);
2. Compute nodes (this is where you want to run your code).

{{< manfig src="hamilton-nodes.svg"
    width="70%"
    caption="Schematic of Hamilton login and compute nodes" >}}

To run code on the compute nodes we need to submit a job to the
scheduler. This program takes care of allocate simulations to the
compute nodes to maximise throughput for all users of the system. On
Hamilton, the scheduler is called
[Slurm](https://slurm.schedmd.com/documentation.html).

To use the scheduler we have to create a script that provides a recipe
for Hamilton to run our code. This script is a shell script that
contains some magic comments that describe things to the scheduler.
The individual exercises contain some examples, as does the [Hamilton
documentation](https://www.dur.ac.uk/cis/local/hpc/hamilton/slurm-v2/templates/).
Here is a simple example for a serial job.

```bash
#!/bin/bash
#SBATCH --job-name="myjob"
#SBATCH -o myjob.%A.out
#SBATCH -e myjob.%A.err
#SBATCH -p test.q
#SBATCH -t 00:05:00
#SBATCH --nodes=1
#SBATCH --cpus-per-task=24
#SBATCH --mail-type=ALL
#SBATCH --mail-user=YOUREMAIL@durham.ac.uk

source /etc/profile.d/modules.sh

module load intel/xe_2018.2
module load gcc/9.3.0

./myexecutable
```

Some things to note. This is a shell script executed with bash (as
indicated by the shebang-line). Lines beginning with `#SBATCH` are
parsed by the job submission command `sbatch` and are used to provide
options to it. Here we selected a particular queue `test.q` and said
the job will run for a maximum of five minutes (`-t 00:05:00`). The
other options control the size of the job and where output is sent.
Run `man sbatch` on the Hamilton login node to see details of these
flags.

{{< hint "info" >}}

A typical reason your job might fail is because you did not load the
necessary modules, so don't forget to do so!

This is also useful for reproducibility, since it helps you to record
exactly the software you used to produce the results.

{{< /hint >}}

After all the comments we have the commands that will be run _on the
compute node_. The first makes the `module` command available. We then
load the same modules we used during compilation. Potentially we might
also load other modules here to gain access to profiling commands.
Finally is the command to run our code (here named `myexecutable`).

Having saved this submission script (say as `myjob.slurm`) we submit
it `sbatch`

```sh
$ sbatch myjob.slurm
```

This job is now submitted to the queue and will run when a slot is
available.

You can see what jobs you currently have in the queue with

```sh
$ squeue -u $USER
```

For more details on using Hamilton, you're encouraged to check [their
documentation](https://www.dur.ac.uk/cis/local/hpc/hamilton/). We'll
also recapitulate aspects of this guide when we carry out the exercises.
