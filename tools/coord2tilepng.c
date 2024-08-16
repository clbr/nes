#include <png.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "common.h"

// Inverse coord2png; creates a tileset from an image and coords

static u8 *readpng(const char *name, u32 *w, u32 *h, png_color *outpal) {
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

	png_color *col;
	int num;
	png_get_PLTE(png_ptr, info, &col, &num);
	if (num > 16)
		die("%s has over 16 colors\n", name);
	memcpy(outpal, col, num * sizeof(png_color));

	fclose(f);
	png_destroy_info_struct(png_ptr, &info);
	png_destroy_read_struct(&png_ptr, NULL, NULL);

	*w = imgw;
	*h = imgh;

	return data;
}

static void savepng(const char *name, const u8 * const data, const u32 w, const u32 h,
			const png_color palette[16]) {

	FILE *f = fopen(name, "w");
	if (!f)
		die("Can't open file\n");

	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr) die("PNG error\n");
	png_infop info = png_create_info_struct(png_ptr);
	if (!info) die("PNG error\n");
	if (setjmp(png_jmpbuf(png_ptr))) die("PNG error\n");

	printf("Saving to a %ux%u PNG.\n", w, h);

	png_init_io(png_ptr, f);
	png_set_IHDR(png_ptr, info, w, h, 2, PNG_COLOR_TYPE_PALETTE, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	png_set_PLTE(png_ptr, info, (png_color *) palette, 16);
	png_write_info(png_ptr, info);
	png_set_packing(png_ptr);

	// Write
	u32 i;
	for (i = 0; i < h; i++) {
		png_write_row(png_ptr, (void *) data + i * w);
	}
	png_write_end(png_ptr, NULL);

	png_destroy_info_struct(png_ptr, &info);
	png_destroy_write_struct(&png_ptr, NULL);
	fclose(f);
}

int main(int argc, char **argv) {

	if (argc < 3)
		die("Usage: %s coord pic.png [out.png]\n", argv[0]);

	char outnamebuf[4096], *outname;
	if (argc < 4) {
		outname = outnamebuf;
		sprintf(outnamebuf, "out.png");
	} else {
		outname = argv[3];
	}

	u32 inw, inh;
	png_color pal[16] = {{ 0, 0, 0 }};
	const u8 *tiles = readpng(argv[2], &inw, &inh, pal);

	char buf[4096];
	u8 *uncomp = NULL;
	u32 w = 0, x = 0, y = 0;

	// Read coords
	FILE *f = fopen(argv[1], "r");
	while (fgets(buf, 4096, f)) {
		if (!w) {
			for (x = 0; buf[x]; x++) {
				if (buf[x] == 'x')
					w++;
			}

			uncomp = calloc(1, w);
		}

		const char *ptr = buf;
		for (x = 0; x < w; x++) {
			const u8 tile = strtol(ptr, NULL, 16);
			ptr += 5;
			uncomp[y * w + x] = tile;
		}

		y++;
		uncomp = realloc(uncomp, (y + 1) * w);
	}
	fclose(f);

	const u32 h = y;
	u8 *out = calloc(128, 128);

	// Map to tiles
	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			u8 ty;
			const u8 tile = uncomp[y * w + x];
			const u16 sy = tile / 16;
			const u16 sx = tile % 16;
			u8 * const to = &out[sy * 8 * 8 * 16 + sx * 8];
			const u8 * const from = &tiles[y * 8 * inw + x * 8];
			for (ty = 0; ty < 8; ty++) {
				memcpy(to + ty * 8 * 16, from + ty * inw, 8);
			}
		}
	}

	savepng(outname, out, 128, 128, pal);

	free(uncomp);
	free((void *) tiles);
	free(out);
	return 0;
}
