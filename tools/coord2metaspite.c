#include "common.h"

int main(int argc, char **argv) {

	const char *attr = "0";
	const char *noskip = getenv("noskip");
	char buf[4096];
	u32 w = 0, x = 0, y = 0;

	if (argc == 2)
		attr = argv[1];

	printf("static const u8 _metasprite[] = {\n");

	// Read coords from stdin and transform into metasprites.
	while (fgets(buf, 4096, stdin)) {
		if (!w) {
			for (x = 0; buf[x]; x++) {
				if (buf[x] == 'x')
					w++;
			}
		}

		const char *ptr = buf;
		for (x = 0; x < w; x++) {
			const u8 tile = strtol(ptr, NULL, 16);
			ptr += 5;
			if (!tile && !noskip) continue;
			printf("%u, %u, 0x%02x, %s,\n", x * 8, y * 8, tile, attr);
		}

		y++;
	}

	printf("};\n");

	return 0;
}
