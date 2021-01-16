#include "cpu/exec/template-start.h"

#define instr movcr

make_helper(movcr_r2cr_l) {
    uint8_t modrm = instr_fetch(eip + 1, 1);
    uint8_t cr = ((modrm >> 3) & 0x7);
    uint8_t reg = modrm & 0x7;
    uint32_t src = REG(reg);
    if(cr == 0) {
        cpu.cr0.val = src;
    }
    else if(cr == 3) {
        cpu.cr3.val = src;
    }
    print_asm(str(instr) str(SUFFIX) " %s,%s%d", REG_NAME(reg), "cr", cr);
    return 2;
}
make_helper(movcr_cr2r_l) {
    uint8_t modrm = instr_fetch(eip + 1, 1);
    uint8_t cr = ((modrm >> 3) & 0x7);
    uint8_t reg = modrm & 0x7;
    if(cr == 0) {
        REG(reg) = cpu.cr0.val;
    }
    else if(cr == 0) {
        REG(reg) = cpu.cr3.val;
    }
    print_asm(str(instr) str(SUFFIX) " %s%d,%s", "cr", cr, REG_NAME(reg));
    return 2;
}

#include "cpu/exec/template-end.h"
