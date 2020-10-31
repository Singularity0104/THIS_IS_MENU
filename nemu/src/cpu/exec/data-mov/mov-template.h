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
	uint32_t len = decode_rm2r_b(eip + 1);
	DATA_TYPE extend_val = (DATA_TYPE)(op_src->val);
	REG(op_dest->reg) = extend_val;
	print_asm("movzb_" str(SUFFIX));
	return len + 1;
}
#endif

#if DATA_BYTE == 4
make_helper(concat(movzw_, SUFFIX)) {
	uint32_t len = decode_rm2r_w(eip + 1);
	DATA_TYPE extend_val = (DATA_TYPE)(op_src->val);
	REG(op_dest->reg) = extend_val;
	print_asm("movzw_" str(SUFFIX));
	return len + 1;
}
#endif

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_helper(concat(movsb_, SUFFIX)) {
	uint32_t len = decode_rm2r_b(eip + 1);
	int8_t s_val = (int8_t)(op_src->val);
	DATA_TYPE_S extend_val = (DATA_TYPE_S)s_val;
	REG(op_dest->reg) = extend_val;
	print_asm("movsb_" str(SUFFIX));
	return len + 1;
}
#endif

#if DATA_BYTE == 4
make_helper(concat(movsw_, SUFFIX)) {
	uint32_t len = decode_rm2r_w(eip + 1);
	int16_t s_val = (int16_t)(op_src->val);
	DATA_TYPE_S extend_val = (DATA_TYPE_S)s_val;
	REG(op_dest->reg) = extend_val;
	print_asm("movsw_" str(SUFFIX));
	return len + 1;
}
#endif

make_helper(concat(movs_, SUFFIX)) {
	DATA_TYPE src = MEM_R(cpu.esi);
	MEM_W(cpu.edi, src);
	if(cpu.DF == 0) {
        cpu.esi = cpu.esi + DATA_BYTE;
        cpu.edi = cpu.edi + DATA_BYTE;
    }
    else {
        cpu.esi = cpu.esi - DATA_BYTE;
        cpu.edi = cpu.edi - DATA_BYTE;
    }
	print_asm("movs_" str(SUFFIX));
	return 1;
}

#include "cpu/exec/template-end.h"
