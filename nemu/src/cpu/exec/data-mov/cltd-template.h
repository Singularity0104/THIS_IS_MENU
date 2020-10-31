#include "cpu/exec/template-start.h"

#define instr cltd

make_helper(concat(cltd_, SUFFIX)) {
    if(REG(R_EAX) < 0) {
        REG(R_EDX) = (DATA_TYPE)(~0);
    }
    else {
        REG(R_EDX) = 0;
    }
	print_asm("cltd_" str(SUFFIX));
	return 1;
}

#include "cpu/exec/template-end.h"
