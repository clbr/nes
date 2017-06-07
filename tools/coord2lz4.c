#include "common.h"
#include "lz4hc.h"

int main() {

	char buf[4096];
	u8 *uncomp = NULL, *comp = NULL;
	u32 w = 0, x = 0, y = 0;

	// Read coords from stdin and transform into metasprites.
	while (fgets(buf, 4096, stdin)) {
		if (!w) {
			for (x = 0; buf[x]; x++) {
				if (buf[x] == 'x')
					w++;
			}

			uncomp = calloc(1, w);
		}

		const char *ptr = buf;
		for (x = 0; x < w; x++) {
			const u8 tile = strtol(ptr, NULL, 16);
			ptr += 5;
			uncomp[y * w + x] = tile;
		}

		y++;
		uncomp = realloc(uncomp, (y + 1) * w);
	}

	const u32 uncompsize = y * w;
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

	printf("};\n");
	return 0;
}
