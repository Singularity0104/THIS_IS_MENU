#include "cpu/exec/template-start.h"

#define instr movcr

make_helper(movcr_r2rm_l) {
    // int len = decode_r2rm_l(eip);
    // if(op_dest->addr == 0) {
    //     cpu.cr0.val = op_src->val;
    // }
	// print_asm_template2();
    // return len;
    return 0;
}
make_helper(movcr_rm2r_l) {
    // int len = decode_rm2r_l(eip);
    // if(op_src->addr == 0) {
    //     REG(op_dest->reg) = cpu.cr0.val;
    // }
	// print_asm_template2();
    // return len;
    return 0;
}

#include "cpu/exec/template-end.h"
