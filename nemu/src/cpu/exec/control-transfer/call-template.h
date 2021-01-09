#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
    cpu.esp = cpu.esp - DATA_BYTE;
    if(DATA_BYTE == 2) {
        uint16_t ip = (uint16_t)((cpu.eip + 1 + DATA_BYTE) & 0x0000ffff);
        MEM_W(cpu.esp, ip, R_SS);
        cpu.eip = (cpu.eip + op_src->val) & 0x0000ffff;
    }
    else if(DATA_BYTE == 4) {
        MEM_W(cpu.esp, (cpu.eip + 1 + DATA_BYTE), R_SS);
        cpu.eip = cpu.eip + op_src->val;
    }
	print_asm_template1();
}

make_instr_helper(i)

make_helper(concat(call_rm_, SUFFIX)) {
    int len = concat(decode_rm_, SUFFIX)(eip + 1);
    cpu.esp = cpu.esp - DATA_BYTE;
    if(DATA_BYTE == 2) {
        uint16_t ip = (uint16_t)((cpu.eip + 1 + len) & 0x0000ffff);
        MEM_W(cpu.esp, ip, R_SS);
        cpu.eip = op_src->val & 0x0000ffff;
    }
    else if(DATA_BYTE == 4) {
        MEM_W(cpu.esp, (cpu.eip + 1 + len), R_SS);
        cpu.eip = op_src->val;
    }
	print_asm_template1();
    return 0;
}

#include "cpu/exec/template-end.h"
