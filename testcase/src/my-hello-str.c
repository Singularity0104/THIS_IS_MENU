#include <stdio.h>
#include <string.h>
#include "trap.h"

char buf[128];

int main() {
	sprintf(buf, "%s", "Hell");
	nemu_assert(strcmp(buf, "Hell") == 0);
	return 0;
}
