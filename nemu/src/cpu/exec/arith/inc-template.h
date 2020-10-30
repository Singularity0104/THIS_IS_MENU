#include "cpu/exec/template-start.h"

#define instr inc

static void do_execute () {
	DATA_TYPE res = op_src->val + 1;
	OPERAND_W(op_src, res);

	/* TODO: Update EFLAGS. */
	// panic("please implement me");
	if((uint32_t)res < (uint32_t)op_src->val || (uint32_t)res < 1u) {
        cpu.CF = 1;
    }
    else {
        cpu.CF = 0;
    }
    if((~(0 ^ (op_src->val >> (8 * DATA_BYTE - 1)))) & ((res >> (DATA_BYTE * 8 - 1)) ^ (op_src->val >> (8 * DATA_BYTE - 1))) & 1) {
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

	print_asm_template1();
}

make_instr_helper(rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
#endif

#include "cpu/exec/template-end.h"
