#include "cpu/exec/template-start.h"

#define instr xor

static void do_execute () {
	DATA_TYPE res = op_dest->val ^ op_src->val;
	OPERAND_W(op_dest, res);

	/* TODO: Update EFLAGS. */
	// panic("please implement me");
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

make_instr_helper(i2a)
make_instr_helper(i2rm)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm)
#endif
make_instr_helper(r2rm)
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
