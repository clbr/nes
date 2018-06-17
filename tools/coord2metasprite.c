#include "common.h"
#include <ctype.h>
#include <string.h>

int main(int argc, char **argv) {

	const char *attr = "0";
	const char *noskip = getenv("noskip");
	char buf[4096];
	u32 w = 0, x = 0, y = 0;
	u8 flips = 0;

	if (argc == 2)
		attr = argv[1];

	printf("static const u8 _metasprite[] = {\n");

	// Read coords from stdin and transform into metasprites.
	while (fgets(buf, 4096, stdin)) {
		if (!w) {
			for (x = 0; buf[x]; x++) {
				if (buf[x] == 'x')
					w++;
				if (buf[x] == ',')
					flips = 1;
			}
		}

		const char *ptr = buf;
		for (x = 0; x < w; x++) {
			const u8 tile = strtol(ptr, NULL, 16);
			u8 flipv = 0, fliph = 0;
			if (!flips) {
				ptr += 5;
			} else {
				const char * const start = ptr + 5;

				while (!isspace(*ptr))
					ptr++;
				while (isspace(*ptr))
					ptr++;

				if (memchr(start, 'H', ptr - start))
					fliph = 1;
				if (memchr(start, 'V', ptr - start))
					flipv = 1;
			}
			if (!tile && !noskip) continue;
			printf("%u, %u, 0x%02x, %s%s%s,\n", x * 8, y * 8, tile, attr,
				fliph ? "|OAM_FLIP_H" : "",
				flipv ? "|OAM_FLIP_V" : "");
		}

		y++;
	}

	printf("128\n};\n");

	return 0;
}
