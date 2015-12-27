#include <nes.h>
#include <conio.h>
#include <stdint.h>
#include <time.h>

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int32_t s32;
typedef int16_t s16;
typedef int8_t s8;

static u8 buf[768];

int main() {

	const u32 expected = 0x4f0f97e;
	u32 sum = 0;
	u16 blanks = 0;
	// Zero the timer out
	*(u16 *) 0x6b = 0;

	// Decompress

	blanks = clock();
	cprintf("%u blanks (%u s)", blanks, blanks / 60);
	gotoxy(0, 1);
	cprintf("checksum %lx", sum);
	gotoxy(0, 2);
	cprintf("expected %lx", expected);

	while (1) {
		waitvblank();
	}

	return 0;
}
