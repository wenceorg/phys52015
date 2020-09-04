CC = gcc-10

CFLAGS = -I. -std=c11 -O3 -Wall -Wextra
LIBS = -lm

DEPS = proto.h
OBJ  = main.o  add_numbers.o io.o
EXE = addnumbers

.PHONY: all clean

all: $(EXE)

$(EXE): $(OBJ) $(DEPS)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)

clean:
	-rm -f $(OBJ) $(EXE)
