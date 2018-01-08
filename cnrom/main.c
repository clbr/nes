#include <joystick.h>
#include <nes.h>
#include <stdint.h>
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


#define BANK ((char *) 0x8000)

static void fill() {

	u8 i;
	ppu_off();

	pal_col(1, 0x30);

	vram_adr(NAMETABLE_A);
	vram_fill(128, 960);
	vram_adr(NAMETABLE_A + 960);
	vram_fill(0, 40);

	for (i = 0; i < 32; i++) {
		vram_adr(NTADR_A(i, 1));

		if (i < 16)
			vram_put(i);
		else
			vram_put(i + 16);

		vram_adr(NTADR_A(i, 2));

		if (i < 16)
			vram_put(i + 16);
		else
			vram_put(i + 32);
	}

	ppu_on_all();
}

static void bankswitch(const u8 to) {

	static const u8 arr[] = {
		0, 1, 2, 3, 4
	};

	(u8) arr[to] = to;
}

int main() {

	u8 ctrl, prevctrl;
	u8 state = 0;

	joy_install(joy_static_stddrv);

	fill();

	while (1) {
		ctrl = joy_read(0);

		if (JOY_START(ctrl) && !JOY_START(prevctrl)) {
			state++;
			state %= 5;

			bankswitch(state);
		}

		ppu_wait_nmi();
		prevctrl = ctrl;
	}

	return 0;
}
