#include "cpu/exec/template-start.h"

#define instr movcr

make_helper(movcr_r2cr_l) {
    // int len = decode_rm2r_l(eip + 1);
    // if(op_dest->reg == 0) {
    //     cpu.cr0.val = op_src->val;

    // }
	// print_asm_template2();
    // return 2;
    uint8_t modrm = instr_fetch(eip + 1, 1);
    uint8_t cr = ((modrm >> 3) & 0x7);
    uint8_t reg = modrm & 0x7;
    uint32_t src = REG(reg);
    if(cr == 0) {
        cpu.cr0.val = src;
    }
    print_asm(str(instr) str(SUFFIX) " %s%d,%s", "cr", cr, REG_NAME(reg));
    return 2;
}
make_helper(movcr_cr2r_l) {
    // int len = decode_r2rm_l(eip + 1);
    // if(op_src->reg == 0) {
    //     REG(op_dest->reg) = cpu.cr0.val;
    // }
	// print_asm_template2();
    // return len;
    return 2;
}

#include "cpu/exec/template-end.h"
