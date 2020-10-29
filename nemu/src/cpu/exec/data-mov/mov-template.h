#include "cpu/exec/template-start.h"

#define instr mov

static void do_execute() {
	OPERAND_W(op_dest, op_src->val);
	print_asm_template2();
}

make_instr_helper(i2r)
make_instr_helper(i2rm)
make_instr_helper(r2rm)
make_instr_helper(rm2r)

make_helper(concat(mov_a2moffs_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	MEM_W(addr, REG(R_EAX));

	print_asm("mov" str(SUFFIX) " %%%s,0x%x", REG_NAME(R_EAX), addr);
	return 5;
}

make_helper(concat(mov_moffs2a_, SUFFIX)) {
	swaddr_t addr = instr_fetch(eip + 1, 4);
	REG(R_EAX) = MEM_R(addr);

	print_asm("mov" str(SUFFIX) " 0x%x,%%%s", addr, REG_NAME(R_EAX));
	return 5;
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_helper(concat(movzb_, SUFFIX)) {
	printf("%xddddddddddddddddddddddddddddddddd", eip);
	uint32_t len = decode_rm2r_b(eip);
	DATA_TYPE extend_val = (DATA_TYPE)(op_src->val);
	REG(op_dest->reg) = extend_val;
	print_asm_template2();
	return len;
}
#endif


#include "cpu/exec/template-end.h"
