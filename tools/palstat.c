#include <png.h>
#include "common.h"

struct pal_t {
	u8 num;
	u8 colors[16 * 3];
};

struct pal_t *getpalette(FILE * const f, const char * const name) {

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	if (!png_ptr) die("PNG error\n");
	png_infop info = png_create_info_struct(png_ptr);
	if (!info) die("PNG error\n");
	if (setjmp(png_jmpbuf(png_ptr))) die("PNG error\n");

	png_init_io(png_ptr, f);
	png_read_info(png_ptr, info);

	struct pal_t *cur = calloc(1, sizeof(struct pal_t));

	const u8 type = png_get_color_type(png_ptr, info);
	if (type != PNG_COLOR_TYPE_PALETTE)
		die("%s is not paletted\n", name);

	png_color *color;
	int num;
	png_get_PLTE(png_ptr, info, &color, &num);

	if (num > 16)
		die("%s has over 16 colors (%u)\n", name, num);

	cur->num = num;

	u32 i;
	for (i = 0; i < (u32) num; i++) {
		cur->colors[i * 3 + 0] = color[i].red;
		cur->colors[i * 3 + 1] = color[i].green;
		cur->colors[i * 3 + 2] = color[i].blue;
	}

	png_destroy_info_struct(png_ptr, &info);
	png_destroy_read_struct(&png_ptr, NULL, NULL);

	return cur;
}

int main(int argc, char **argv) {

	if (argc < 2)
		die("Usage: %s file*png\n\n"
			"This is useful for checking how many palettes are used,\n"
			"and how they are distributed.\n\n"
			"files=1 to show file names.\n", argv[0]);

	struct pal_t *pals = calloc(argc - 1, sizeof(struct pal_t));
	u32 used = 0;
	u32 *name2pal = calloc(argc - 1, sizeof(u32));

	u32 i;
	for (i = 1; i < (u32) argc; i++) {
		FILE *f = fopen(argv[i], "r");
		if (!f)
			die("Can't open %s\n", argv[i]);

		struct pal_t *cur = getpalette(f, argv[i]);

		fclose(f);

		u32 k;
		for (k = 0; k < used; k++) {
			if (memcmp(&pals[k], cur, sizeof(struct pal_t)) == 0) {
				name2pal[i - 1] = k;
				goto next;
			}
		}

		// Not found, add it.
		memcpy(&pals[used], cur, sizeof(struct pal_t));
		name2pal[i - 1] = used;
		used++;

		next:
		free(cur);
	}

	// Print stats
	const char * const files = getenv("files");

	printf("%u palettes total.\n\n", used);
	for (i = 0; i < used; i++) {

		u32 many = 0;
		u32 k;
		for (k = 0; k < (u32) argc - 1; k++) {
			if (name2pal[k] == i)
				many++;
		}

		printf("Pal %u: %u colors, %.2f%%, %u/%u\n",
			i, pals[i].num, many * 100 / (float) (argc - 1),
			many, argc - 1);

		for (k = 0; k < pals[i].num; k++) {
			printf("\t%u %u %u\n",
				pals[i].colors[k * 3 + 0],
				pals[i].colors[k * 3 + 1],
				pals[i].colors[k * 3 + 2]);
		}

		if (files) {
			puts("");
			for (k = 0; k < (u32) argc - 1; k++) {
				if (name2pal[k] == i)
					printf("	%s\n",
						argv[k + 1]);
			}
		}

		puts("");
	}

	free(pals);
	free(name2pal);
	return 0;
}
