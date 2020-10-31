#include <stdio.h>
#include <string.h>
#include "trap.h"

char buf[128];

int main() {
	sprintf(buf, "%s", "Hello world!\n");
	nemu_assert(strcmp(buf, "Hello world!\n") == 0);
	return 0;
}
