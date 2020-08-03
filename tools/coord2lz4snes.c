#include "common.h"
#include "lz4hc.h"
#include <string.h>
#include <ctype.h>

#define FLIPH (1 << 14)
#define FLIPV (1 << 15)

int main() {

	char buf[4096];
	u16 *uncomp = NULL;
	u8 *comp = NULL;
	u32 w = 0, x = 0, y = 0;
	u8 flips = 0;

	// Read coords from stdin
	while (fgets(buf, 4096, stdin)) {
		if (!w) {
			for (x = 0; buf[x]; x++) {
				if (buf[x] == 'x')
					w++;
				if (buf[x] == ',')
					flips = 1;
			}

			uncomp = calloc(2, w);
		}

		const char *ptr = buf;
		for (x = 0; x < w; x++) {
			u16 tile = strtol(ptr, NULL, 16);
			if (!flips) {
				ptr += 5;
			} else {
				const char * const start = ptr + 5;

				while (!isspace(*ptr))
					ptr++;
				while (isspace(*ptr))
					ptr++;

				if (memchr(start, 'H', ptr - start))
					tile |= FLIPH;
				if (memchr(start, 'V', ptr - start))
					tile |= FLIPV;
			}
			uncomp[y * w + x] = tile;
		}

		y++;
		uncomp = realloc(uncomp, (y + 1) * w * 2);
	}

	const u32 uncompsize = y * w * 2;
	comp = calloc(uncompsize, 1);

	const u32 compsize = LZ4_compress_HC((char *) uncomp, (char *) comp,
				uncompsize, uncompsize, 16);

	if (!compsize) {
		printf("Failed to compress it\n");
		return 1;
	}

	printf("// Compressed from %u to %u bytes, %.2f%%\n\n",
		uncompsize, compsize, compsize * 100.0f / uncompsize);

	printf("static const u8 _lz4[%u] = {\n", compsize);

	for (x = 0; x < compsize; x++) {
		if (x % 16 == 0)
			printf("\t");

		printf("0x%02x,", comp[x]);

		if (x % 16 == 15 || x == compsize - 1)
			printf("\n");
		else
			printf(" ");
	}

	free(comp);
	free(uncomp);

	printf("};\n");
	return 0;
}
