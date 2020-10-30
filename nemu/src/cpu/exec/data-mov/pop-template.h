#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
    REG(op_src->reg) =  MEM_R(cpu.esp);
    uint32_t offset = 4;
    if(DATA_BYTE == 2) {
        offset = 2;
    }
    cpu.esp = cpu.esp + offset;
    print_asm_template1();
}

make_instr_helper(r)

#include "cpu/exec/template-end.h"
