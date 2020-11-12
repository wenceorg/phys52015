---
title: "Local setup"
---

# Running the exercises on your own hardware

Although you don't need to, you might find it useful or convenient to
run the exercises on your own computer. To do so, you'll need to
obtain appropriate compilers and MPI implementations.

For more general information on getting a development toolchain setup,
the Faculty of Natural Sciences at [Imperial
College](https://www.imperial.ac.uk/) have prepared some [useful guides](https://imperial-fons-computing.github.io)

## MacOS

### Open source tools

I recommend [homebrew](https://brew.sh) for installation of packages
on MacOS. You will need an MPI implementation, I recommend
[mpich](https://www.mpich.org), which can be obtained with

```sh
$ brew install mpich
```

Apple's C compiler does not come with
[OpenMP](https://www.openmp.org), so you'll need to use
[gcc](https://gcc.gnu.org). You can get it with

```sh
$ brew install gcc
```

Since MacOS ships with its own compiler toolchain, homebrew doesn't
install these compilers with clashing names, so to use this version of
GCC you'll need to run `gcc-10` (rather than just `gcc`).

### Intel toolchain

The Intel compiler and profiling toolchain is available for [free for
student
use](https://software.intel.com/content/www/us/en/develop/articles/qualify-for-free-software.html#student).
You'll want the "Parallel Studio XE". Note that the license under
which you can use these tools is slightly constrained: read it
carefully.

## Linux-based systems

### Open source tools

Most linux distributions come with MPI libraries and compilers. You
may need to install appropriate packages using the package manager. On
Debian-based systems you'll need

```
build-essential
libmpich-dev
mpich
gcc
```


### Intel toolchain

As on MacOS, you can install a free version of the [Intel compiler
toolchain](https://software.intel.com/content/www/us/en/develop/articles/qualify-for-free-software.html#student)
as a student.

## Windows

If you have Windows 10, you might be able to to use
[WSL](https://docs.microsoft.com/en-us/windows/wsl/install-win10) and
then follow the instructions for Linux-based systems. See also [this
tutorial](https://www.laptopmag.com/articles/use-bash-shell-windows-10)
on enabling the bash shell in Windows 10. The Intel toolchain is
[available on
Windows](https://software.intel.com/content/www/us/en/develop/articles/qualify-for-free-software.html#student),
but I have no experience of it.
