#include <stdio.h>
#include <string.h>
#include "trap.h"

char buf[] = "fuck_nemu";

int main() {
	nemu_assert(strcmp(buf, "fuck_nemu") == 0);

	return 0;
}
