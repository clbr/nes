#include <limits.h>
#include <string.h>
#include "common.h"
#include "lz4hc.h"

#define LEN 60000U

int main(int argc, char* argv[])
{
	FILE *f;
	u8 inbuf[LEN], outbuf[LEN];
	size_t inlen;
	size_t outlen;

	if (argc != 3 && argc != 2)
		die("Usage: %s input [output]\n", argv[0]);

	f = fopen(argv[1], "rb");
	if (!f)
		die("Can't open input file %s\n", argv[1]);
	inlen = fread(inbuf, 1, LEN, f);
	fclose(f);

	outlen = LZ4_compress_HC((char *) inbuf, (char *) outbuf,
				inlen, LEN, 16);

	const char *outname = argv[2];
	char namebuf[PATH_MAX] = "";
	if (argc != 3) {
		strcpy(namebuf, argv[1]);
		strcat(namebuf, ".lz4");
		outname = namebuf;
	}

	f = fopen(outname, "wb");
	if (!f)
		die("Can't open output file %s\n", outname);

	if (fwrite(outbuf, 1, outlen, f) != outlen)
		die("Error writing\n");
	fclose(f);

	printf("Compressed %s to %s (%zu to %zu bytes), %.2f%%\n",
		argv[1], outname, inlen, outlen,
		outlen * 100.0f / inlen);
	return 0;
}
