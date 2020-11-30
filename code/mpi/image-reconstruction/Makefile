CC = mpicc

CFLAGS = -Wall -Wextra -std=c11 -D_GNU_SOURCE -g -O3
LDFLAGS = -lm

OBJ = image.o pgmio.o main.o

.PHONY: clean all

all: main

main: $(OBJ) Makefile
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

main.o: main.c image.h pgmio.h Makefile
pgmio.o: pgmio.c image.h pgmio.h Makefile
image.o: image.c image.h Makefile

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<


clean:
	-rm -rf $(OBJ) main main.dSYM
