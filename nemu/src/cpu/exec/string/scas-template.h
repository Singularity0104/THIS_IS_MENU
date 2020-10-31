#include "cpu/exec/template-start.h"

#define instr scas

make_helper(concat(scas_, SUFFIX)) {
    // DATA_TYPE_S des = REG(R_EAX);
    // DATA_TYPE_S src = MEM_R(cpu.edi);
    // DATA_TYPE_S res = des - src;
    // if((uint32_t)des < (uint32_t)src) {
    //     cpu.CF = 1;
    // }
    // else {
    //     cpu.CF = 0;
    // }
    // if(((des >> (DATA_BYTE * 8 - 1)) ^ (src >> (8 * DATA_BYTE - 1))) & (~((res >> (DATA_BYTE * 8 - 1)) ^ (src >> (8 * DATA_BYTE - 1)))) & 1) {
    //     cpu.OF = 1;
    // }
    // else {
    //     cpu.OF = 0;
    // }
    // if(res == 0) {
    //     cpu.ZF = 1;
    // }
    // else {
    //     cpu.ZF = 0;
    // }
    // cpu.SF = (res & (1 << (DATA_BYTE * 8 - 1))) >> (DATA_BYTE * 8 - 1);
    // uint32_t tmp = res & 1;
    // int i;
    // for(i = 1; i < 8; i++) {
    //     tmp = tmp ^ (res & (1 << i) >> i);
    // }
    // cpu.PF = !tmp;
    // print_asm(str(instr) str(SUFFIX));
    return 0;
}

#include "cpu/exec/template-end.h"
