#include <nes.h>

int main() {

	unsigned char *apustatus = (unsigned char *) 0x4015;
	unsigned char *pulse0 = (unsigned char *) 0x4000;
	unsigned char *pulse1 = (unsigned char *) 0x4001;
	unsigned char *pulse2 = (unsigned char *) 0x4002;
	unsigned char *pulse3 = (unsigned char *) 0x4003;
	unsigned char note = 0;

	*apustatus = 1;
	*pulse0 = 0xbf;
	*pulse2 = 0;
	*pulse3 = 0;

	while (1) {
		waitvblank();

		*pulse2 = note;
		note += 8;
	}

	return 0;
}
