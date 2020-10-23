#include "cpu/exec/template-start.h"

#define instr je

static void do_execute() {
	if(cpu.ZF == 1) {
        cpu.eip = cpu.eip + op_src->val;
        if(DATA_BYTE == 2) {
            cpu.eip = cpu.eip & 0x0000ffff;
        }
    }
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"
