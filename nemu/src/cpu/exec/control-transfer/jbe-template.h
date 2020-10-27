#include "cpu/exec/template-start.h"

#define instr jbe

static void do_execute() {
	if(cpu.ZF == 1 || cpu.CF == 1) {
        // uint32_t sign = op_src->val & (1 << (DATA_BYTE * 8 - 1));
        // sign = sign << (())
        cpu.eip = cpu.eip + ((op_src->val << ((4 - DATA_BYTE) * 8)) >> ((4 - DATA_BYTE) * 8));
        if(DATA_BYTE == 2) {
            cpu.eip = cpu.eip & 0x0000ffff;
        }
    }
    print_asm_template1();
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
