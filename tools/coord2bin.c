#include "common.h"
#include <unistd.h>

int main() {

	char buf[4096];
	u32 w = 0, x = 0;

	if (isatty(fileno(stdout)))
		die("Won't output binary to terminal\n");

	// Read coords from stdin and write binary to stdout
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
			fwrite(&tile, 1, 1, stdout);
		}
	}

	return 0;
}
