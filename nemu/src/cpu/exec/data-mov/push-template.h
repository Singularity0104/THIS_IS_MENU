#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
    cpu.esp -= DATA_BYTE;
    MEM_W(cpu.esp, REG(op_src->reg));
    print_asm_template2();
}

make_instr_helper(r);

#include "cpu/exec/template-end.h"
