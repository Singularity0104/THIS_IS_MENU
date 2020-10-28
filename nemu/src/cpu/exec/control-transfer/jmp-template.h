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

#include "cpu/exec/template-end.h"
