#define _GNU_SOURCE

#include <png.h>
#include <unistd.h>
#include <getopt.h>
#include "common.h"

static png_color srcpal[16];
static u8 numsrc;

static void readsrc(const char in[]) {

	FILE *f = fopen(in, "r");
	if (!f) die("Can't open %s\n", in);

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	if (!png_ptr) die("PNG error\n");
	png_infop info = png_create_info_struct(png_ptr);
	if (!info) die("PNG error\n");
	if (setjmp(png_jmpbuf(png_ptr))) die("PNG error\n");

	png_init_io(png_ptr, f);
	png_read_png(png_ptr, info,
		PNG_TRANSFORM_PACKING|PNG_TRANSFORM_STRIP_16|PNG_TRANSFORM_STRIP_ALPHA, NULL);

	const u8 type = png_get_color_type(png_ptr, info);
	if (type != PNG_COLOR_TYPE_PALETTE)
		die("Input must be a paletted PNG, got %u\n", type);

	const u8 depth = png_get_bit_depth(png_ptr, info);
	if (depth != 8)
		die("Depth not 8 (%u) - maybe you have old libpng?\n", depth);

	png_color *ptr;
	int num;
	png_get_PLTE(png_ptr, info, &ptr, &num);

	printf("Source: %u colors\n", num);

	memset(srcpal, 0, sizeof(png_color) * 16);

	u8 i;
	const u8 max = num < 16 ? num : 16;
	for (i = 0; i < max; i++) {
		memcpy(&srcpal[i], &ptr[i], sizeof(png_color));
	}

	numsrc = max;

	fclose(f);
	png_destroy_info_struct(png_ptr, &info);
	png_destroy_read_struct(&png_ptr, NULL, NULL);
}

static u8 equal(const png_color a, const png_color b) {
	return a.red == b.red && a.green == b.green &&
		a.blue == b.blue;
}

static void remap(const char in[], const u8 offset) {

	FILE *f = fopen(in, "r");
	if (!f) die("Can't open %s\n", in);

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	if (!png_ptr) die("PNG error\n");
	png_infop info = png_create_info_struct(png_ptr);
	if (!info) die("PNG error\n");
	if (setjmp(png_jmpbuf(png_ptr))) die("PNG error\n");

	png_init_io(png_ptr, f);
	png_read_png(png_ptr, info,
		PNG_TRANSFORM_PACKING|PNG_TRANSFORM_STRIP_16|PNG_TRANSFORM_STRIP_ALPHA, NULL);

	const u8 type = png_get_color_type(png_ptr, info);
	if (type != PNG_COLOR_TYPE_PALETTE)
		die("Input must be a paletted PNG, got %u\n", type);

	const u8 depth = png_get_bit_depth(png_ptr, info);
	if (depth != 8)
		die("Depth not 8 (%u) - maybe you have old libpng?\n", depth);

	int num;
	png_color *ptr;
	png_get_PLTE(png_ptr, info, &ptr, &num);

	if (num > 16 && !offset)
		die("Image %s has %u colors, not able to remap\n", in, num);

	const u32 max = num < 16 ? num : 16;

	png_color inpal[16];
	memset(inpal, 0, sizeof(png_color) * 16);
	memcpy(inpal, ptr, sizeof(png_color) * max);

	u8 **rows = png_get_rows(png_ptr, info);
	const u32 imgw = png_get_image_width(png_ptr, info);
	const u32 imgh = png_get_image_height(png_ptr, info);

	const u32 rowbytes = png_get_rowbytes(png_ptr, info);
	if (rowbytes != imgw)
		die("Packing failed, row was %u instead of %u\n", rowbytes, imgw);

	u8 * const data = calloc(imgw, imgh);
	u32 i, j;
	for (i = 0; i < imgh; i++) {
		u8 * const target = data + imgw * i;
		memcpy(target, &rows[i][0], imgw);
	}

	// Remap tables
	u8 table[16];
	memset(table, 0, 16);
	if (offset) {
		table[0] = 0;

		for (i = 1; i < max; i++)
			table[i] = i + offset;
	} else {
		u8 found;
		for (i = 0; i < max; i++) {
			found = 0;
			for (j = 0; j < numsrc; j++) {
				if (equal(srcpal[j], inpal[i])) {
					found = 1;
					break;
				}
			}

			if (found)
				table[i] = j;
			else
				table[i] = 0;
		}
	}

	fclose(f);
	png_destroy_info_struct(png_ptr, &info);
	png_destroy_read_struct(&png_ptr, NULL, NULL);

	f = fopen(in, "w");
	if (!f) die("Can't write %s\n", in);

	// Action
	for (j = 0; j < imgh; j++) {
		for (i = 0; i < imgw; i++) {
			u8 * const pixel = &data[imgw * j + i];
			if (*pixel > max)
				die("%s has too many colors (%u)\n", in, *pixel);
			*pixel = table[*pixel];
		}
	}

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) die("PNG error\n");
	info = png_create_info_struct(png_ptr);
	if (!info) die("PNG error\n");
	if (setjmp(png_jmpbuf(png_ptr))) die("PNG error\n");

	png_init_io(png_ptr, f);
	png_set_IHDR(png_ptr, info, imgw, imgh, 4, PNG_COLOR_TYPE_PALETTE,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
			PNG_FILTER_TYPE_BASE);

	png_color outpal[16];
	memset(outpal, 0, sizeof(png_color) * 16);
	if (offset) {
		outpal[0] = inpal[0];
		for (i = 1; i < max; i++) {
			if (table[i] >= 16)
				continue;
			outpal[table[i]] = inpal[i];
		}
	} else {
		memcpy(outpal, srcpal, sizeof(png_color) * 16);
	}

	png_set_PLTE(png_ptr, info, outpal, 16);
	png_write_info(png_ptr, info);
	png_set_packing(png_ptr);

	for (i = 0; i < imgh; i++) {
		png_write_row(png_ptr, data + i * imgw);
	}
	png_write_end(png_ptr, NULL);

	fclose(f);
	free(data);
	png_destroy_info_struct(png_ptr, &info);
	png_destroy_write_struct(&png_ptr, NULL);

	printf("%s successfully converted.\n", in);
}

int main(int argc, char **argv) {

	const char *source = NULL;
	u8 offset = 0, i;

	if (argc < 2) {
		die("Usage: %s [--source src.png / --offset 4] dst.png ...\n",
			argv[0]);
	}

	const struct option opts[] = {
		{"source", 1, NULL, 's'},
		{"offset", 1, NULL, 'o'},
		{NULL, 0, NULL, 0}
	};

	while (1) {
		const int c = getopt_long(argc, argv, "s:o:", opts, NULL);
		if (c < 0)
			break;

		switch (c) {
			case 's':
				source = strdup(optarg);
			break;
			case 'o':
				offset = atoi(optarg);
			break;
		}
	}

	if (source && offset)
		die("Can't use both offset and source at the same time\n");

	if (source) {
		readsrc(source);

		for (i = optind; i < argc; i++)
			remap(argv[i], 0);
	} else if (offset) {
		for (i = optind; i < argc; i++)
			remap(argv[i], offset);
	}

	if (source) free((char *) source);
	return 0;
}
