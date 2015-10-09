#include <png.h>
#include "common.h"

struct tile {
	u32 x, y;
	u8 data[64];
};

static u8 pixel(const u8 * const data, const u32 x, const u32 y, const u32 w) {

	return data[y * w + x];
}

static int tilecmp(const void *ap, const void *bp) {
	const struct tile * const a = (struct tile *) ap;
	const struct tile * const b = (struct tile *) bp;

	return memcmp(a->data, b->data, 64);
}

static void handle(const u8 * const data, const u32 w, const u32 h) {

	const u32 rows = h / 8;
	const u32 cols = w / 8;
	const u32 numtiles = cols * rows;

	struct tile * const tiles = calloc(sizeof(struct tile), numtiles);
	u32 curtile = 0;

	u8 zerodata[64] = { 0 };

	u32 r, c;
	for (r = 0; r < rows; r++) {
		for (c = 0; c < cols; c++) {
			u32 x, y;
			u32 bufy = 0;
			for (y = r * 8; y < r * 8 + 8; y++, bufy++) {
				u32 bufx = 0;
				for (x = c * 8; x < c * 8 + 8; x++, bufx++) {
					const u8 pix = pixel(data, x, y, w);
					if (pix > 3)
						die("Palette has too many colors (%u) at %u,%u\n",
							pix, x, y);

					tiles[curtile].data[bufy * 8 + bufx] = pix;
				}
			}

			tiles[curtile].x = c * 8;
			tiles[curtile].y = r * 8;

			// Only non-zero tiles get checked
			if (memcmp(zerodata, tiles[curtile].data, 64))
				curtile++;
		}
	}

	qsort(tiles, curtile, sizeof(struct tile), tilecmp);

	for (r = 1; r < curtile; r++) {
		if (!memcmp(tiles[r - 1].data, tiles[r].data, 64))
			printf("Tiles at %u,%u and %u,%u are identical\n",
				tiles[r - 1].x,
				tiles[r - 1].y,
				tiles[r].x,
				tiles[r].y);
	}

	free(tiles);
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

	handle(data, imgw, imgh);

	free(data);
	return 0;
}
