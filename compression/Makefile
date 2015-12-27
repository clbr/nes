.PHONY: all clean

all: compression.nes

compression.nes: test.c
	cl65 -t nes -Oisr -o $@ test.c

clean:
	rm -f *.nes *.o
