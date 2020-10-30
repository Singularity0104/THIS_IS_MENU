#include "cpu/exec/template-start.h"

#define instr jmp

static void do_execute() {
	DATA_TYPE_S offset = op_src->val;
	cpu.eip = cpu.eip + offset;
	if(DATA_BYTE == 2) {
		cpu.eip = cpu.eip & 0x0000ffff;
	}
	print_asm_template1();
}

make_instr_helper(i)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_helper(concat(jmp_rm_, SUFFIX)) {
	uint32_t len = concat(decode_rm_, SUFFIX)(eip + 1);
	swaddr_t addr = op_src->val;
	cpu.eip = addr;
	if(DATA_BYTE == 2) {
		cpu.eip = cpu.eip & 0x0000ffff;
	}
	print_asm_template1();
	return len + 1;
}
#endif

#include "cpu/exec/template-end.h"
