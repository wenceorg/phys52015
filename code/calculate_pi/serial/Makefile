CC = icc

CFLAGS = -I. -std=c11 -O2
LIBS = -lm

DEPS = proto.h
OBJ  = main.o  calculate_pi.o
EXE = calcpi

.PHONY: all clean


all: $(EXE)

$(EXE): $(OBJ) $(DEPS)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)

clean:
	-rm -f $(OBJ) $(EXE)
