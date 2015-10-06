CFLAGS += -Wall -Wextra
LDFLAGS += -Wl,-as-needed -Wl,-O1

.phony: all clean
all: png2chr chr2png

png2chr: png2chr.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

chr2png: chr2png.o common.o
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) -lpng

clean:
	rm -f *.o png2chr chr2png
