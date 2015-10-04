#include <conio.h>
#include <joystick.h>
#include <nes.h>

#define KEY_UP 0x10
#define KEY_DOWN 0x20
#define KEY_LEFT 0x40
#define KEY_RIGHT 0x80
#define KEY_A 0x1
#define KEY_B 0x2
#define KEY_SELECT 0x4
#define KEY_START 0x8

int main() {

	unsigned char a;

	joy_install(joy_static_stddrv);

	while (1) {
		waitvblank();

		a = joy_read(0);

		gotox(0);
		gotoy(0);

		cprintf("Keys: %s%s%s%s%s%s%s%s                           ",
			a & KEY_UP ? "up " : "",
			a & KEY_DOWN ? "down " : "",
			a & KEY_LEFT ? "left " : "",
			a & KEY_RIGHT ? "right " : "",
			a & KEY_A ? "A " : "",
			a & KEY_B ? "B " : "",
			a & KEY_SELECT ? "select " : "",
			a & KEY_START ? "start " : "");
	}

	return 0;
}
