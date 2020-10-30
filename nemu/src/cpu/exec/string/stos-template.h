#include "cpu/exec/template-start.h"

#define instr stos

make_helper(concat(stos_, SUFFIX)) {
    MEM_W(cpu.edi, REG(R_EAX));
    if(cpu.DF == 0) {
        cpu.edi = cpu.edi + DATA_BYTE;
    }
    else {
        cpu.edi = cpu.edi - DATA_BYTE;
    }
    return 1;
}

#include "cpu/exec/template-end.h"
