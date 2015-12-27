#include <stdio.h>
#include <zlib.h>
#include "srcdata.h"

int main() {

	unsigned sum = adler32(0, NULL, 0);

	printf("Sizeof %lu, adler %lx\n", sizeof(srcdata), adler32(sum, srcdata, 768));

	return 0;
}
