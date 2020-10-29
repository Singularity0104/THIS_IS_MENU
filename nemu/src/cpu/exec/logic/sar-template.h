#include "cpu/exec/template-start.h"

#define instr sar

static void do_execute () {
	DATA_TYPE src = op_src->val;
	DATA_TYPE_S res = op_dest->val;
	uint8_t count = src & 0x1f;
	res >>= count;
	OPERAND_W(op_dest, res);

	/* TODO: Update EFLAGS. */
	// panic("please implement me");

	cpu.CF = (op_dest->val & (1 << (count - 1))) >> (count - 1);
    if(count == 1) {
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

make_instr_helper(rm_1)
make_instr_helper(rm_cl)
make_instr_helper(rm_imm)

#include "cpu/exec/template-end.h"
