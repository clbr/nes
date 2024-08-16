CFLAGS += -Wall -Wextra
CXXFLAGS += -Wall -Wextra
CXXFLAGS += -fno-exceptions -fno-rtti
LDFLAGS += -Wl,-as-needed -Wl,-O1

BINS = png2chr chr2png sametiles tilecoords nes2hex pngreorder png2gba \
	palstat coord2metasprite nesasmc nsf2data text2data \
	coord2lz4 coord2bin mode7interleave cc65lz4 png2tilebit \
	tilecoords16 coord2lz4snes png2n64 coord2png coord2tilepng \
	coordreuse2png

.phony: all clean
all: $(BINS)

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

tilecoords16: tilecoords16.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

nes2hex: nes2hex.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

pngreorder: pngreorder.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

palstat: palstat.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

coord2metasprite: coord2metasprite.o common.o

coord2bin: coord2bin.o common.o

mode7interleave: mode7interleave.o common.o

nesasmc: nesasmc.o
	$(CXX) -o $@ $^ $(CXXFLAGS)

nsf2data: nsf2data.o
	$(CXX) -o $@ $^ $(CXXFLAGS)

text2data: text2data.o
	$(CXX) -o $@ $^ $(CXXFLAGS)

coord2lz4: coord2lz4.o common.o lz4hc.o lz4.o

coord2lz4snes: coord2lz4snes.o common.o lz4hc.o lz4.o

cc65lz4: cc65lz4.o common.o lz4hc.o lz4.o

png2tilebit: png2tilebit.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

png2n64: png2n64.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

coord2png: coord2png.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

coord2tilepng: coord2tilepng.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

coordreuse2png: coordreuse2png.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

clean:
	rm -f *.o $(BINS)
