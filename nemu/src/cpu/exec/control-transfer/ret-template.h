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
	return 0;
}

make_helper(concat(ret_i_, SUFFIX)) {
    uint32_t len = decode_i_w(eip + 1);
    if(DATA_BYTE == 2) {
        cpu.eip =  MEM_R(cpu.esp);
        cpu.eip = cpu.eip & 0x0000ffff;
    }
    else if(DATA_BYTE == 4) {
        cpu.eip =  MEM_R(cpu.esp);
    }
    cpu.eip = cpu.eip + op_src->val;
	cpu.esp = cpu.esp + DATA_BYTE;
	print_asm_template1();
	return len;
}

#include "cpu/exec/template-end.h"
