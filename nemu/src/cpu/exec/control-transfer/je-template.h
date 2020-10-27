#include "cpu/exec/template-start.h"

#define instr je

static void do_execute() {
	if(cpu.ZF == 1) {
        int32_t exsrc = (int32_t)op_src->val;
        cpu.eip = cpu.eip + exsrc;
        if(DATA_BYTE == 2) {
            cpu.eip = cpu.eip & 0x0000ffff;
        }
    }
    print_asm_template1();
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
