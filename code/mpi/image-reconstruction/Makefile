CC = mpicc

CFLAGS = -Wall -Wextra -std=c11 -D_GNU_SOURCE -g -O3
LDFLAGS = -lm

EXE = main

OBJ = image.o pgmio.o $(EXE).o


.PHONY: clean all


all: $(EXE)

$(EXE): $(OBJ) Makefile
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

$(EXE).o: $(EXE).c image.h pgmio.h Makefile
pgmio.o: pgmio.c image.h pgmio.h Makefile
image.o: image.c image.h Makefile

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<


clean:
	-rm -rf $(OBJ) $(EXE) $(EXE).dSYM
