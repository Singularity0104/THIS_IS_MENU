#include <stdio.h>
#include <string.h>
#include "trap.h"

char buf[128];

int main() {
	buf[0] = 'a';
	buf[1] = 'a';
	buf[2] = 'a';
	buf[3] = 'a';
	buf[4] = 'a';
	buf[5] = 'a';
	buf[6] = 'a';
	buf[7] = 'a';
	buf[8] = 0;
	nemu_assert(strcmp(buf, "aaaaaaaa") == 0);
	return 0;
}
