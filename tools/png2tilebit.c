#include <png.h>
#include "common.h"

static u8 pixel(const u8 * const data, const u32 x, const u32 y, const u32 w) {

	return data[y * w + x];
}

static void save(const u8 * const data, const u32 w, const u32 h) {
	const u32 rows = h / 8;
	const u32 cols = w / 8;

	fprintf(stderr, "Converting %ux%u PNG to %u bit tile arrays.\n",
		w, h, rows * cols);

	printf("const u8 tilebits[8 * %u] = {\n", rows * cols);

	u8 buf;
	u32 r, c;
	for (r = 0; r < rows; r++) {
		for (c = 0; c < cols; c++) {

			u32 x, y;
			buf = 0;
			printf("\t");
			u8 iny = 0;
			for (y = r * 8; y < r * 8 + 8; y++, iny++) {
				u32 bufx = 0;
				for (x = c * 8; x < c * 8 + 8; x++, bufx++) {
					const u8 pix = pixel(data, x, y, w);
					if (pix)
						buf |= 1 << bufx;
				}
				printf("0x%02x,", buf);
				if (iny < 7)
					printf(" ");
				else
					printf("\n");
			}
		}
	}

	puts("};");
}

int main(int argc, char **argv) {

	if (argc < 2) {
		die("Usage: %s file.png\n", argv[0]);
	}

	FILE *f = fopen(argv[1], "r");
	if (!f)
		die("Can't open file\n");

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

	int num;
	png_color *ptr;
	png_get_PLTE(png_ptr, info, &ptr, &num);
	//if (num > 2)
	//	die("Palette has %u colors, 2 expected\n", num);

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

	save(data, imgw, imgh);

	free(data);
	return 0;
}
