#include <png.h>
#include "common.h"

static u8 pixel(const u8 * const data, const u32 x, const u32 y, const u32 w) {

	return data[y * w + x];
}

static void savechr(FILE *f, const u8 * const data, const u32 w, const u32 h) {

	const u32 rows = h / 8;
	const u32 cols = w / 8;

	if (cols * rows > 1024)
		die("Too large to fit in a GBA page\n");

	printf("Converting %ux%u PNG to %u GBA tiles.\n",
		w, h, rows * cols);

	u32 r, c;
	for (r = 0; r < rows; r++) {
		for (c = 0; c < cols; c++) {
			u32 x, y;
			for (y = r * 8; y < r * 8 + 8; y++) {
				for (x = c * 8; x < c * 8 + 8; x += 2) {
					const u8 left = pixel(data, x, y, w);
					const u8 right = pixel(data, x + 1, y, w);
					if (left > 15)
						die("Palette has too many colors (%u) at %u,%u\n",
							left, x, y);
					if (right > 15)
						die("Palette has too many colors (%u) at %u,%u\n",
							right, x + 1, y);

					const u8 byte = left | (right << 4);
					fwrite(&byte, 1, 1, f);
				}
			}
		}
	}
}

int main(int argc, char **argv) {

	if (argc < 2) {
		die("Usage: %s file.png [file.gbt]\n", argv[0]);
	}

	FILE *f = fopen(argv[1], "r");
	if (!f)
		die("Can't open file\n");

	const u32 namelen = strlen(argv[1]);

	char outname[namelen + 1];
	strcpy(outname, argv[1]);
	outname[namelen - 1] = 't';
	outname[namelen - 2] = 'b';
	outname[namelen - 3] = 'g';

	const char * const outptr = argc > 2 ? argv[2] : outname;

	FILE *out = fopen(outptr, "w");
	if (!out)
		die("Can't open output file '%s'\n", outptr);

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	if (!png_ptr) die("PNG error\n");
	png_infop info = png_create_info_struct(png_ptr);
	if (!info) die("PNG error\n");
	if (setjmp(png_jmpbuf(png_ptr))) die("PNG error\n");

	png_init_io(png_ptr, f);
	png_read_png(png_ptr, info,
		PNG_TRANSFORM_PACKING|PNG_TRANSFORM_STRIP_16|PNG_TRANSFORM_STRIP_ALPHA, NULL);

	u8 **rows = png_get_rows(png_ptr, info);
	const u32 imgw = png_get_image_width(png_ptr, info);
	const u32 imgh = png_get_image_height(png_ptr, info);
	const u8 type = png_get_color_type(png_ptr, info);
	const u8 depth = png_get_bit_depth(png_ptr, info);

	if (imgw % 8 != 0 || imgh % 8 != 0)
		die("Image is not divisible by 8\n");

	if (type != PNG_COLOR_TYPE_PALETTE)
		die("Input must be a paletted PNG, got %u\n", type);

	if (depth != 8)
		die("Depth not 8 (%u) - maybe you have old libpng?\n", depth);

	const u32 rowbytes = png_get_rowbytes(png_ptr, info);
	if (rowbytes != imgw)
		die("Packing failed, row was %u instead of %u\n", rowbytes, imgw);

	u8 * const data = calloc(imgw, imgh);
	u32 i;
	for (i = 0; i < imgh; i++) {
		u8 * const target = data + imgw * i;
		memcpy(target, &rows[i][0], imgw);
	}

	fclose(f);
	png_destroy_info_struct(png_ptr, &info);
	png_destroy_read_struct(&png_ptr, NULL, NULL);

	savechr(out, data, imgw, imgh);

	free(data);
	fclose(out);
	return 0;
}
