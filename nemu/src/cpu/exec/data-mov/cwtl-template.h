#include "cpu/exec/template-start.h"

#define instr cwtl

make_helper(concat(cwtl_, SUFFIX)) {
    if(DATA_BYTE == 2) {
        int16_t val = (int8_t)reg_b(R_EAX);
        reg_w(R_EAX) = val;
    }
    else if(DATA_BYTE == 4) {
        int32_t val = (int16_t)reg_w(R_EAX);
        reg_l(R_EAX) = val;
    }
	print_asm("cwtl_" str(SUFFIX));
	return 1;
}

#include "cpu/exec/template-end.h"
