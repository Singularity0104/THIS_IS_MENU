#include <stdio.h>
#include <string.h>
#include "trap.h"

char buf[128];

int main() {
	sprintf(buf, "%s", "Hello world!\n");
	return 0;
}
