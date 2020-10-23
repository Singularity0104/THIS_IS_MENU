#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute() {
	uint32_t res = 0;
    res = res + (op_dest->val - op_src->val);
    cpu.CF = 0;
    cpu.OF = 0;
    if(res == 0) {
        cpu.ZF = 1;
    }
    else {
        cpu.ZF = 0;
    }
    cpu.SF = (res & (1 << (DATA_BYTE * 8 - 1))) >> (DATA_BYTE * 8 - 1);
    uint32_t tmp = res & 1;
    int i;
    for(i = 1; i < 8; i++) {
        tmp = tmp ^ (res & (1 << i) >> i);
    }
    cpu.PF = !tmp;
    print_asm_template2();
}

make_instr_helper(i2rm)

#include "cpu/exec/template-end.h"
