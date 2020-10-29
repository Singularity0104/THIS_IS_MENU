#include "cpu/exec/template-start.h"
// #include "cpu/decode/modrm.h"

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
	// op_src->size = 1;
	// int len = read_ModR_M(cpu.eip, op_src, op_dest);
	// op_dest->val = REG(op_dest->reg);
	// DATA_TYPE extend_val = (DATA_TYPE)op_src->val;
	// OPERAND_W(op_dest, extend_val);
	// print_asm_template2();
	uint32_t len = concat(decode_rm2r_, SUFFIX)(cpu.eip);
	DATA_TYPE extend_val = swaddr_read(op_src->addr, 1);
	OPERAND_W(op_dest, extend_val);
	print_asm_template2();
	return len;
}
#endif


#include "cpu/exec/template-end.h"
