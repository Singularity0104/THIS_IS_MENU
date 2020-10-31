#include <stdio.h>
#include <string.h>
#include "trap.h"

char buf[] = "fuck_nemu";

int main() {
	nemu_assert(strlen(buf) == 9);
	return 0;
}
