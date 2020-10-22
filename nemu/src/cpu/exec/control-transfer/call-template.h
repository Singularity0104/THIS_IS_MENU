#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
    cpu.esp -= DATA_BYTE;
    if(DATA_BYTE == 2) {
        uint16_t ip = (uint16_t)(cpu.eip & 0x0000ffff);
        MEM_W(cpu.esp, ip);
        cpu.eip = (cpu.eip + op_src->imm) & 0x0000ffff;
    }
    else if(DATA_BYTE == 4) {
        MEM_W(cpu.esp, cpu.eip);
        cpu.eip = cpu.eip + op_src->imm;
    }
	print_asm_template2();
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"