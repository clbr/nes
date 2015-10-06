#include "common.h"

void die(const char fmt[], ...) {

	va_list ap;
	va_start(ap, fmt);

	vfprintf(stderr, fmt, ap);

	va_end(ap);
	exit(1);
}
