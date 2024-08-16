#include <png.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "common.h"

// Create an overlay png that shows reused tiles in red

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

	if (argc < 2)
		die("Usage: %s coord [out.png]\n", argv[0]);

	char outnamebuf[4096], *outname;
	if (argc < 3) {
		outname = outnamebuf;
		sprintf(outnamebuf, "out.png");
	} else {
		outname = argv[2];
	}

	const png_color pal[16] = {
		{ 0, 0, 0 },
		{ 255, 0, 0 },
	};

	char buf[4096];
	u8 *uncomp = NULL;
	u32 w = 0, x = 0, y = 0;

	// Read coords
	u8 reused[256] = { 0 };
	FILE *f = fopen(argv[1], "r");
	if (!f) die("can't open %s\n", argv[1]);
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
			if (reused[tile] < 2)
				reused[tile]++;
		}

		y++;
		uncomp = realloc(uncomp, (y + 1) * w);
	}
	fclose(f);

	const u32 h = y;
	u8 *out = calloc(w * 8, h * 8);

	// Map to tiles
	for (y = 0; y < h; y++) {
		for (x = 0; x < w; x++) {
			u8 ty;
			const u8 tile = uncomp[y * w + x];
			u8 * const to = &out[y * 8 * 8 * w + x * 8];
			for (ty = 0; ty < 8; ty++) {
				memset(to + ty * 8 * w, reused[tile] > 1 ? 1 : 0, 8);
			}
		}
	}

	savepng(outname, out, w * 8, h * 8, pal);

	free(uncomp);
	free(out);
	return 0;
}
