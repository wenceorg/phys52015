CC = icc

CFLAGS = -I. -O2 -std=c11
LIBS = -lm

DEPS = proto.h
OBJ  = main.o io.o filters.o
EXE = blur

.PHONY: all clean

all: $(EXE) Makefile

$(EXE): $(OBJ) $(DEPS)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)

clean:
	-rm -f $(OBJ) $(EXE)
