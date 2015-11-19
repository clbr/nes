CFLAGS += -Wall -Wextra
LDFLAGS += -Wl,-as-needed -Wl,-O1

.phony: all clean
all: png2chr chr2png sametiles tilecoords nes2hex

png2chr: png2chr.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

chr2png: chr2png.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

sametiles: sametiles.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

tilecoords: tilecoords.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

nes2hex: nes2hex.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

clean:
	rm -f *.o png2chr chr2png sametiles tilecoords
