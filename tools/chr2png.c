#include <png.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "common.h"

static void savepng(FILE *f, const u8 * const data, const u32 tiles) {

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) die("PNG error\n");
	png_infop info = png_create_info_struct(png_ptr);
	if (!info) die("PNG error\n");
	if (setjmp(png_jmpbuf(png_ptr))) die("PNG error\n");

	// Calculate a suitable resolution.
	u32 w, h;
	if (tiles < 16) {
		h = 8;
		w = tiles * 8;
	} else if (tiles % 16) {
		h = 8 * tiles / 16 + 1;
		w = 8 * 16;
	} else {
		h = 8 * tiles / 16;
		w = 8 * 16;
	}

	printf("Saving to a %ux%u PNG.\n", w, h);

	png_init_io(png_ptr, f);
	png_set_IHDR(png_ptr, info, w, h, 2, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_color palette[4] = {
		{ 0, 0, 0 },
		{ 255, 0, 0 },
		{ 0, 255, 0 },
		{ 0, 0, 255 },
	};
	png_set_PLTE(png_ptr, info, palette, 4);
	png_write_info(png_ptr, info);
	png_set_packing(png_ptr);

	// Convert to a single buffer
	u8 * const pdata = calloc(w * h, 1);
	u32 i;
	const u8 *ptr = data;
	for (i = 0; i < tiles; i++) {
		const u32 row = (i / 16) * 8;
		const u32 col = (i % 16) * 8;
		u8 rows[8][8];
		u32 j;
		for (j = 0; j < 8; j++)
			memset(&rows[j][0], 0, 8);

		for (j = 0; j < 8; j++) {
			const u8 pix = *ptr;
			ptr++;

			if (pix & 0x80)
				rows[j][0] |= 1;
			if (pix & 0x40)
				rows[j][1] |= 1;
			if (pix & 0x20)
				rows[j][2] |= 1;
			if (pix & 0x10)
				rows[j][3] |= 1;
			if (pix & 0x8)
				rows[j][4] |= 1;
			if (pix & 0x4)
				rows[j][5] |= 1;
			if (pix & 0x2)
				rows[j][6] |= 1;
			if (pix & 0x1)
				rows[j][7] |= 1;
		}

		for (j = 0; j < 8; j++) {
			const u8 pix = *ptr;
			ptr++;

			if (pix & 0x80)
				rows[j][0] |= 2;
			if (pix & 0x40)
				rows[j][1] |= 2;
			if (pix & 0x20)
				rows[j][2] |= 2;
			if (pix & 0x10)
				rows[j][3] |= 2;
			if (pix & 0x8)
				rows[j][4] |= 2;
			if (pix & 0x4)
				rows[j][5] |= 2;
			if (pix & 0x2)
				rows[j][6] |= 2;
			if (pix & 0x1)
				rows[j][7] |= 2;
		}

		// Copy over
		for (j = 0; j < 8; j++)
			memcpy(pdata + (row + j) * w + col, &rows[j][0], 8);
	}

	// Write
	for (i = 0; i < h; i++) {
		png_write_row(png_ptr, pdata + i * w);
	}
	png_write_end(png_ptr, NULL);
	free(pdata);

	png_destroy_info_struct(png_ptr, &info);
	png_destroy_write_struct(&png_ptr, NULL);
}

int main(int argc, char **argv) {

	if (argc != 2) {
		printf("Usage: %s file.png\n", argv[0]);
		return 1;
	}

	FILE *f = fopen(argv[1], "r");
	if (!f)
		die("Can't open file\n");

	const u32 namelen = strlen(argv[1]);

	char outname[namelen + 1];
	strcpy(outname, argv[1]);
	outname[namelen - 1] = 'g';
	outname[namelen - 2] = 'n';
	outname[namelen - 3] = 'p';

	FILE *out = fopen(outname, "w");
	if (!out)
		die("Can't open output file '%s'\n", outname);

	struct stat st;
	fstat(fileno(f), &st);

	if (st.st_size % 16 || !st.st_size)
		die("Unaligned CHR file?\n");

	u8 * const data = calloc(st.st_size, 1);
	fread(data, st.st_size, 1, f);
	fclose(f);

	savepng(out, data, st.st_size / 16);

	free(data);
	fclose(out);
	return 0;
}
