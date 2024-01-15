/*
	Shows drawing a movable arrow, all in C.

	All of Shiru's examples use a magic IRQ routine, making the drawing harder to
	understand. Here it's all plain C, usable in cc65 without any config or crt0
	changes.
*/

#include <joystick.h>
#include <nes.h>

#define PPUADDR ((unsigned char *) 0x2006)
#define PPUDATA ((unsigned char *) 0x2007)

static void vramwrite(const unsigned short ptr, const unsigned char val) {
	// The following can't use a pointer variable, due to a compiler issue.
	*PPUADDR = ptr >> 8;
	*PPUADDR = ptr;
	*PPUDATA = val;
}

int main() {

	unsigned char a;
	unsigned char *ppuctrl = (unsigned char *) 0x2000;
	unsigned char *ppumask = (unsigned char *) 0x2001;
	unsigned char *ppuscroll = (unsigned char *) 0x2005;
	unsigned char x = 16, y = 15, prevx = 7, prevy = 8;
	unsigned short i;

	joy_install(joy_static_stddrv);
	waitvblank();
	waitvblank();
	waitvblank();

	// Black background
	vramwrite(0x3f00, 0x3f);

	// White text
	vramwrite(0x3f01, 0x30);

	// Clear the nametable
	for (i = 0; i < 1024; i++)
		vramwrite(0x2000 + i, 0);

	*ppuscroll = 0;
	*ppuscroll = 0;

	*ppuctrl = 0x80;
	*ppumask = 0x1e;
	waitvblank();

	while (1) {
		const unsigned short addr = 0x2000 + 32 * y + x;
		const unsigned short prevaddr = 0x2000 + 32 * prevy + prevx;

		waitvblank();
		// Any graphics access has to happen in the vblank period, 2.2k cpu cycles
		if (prevaddr != addr) vramwrite(prevaddr, 0);
		vramwrite(addr, 2);

		// To end the drawing, reset the scrolling position.
		*ppuscroll = 0;
		*ppuscroll = 0;

		// Logic, while the PPU is drawing.
		a = joy_read(0);

		prevy = y;
		prevx = x;

		if (a & PAD_LEFT && x)
			x--;
		if (a & PAD_RIGHT && x < 31)
			x++;
		if (a & PAD_UP && y > 1)
			y--;
		if (a & PAD_DOWN && y < 28)
			y++;
	}

	return 0;
}
