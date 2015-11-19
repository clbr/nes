#include "common.h"
#include "col.h"

int main(int argc, char **argv) {

	if (argc < 2)
		die("Usage: %s color [color...]\n",
			argv[0]);

	u32 i;
	for (i = 1; i < (u32) argc; i++) {
		const u32 in = strtol(argv[i], NULL, 16);
		if (in > 64)
			die("Error: %u out of range\n", in);
		const u32 out = palette[in];

		printf("0x%02x -> 0x%06x\n", in, out);
	}

	return 0;
}
