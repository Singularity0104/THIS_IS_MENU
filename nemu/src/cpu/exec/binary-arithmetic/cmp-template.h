#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute() {
	uint32_t res = 0;
    res = ((DATA_TYPE_S)op_dest->val - (DATA_TYPE_S)op_src->val) & (((1u << (DATA_BYTE * 8 - 1)) - 1) + (1u << (DATA_BYTE * 8 - 1)));
    if((uint32_t)op_dest->val < (uint32_t)op_src->val) {
        cpu.CF = 1;
    }
    else {
        cpu.CF = 0;
    }
    if(((op_dest->val >> (DATA_BYTE * 8 - 1)) ^ (op_src->val >> (8 * DATA_BYTE - 1))) & (~((res >> (DATA_BYTE * 8 - 1)) ^ (op_src->val >> (8 * DATA_BYTE - 1)))) & 1) {
        cpu.OF = 1;
    }
    else {
        cpu.OF = 0;
    }
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

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif
make_instr_helper(i2rm)
make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"
