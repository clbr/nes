#include "common.h"

#define SIZE 16384

static void readdata(const char name[], u8 *arr) {
	FILE *f = fopen(name, "r");
	if (!f) die("Can't open %s\n", name);

	fseek(f, 0, SEEK_END);
	const u32 len = ftell(f);
	if (len > SIZE) die("%s too large (%lu)\n", name, len);
	rewind(f);

	if (fread(arr, len, 1, f) != 1) die("Read error\n");

	fclose(f);
}

int main(int argc, char **argv) {

	u8 map[SIZE] = { 0 }, chr[SIZE] = { 0 };

	if (argc != 4) die("Usage: %s map chr combined\n", argv[0]);

	readdata(argv[1], map);
	readdata(argv[2], chr);

	FILE *f = fopen(argv[3], "w");
	if (!f) die("Can't open %s\n", argv[3]);

	u32 i;
	for (i = 0; i < SIZE; i++) {
		fwrite(&map[i], 1, 1, f);
		fwrite(&chr[i], 1, 1, f);
	}

	fclose(f);

	return 0;
}
