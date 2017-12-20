#include <joystick.h>
#include <nes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "neslib.h"

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

#pragma bss-name (push,"ZEROPAGE")
#pragma data-name (push,"ZEROPAGE")

u8 oam_off;

#pragma data-name(pop)
#pragma bss-name (pop)

void str(const char *msg) {
	while (*msg) {
		vram_put(*msg - ' ');
		msg++;
	}
}

static void fill() {

	ppu_off();
	vram_adr(NAMETABLE_A);

	// Fill nametable A
	vram_fill(0x3b, 960);

	vram_adr(NTADR_A(3, 2));
	str("HUD");

	// Put B nametable B, C on C...
	vram_adr(NAMETABLE_B);
	vram_fill('B' - ' ', 960);

	vram_adr(NAMETABLE_C);
	vram_fill('C' - ' ', 960);

	vram_adr(NAMETABLE_D);
	vram_fill('D' - ' ', 960);

	ppu_on_all();
}

void splitXY(unsigned x, unsigned y);

int main() {

	char buf[32];
	static char bgbuf[256];

	u8 ctrl, prevctrl;
	u8 i;
	u16 x = 240, y = 128;
	u8 frames = 0, len;

	bgbuf[0] = NT_UPD_EOF;
	set_vram_update(bgbuf);

	joy_install(joy_static_stddrv);

	pal_col(0, 0xf);
	pal_col(1, 0x30);
	pal_col(2, 0x31); // blue
	pal_col(3, 0x35); // pink

	pal_col(17, 0x30);

	fill();
	oam_clear();

	// Sprite 0 hit
	oam_spr(0, 31, 0x3b, 0, 0);

	while (1) {
		bgbuf[0] = NT_UPD_EOF;
		ctrl = joy_read(0);

		//split(x, y);
		splitXY(x, y);

		sprintf(buf, "  %u %u   ", x, y);
		len = strlen(buf);
		// Update via the queue
		bgbuf[0] = MSB(NTADR_A(8, 2)) | NT_UPD_HORZ;
		bgbuf[1] = LSB(NTADR_A(8, 2));
		bgbuf[2] = len;

		for (i = 0; i < len; i++) {
			bgbuf[i + 3] = buf[i] - ' ';
		}
		bgbuf[i + 3] = NT_UPD_EOF;

		// slow scrolling
		if (frames % 2 == 0) {
//			++x;
//			if (x == 512)
//				x = 0;

			++y;
			if (y == 480)
				y = 0;
		}

		ppu_wait_nmi();
		++frames;
		prevctrl = ctrl;
	}

	return 0;
}
