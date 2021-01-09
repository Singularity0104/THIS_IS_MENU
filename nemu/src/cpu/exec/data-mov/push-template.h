#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
    uint32_t offset = 4;
    if(DATA_BYTE == 2) {
        offset = 2;
    }
    cpu.esp = cpu.esp - offset;
    MEM_W(cpu.esp, op_src->val, R_SS);
    print_asm_template1();
}

make_instr_helper(r)
make_instr_helper(rm)
make_instr_helper(i)

#include "cpu/exec/template-end.h"
