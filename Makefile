all: ctrltest.nes

ctrltest.nes:
	cl65 -t nes -Oisr -o $@ ctrltest.c
