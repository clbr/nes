.PHONY: all clean

all: ctrltest.nes

ctrltest.nes:
	cl65 -t nes -Oisr -o $@ ctrltest.c

clean:
	rm -f *.nes *.o
