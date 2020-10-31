#include <stdio.h>
#include <string.h>
#include "trap.h"

char buf[128];

int main() {
	sprintf(buf, "%s", "Hello");
	nemu_assert(strcmp(buf, "Hello") == 0);
	return 0;
}
