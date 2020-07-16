#include <png.h>
#include "common.h"

#define SIDE 16
#define AREA (SIDE * SIDE)

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

	if (imgw % SIDE != 0 || imgh % SIDE != 0)
		die("Image is not divisible by %u\n", SIDE);

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
	u8 data[AREA];
};

static void horzflip(const struct tile_t * const src, struct tile_t * const dst) {
	u8 i, x;
	for (i = 0; i < SIDE; i++) {
		for (x = 0; x < SIDE; x++) {
			dst->data[i * SIDE + x] = src->data[i * SIDE + (SIDE - 1 - x)];
		}
	}
}

static void vertflip(const struct tile_t * const src, struct tile_t * const dst) {
	u8 i;
	for (i = 0; i < SIDE; i++) {
		memcpy(&dst->data[(SIDE - 1 - i) * SIDE], &src->data[i * SIDE], SIDE);
	}
}

static void spacer(const u32 i, const u32 perrow, const char * const flips) {
	if (i % perrow != perrow - 1) {
		if (flips)
			printf(", ");
		else
			printf(" ");
	}
}

static u32 trans(const u32 in) {
	// Translate the tile number to 8px coords
	#define NUM_PER_ROW (128 / SIDE)
	const u32 x = in % NUM_PER_ROW;
	const u32 y = in / NUM_PER_ROW;

	return y * 2 * 16 + x * 2;
}

int main(int argc, char **argv) {

	u8 retval = 0;
	const char *flips = getenv("flips");

	if (argc < 3) {
		die("Usage: %s tilemap.png sprite.png\n", argv[0]);
	}

	u8 *tilemap, *img;
	struct tile_t *flipv = NULL, *fliph = NULL, *fliphv = NULL;
	u32 tilew, tileh, imgw, imgh, x, y;

	loadpng(argv[1], &tilemap, &tilew, &tileh);
	loadpng(argv[2], &img, &imgw, &imgh);

	// Preprocess the tilemap into an easy-to-search format.
	const u32 numtiles = tilew * tileh / AREA;
	struct tile_t *tiles = calloc(numtiles, sizeof(struct tile_t));

	u32 i;
	for (i = 0; i < numtiles; i++) {
		y = (i * SIDE) / tilew;
		x = (i * SIDE) % tilew;

		y *= SIDE;

		const u32 endy = y + SIDE;
		const u32 starty = y;

		u8 pix = 0;
		for (y = starty; y < endy; y++) {
			memcpy(tiles[i].data + pix * SIDE,
				tilemap + y * tilew + x, SIDE);
			pix++;
		}
		if (pix != SIDE) die("BUG, pix %u\n", pix);

/*		printf("Read tile %u:\n", i);
		u32 k;
		for (k = 0; k < AREA; k++) {
			printf("%u ", tiles[i].data[k]);
			if (k % SIDE == SIDE - 1) puts("");
		}*/
	}

	// Make flipped copies for checks
	if (flips) {
		flipv = calloc(numtiles, sizeof(struct tile_t));
		fliph = calloc(numtiles, sizeof(struct tile_t));
		fliphv = calloc(numtiles, sizeof(struct tile_t));

		for (i = 0; i < numtiles; i++) {
			horzflip(&tiles[i], &fliph[i]);
			vertflip(&tiles[i], &flipv[i]);
			vertflip(&fliph[i], &fliphv[i]);
		}
	}

	// For each tile in img, find its number in tilemap.
	u8 curtile[AREA] = { 0 };
	const u32 imgtiles = imgw * imgh / AREA;
	const u32 perrow = imgw / SIDE;
	for (i = 0; i < imgtiles; i++) {
		y = (i * SIDE) / imgw;
		x = (i * SIDE) % imgw;

		y *= SIDE;

		const u32 endy = y + SIDE;
		const u32 starty = y;

		u8 pix = 0;
		u32 k;
		for (y = starty; y < endy; y++) {
			memcpy(&curtile[pix * SIDE],
				img + y * imgw + x, SIDE);
			pix++;
		}
		if (pix != SIDE) die("BUG, pix %u\n", pix);

/*		puts("Looking for tile:");
		for (k = 0; k < AREA; k++) {
			printf("%u ", curtile[k]);
			if (k % SIDE == SIDE - 1) puts("");
		}*/

		u8 found = 0;
		for (k = 0; k < numtiles; k++) {
			if (!memcmp(curtile, tiles[k].data, AREA)) {
				found = 1;
				printf("0x%02x", trans(k));
				spacer(i, perrow, flips);
				break;
			} else if (flips && !memcmp(curtile, flipv[k].data, AREA)) {
				found = 1;
				printf("0x%02x|FLIPV", trans(k));
				spacer(i, perrow, flips);
				break;
			} else if (flips && !memcmp(curtile, fliph[k].data, AREA)) {
				found = 1;
				printf("0x%02x|FLIPH", trans(k));
				spacer(i, perrow, flips);
				break;
			} else if (flips && !memcmp(curtile, fliphv[k].data, AREA)) {
				found = 1;
				printf("0x%02x|FLIPH|FLIPV", trans(k));
				spacer(i, perrow, flips);
				break;
			}
		}

		if (!found) {
			printf("NONE ");
			retval = 1;
		}

		if (i % perrow == perrow - 1) {
			if (flips)
				puts(",");
			else
				puts("");
		}
	}

	free(tilemap);
	free(img);
	free(tiles);
	free(flipv);
	free(fliph);
	free(fliphv);
	return retval;
}
