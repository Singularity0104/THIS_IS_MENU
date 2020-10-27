#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute() {
	uint32_t res = 0;
    res = res + (op_dest->val - op_src->val);
    if((uint32_t)op_dest->val < (uint32_t)op_src->val) {
        cpu.CF = 1;
    }
    else {
        cpu.CF = 0;
    }
    if(((op_dest->val >> (DATA_BYTE * 8 - 1)) ^ (op_src->val >> (8 * DATA_BYTE - 1))) & (!((res >> (DATA_BYTE * 8 - 1)) ^ (op_src->val >> (8 * DATA_BYTE - 1)))) & 1) {
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

make_instr_helper(rm_imm)

#include "cpu/exec/template-end.h"
