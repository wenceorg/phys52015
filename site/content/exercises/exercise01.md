---
title: "Parallel Hello World"
weight: 1
---

# Hello, World!

As with every programming course, the first thing we will do is
compile and run a "Hello world" program. Actually we'll do three. The
goal of this is to familiarise you with the module system on Hamilton,
so take a look at the [quickstart guide]({{< ref
"hamilton-quickstart.md" >}}) if you haven't already.

## A serial version

Log in to the Hamilton compute node and load the relevant compiler
modules

```
intel/xe_2018.2
gcc/9.3.0
```

Create a C file `serial.c` containing the below.

{{< code-include "hello/serial.c" "c" >}}

{{< hint "info" >}}

These code snippets have a link to the raw code, you can copy that
link and use `wget` to download the code on Hamilton.

{{< /hint >}}

Having done that you should compile the code with `icc`

```sh
$ icc -o hello-serial serial.c
```

The creates an executable named `hello-serial` from the `serial.c`
file.

Run it on the login node

```sh
$ ./hello-serial
Hello, World!
```

Next, as you saw in the quickstart guide, we should actually do our
runs on the compute nodes. To do this, we need to create a submission
script

{{< code-include "hello/serial.slurm" "sh" >}}
