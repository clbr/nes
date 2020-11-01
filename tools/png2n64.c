#include <arpa/inet.h>
#include <png.h>
#include "common.h"

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
		PNG_TRANSFORM_PACKING|PNG_TRANSFORM_STRIP_16|PNG_TRANSFORM_STRIP_ALPHA|PNG_TRANSFORM_EXPAND, NULL);

	u8 **rows = png_get_rows(png_ptr, info);
	const u32 imgw = png_get_image_width(png_ptr, info);
	const u32 imgh = png_get_image_height(png_ptr, info);
	const u8 type = png_get_color_type(png_ptr, info);
	const u8 depth = png_get_bit_depth(png_ptr, info);

//	if (imgw % 8 != 0 || imgh % 8 != 0)
//		die("Image is not divisible by 8\n");

	if (type != PNG_COLOR_TYPE_RGB)
		die("Input must be a RGB PNG, got %u\n", type);

	if (depth != 8)
		die("Depth not 8 (%u) - maybe you have old libpng?\n", depth);

	const u32 rowbytes = png_get_rowbytes(png_ptr, info);
	if (rowbytes != imgw * 3)
		die("Packing failed, row was %u instead of %u\n", rowbytes, imgw);

	u8 * const data = calloc(imgw, imgh * 3);
	u32 i;
	for (i = 0; i < imgh; i++) {
		u8 * const target = data + imgw * i * 3;
		memcpy(target, &rows[i][0], imgw * 3);
	}

	fclose(f);
	png_destroy_info_struct(png_ptr, &info);
	png_destroy_read_struct(&png_ptr, NULL, NULL);

	*outdata = data;
	*w = imgw;
	*h = imgh;
}

int main(int argc, char **argv) {

	if (argc < 2) {
		die("Usage: %s in.png [out.bin]\n", argv[0]);
	}

	u32 imgw, imgh, x, y;
	u8 *img;

	loadpng(argv[1], &img, &imgw, &imgh);

	const u32 namelen = strlen(argv[1]);

	char outname[namelen + 1];
	strcpy(outname, argv[1]);
	outname[namelen - 1] = 'n';
	outname[namelen - 2] = 'i';
	outname[namelen - 3] = 'b';

	const char * const outptr = argc > 2 ? argv[2] : outname;

	FILE *out = fopen(outptr, "w");
	if (!out)
		die("Can't open output file '%s'\n", outptr);

	const u8 *ptr = img;
	for (y = 0; y < imgh; y++) {
		for (x = 0; x < imgw; x++) {
			u16 var = (ptr[0] & 0xf8) << 8 |
					(ptr[1] & 0xf8) << 3 |
					(ptr[2] & 0xf8) >> 2;
			ptr += 3;
			var = htons(var);
			if (fwrite(&var, 2, 1, out) != 1)
				die("Write error\n");
		}
	}

	fclose(out);
	free(img);
	return 0;
}
