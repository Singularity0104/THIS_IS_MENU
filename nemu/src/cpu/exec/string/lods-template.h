#include "cpu/exec/template-start.h"

#define instr lods

make_helper(concat(lods_, SUFFIX)) {
    REG(R_EAX) = MEM_R(cpu.esi, R_DS);
    if(cpu.DF == 0) {
        cpu.esi = cpu.esi + DATA_BYTE;
    }
    else {
        cpu.esi = cpu.esi - DATA_BYTE;
    }
    print_asm(str(instr) str(SUFFIX));
    return 1;
}

#include "cpu/exec/template-end.h"
