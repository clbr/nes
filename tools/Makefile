CFLAGS += -Wall -Wextra
LDFLAGS += -Wl,-as-needed -Wl,-O1

.phony: all clean
all: png2chr chr2png sametiles tilecoords nes2hex pngreorder png2gba palstat coord2metasprite nesasmc nsf2data text2data

png2chr: png2chr.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

png2gba: png2gba.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

chr2png: chr2png.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

sametiles: sametiles.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

tilecoords: tilecoords.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

nes2hex: nes2hex.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

pngreorder: pngreorder.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

palstat: palstat.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

coord2metasprite: coord2metasprite.o common.o

nesasmc: nesasmc.o
	$(CC) -o $@ $^ $(CFLAGS)

nsf2data: nsf2data.o
	$(CC) -o $@ $^ $(CFLAGS)

text2data: text2data.o
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o png2chr chr2png sametiles tilecoords nes2hex pngreorder png2gba palstat \
		coord2metasprite nesasmc nsf2data text2data
