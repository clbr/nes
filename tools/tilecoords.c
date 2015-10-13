#include <png.h>
#include "common.h"

static u8 pixel(const u8 * const data, const u32 x, const u32 y, const u32 w) {

	return data[y * w + x];
}

static void loadpng(const char name[], u8 **outdata, u32 *w, u32 *h) {

	FILE *f = fopen(name, "r");
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

	*outdata = data;
	*w = imgw;
	*h = imgh;
}

struct tile_t {
	u8 data[64];
};

int main(int argc, char **argv) {

	u8 retval = 0;

	if (argc < 3) {
		die("Usage: %s tilemap.png sprite.png\n", argv[0]);
	}

	u8 *tilemap, *img;
	u32 tilew, tileh, imgw, imgh, x, y;

	loadpng(argv[1], &tilemap, &tilew, &tileh);
	loadpng(argv[2], &img, &imgw, &imgh);

	// Preprocess the tilemap into an easy-to-search format.
	const u32 numtiles = tilew * tileh / 64;
	struct tile_t *tiles = calloc(numtiles, sizeof(struct tile_t));

	u32 i;
	for (i = 0; i < numtiles; i++) {
		y = (i * 8) / tilew;
		x = (i * 8) % tilew;

		y *= 8;

		const u32 endx = x + 8;
		const u32 endy = y + 8;
		const u32 starty = y;

		u8 pix = 0;
		for (; x < endx; x++) {
			for (y = starty; y < endy; y++) {
				tiles[i].data[pix++] = pixel(tilemap, x, y, tilew);
			}
		}
		if (pix != 64) die("BUG, pix %u\n", pix);

/*		printf("Read tile %u:\n", i);
		u32 k;
		for (k = 0; k < 64; k++) {
			printf("%u ", tiles[i].data[k]);
			if (k % 8 == 7) puts("");
		}*/
	}

	// For each tile in img, find its number in tilemap.
	u8 curtile[64] = { 0 };
	const u32 imgtiles = imgw * imgh / 64;
	const u32 perrow = imgw / 8;
	for (i = 0; i < imgtiles; i++) {
		y = (i * 8) / imgw;
		x = (i * 8) % imgw;

		y *= 8;

		const u32 endx = x + 8;
		const u32 endy = y + 8;
		const u32 starty = y;

		u8 pix = 0;
		u32 k;
		for (; x < endx; x++) {
			for (y = starty; y < endy; y++) {
				curtile[pix++] = pixel(img, x, y, imgw);
			}
		}
		if (pix != 64) die("BUG, pix %u\n", pix);

/*		puts("Looking for tile:");
		for (k = 0; k < 64; k++) {
			printf("%u ", curtile[k]);
			if (k % 8 == 7) puts("");
		}*/

		u8 found = 0;
		for (k = 0; k < numtiles; k++) {
			if (!memcmp(curtile, tiles[k].data, 64)) {
				found = 1;
				printf("0x%02x ", k);
				break;
			}
		}

		if (!found) {
			printf("NONE ");
			retval = 1;
		}

		if (i % perrow == perrow - 1)
			puts("");
	}

	free(tilemap);
	free(img);
	free(tiles);
	return retval;
}
