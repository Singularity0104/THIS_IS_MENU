#include <stdio.h>
#include <string.h>
#include "trap.h"

char buf[128];

int main() {
	sprintf(buf, "%s", "Hellooooooooooooooooooooo");
	nemu_assert(strcmp(buf, "Hellooooooooooooooooooooo") == 0);
	return 0;
}
