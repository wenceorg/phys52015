CC = mpicc
OPENBLAS = /usr/local/opt/openblas
CFLAGS = -Wall -Wextra -I$(OPENBLAS)/include -I. -g -O2
LDFLAGS = -L$(OPENBLAS)/lib -Wl,-rpath,$(OPENBLAS)/lib -lopenblas -lm

SOLUTION = solution
HDR = vec.h mat.h utils.h
OBJ = vec.o mat.o $(SOLUTION).o
EXE = main

.PHONY: all clean

all: $(EXE)

clean:
	-rm -f $(OBJ) $(EXE)

$(EXE): $(EXE).c $(OBJ) $(HDR) Makefile
	$(CC) $(CFLAGS) -o $@ $< $(OBJ) $(LDFLAGS)

vec.o: vec.c vec.h utils.h Makefile
	$(CC) $(CFLAGS) -c -o $@ $<

mat.o: mat.c mat.h vec.h utils.h Makefile
	$(CC) $(CFLAGS) -c -o $@ $<

$(SOLUTION).o: $(SOLUTION).c mat.h vec.h utils.h Makefile
	$(CC) $(CFLAGS) -c -o $@ $<
