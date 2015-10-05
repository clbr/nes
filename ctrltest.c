#include <conio.h>
#include <joystick.h>
#include <nes.h>

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
