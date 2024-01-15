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
			a & PAD_UP ? "up " : "",
			a & PAD_DOWN ? "down " : "",
			a & PAD_LEFT ? "left " : "",
			a & PAD_RIGHT ? "right " : "",
			a & PAD_A ? "A " : "",
			a & PAD_B ? "B " : "",
			a & PAD_SELECT ? "select " : "",
			a & PAD_START ? "start " : "");
	}

	return 0;
}
