CC = mpicc
# You may need to edit this location.

# This is for MacOS + Homebrew
OPENBLAS = /usr/local/opt/openblas
# This is for Hamilton
OPENBLAS = /ddn/apps/Cluster-Apps/openblas/gcc-8.2.0/0.2.20
# COSMA shouldn't need anything

# You should not need to change anything else.
CFLAGS := $(CPPFLAGS) -std=c99 -Wall -Wextra -I$(OPENBLAS)/include -I. -g -O2 -D_GNU_SOURCE
LDFLAGS := $(LDFLAGS) -L$(OPENBLAS)/lib -Wl,-rpath,$(OPENBLAS)/lib -lopenblas -lm

SOLUTION ?= solution
HDR = vec.h mat.h utils.h check.h bench.h
OBJ = vec.o mat.o check.o bench.o $(SOLUTION).o
EXE = main

.PHONY: all clean

all: $(EXE)

clean:
	-rm -rf $(OBJ) $(EXE) $(EXE).dSYM

$(EXE): $(EXE).c $(OBJ)
	$(CC) $(CFLAGS) -o $@ $< $(OBJ) $(LDFLAGS)

vec.o: vec.c vec.h utils.h Makefile
mat.o: mat.c mat.h vec.h utils.h Makefile
check.o: check.c check.h utils.h mat.h vec.h Makefile
bench.o: bench.c bench.h utils.h mat.h vec.h Makefile
$(SOLUTION).o: $(SOLUTION).c mat.h vec.h utils.h Makefile

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<



