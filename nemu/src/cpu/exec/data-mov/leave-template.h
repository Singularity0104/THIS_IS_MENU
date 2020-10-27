#include "cpu/exec/template-start.h"

#define instr leave

make_helper(concat(leave_, SUFFIX)) {
    cpu.esp = cpu.ebp;
    cpu.ebp =  MEM_R(cpu.esp);
    cpu.esp += DATA_BYTE;
    print_asm_template1();
    printf("%x    aaaaaaaaaaaaaaaaa\n", cpu.eip);
    return 1;
}

#include "cpu/exec/template-end.h"
