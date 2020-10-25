#include "cpu/exec/template-start.h"

#define instr ret

make_helper(concat(ret_, SUFFIX)) {
    if(DATA_BYTE == 2) {
        cpu.eip =  MEM_R(cpu.esp);
        cpu.eip = cpu.eip & 0x0000ffff;
    }
    else if(DATA_BYTE == 4) {
        cpu.eip =  MEM_R(cpu.esp);
    }
	cpu.esp += DATA_BYTE;
	print_asm_template1();
	return 1;
}

#include "cpu/exec/template-end.h"
