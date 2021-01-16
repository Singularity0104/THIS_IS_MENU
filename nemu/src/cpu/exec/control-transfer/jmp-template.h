#include "cpu/exec/template-start.h"

#define instr jmp

static void do_execute() {
	DATA_TYPE_S offset = op_src->val;
	cpu.eip = cpu.eip + offset;
	if(DATA_BYTE == 2) {
		cpu.eip = cpu.eip & 0x0000ffff;
	}
	print_asm_template1();
}

make_instr_helper(i)
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_helper(concat(jmp_rm_, SUFFIX)) {
	concat(decode_rm_, SUFFIX)(eip + 1);
	swaddr_t addr = op_src->val;
	cpu.eip = addr;
	if(DATA_BYTE == 2) {
		cpu.eip = cpu.eip & 0x0000ffff;
	}
	print_asm_template1();
	return 0;
}

make_helper(concat(jmp_ptr_, SUFFIX)) {
	DATA_TYPE_S eip_src = instr_fetch(eip + 1, DATA_BYTE);
	uint16_t cs_src = instr_fetch(eip + 1 + DATA_BYTE, 2);
	cpu.eip = eip_src;
	cpu.CS = cs_src;
	uint32_t index = cpu.CS >> 3;
	uint64_t gdt_part_1 = lnaddr_read(cpu.gdtr.base + 8 * index, 4);
	uint64_t gdt_part_2 = lnaddr_read(cpu.gdtr.base + 8 * index + 4, 4);
	uint64_t gdt = gdt_part_1 + (gdt_part_2 << 32);
	lnaddr_t base = (lnaddr_t)(((gdt >> 16) & 0xffffff) + ((gdt >> 32) & 0xff000000));
	lnaddr_t limit = (lnaddr_t)((gdt & 0xffff) + ((gdt >> 32) & 0xf0000));
	cpu.SRcache[R_CS] = (uint64_t)(base) + (((uint64_t)(limit)) << 32);
	if(DATA_BYTE == 2) {
		cpu.eip = cpu.eip & 0xffff;
	}
	print_asm("jmpptr" str(SUFFIX) " 0x%x 0x%x", eip_src, cs_src);
	return 0;
}
#endif

#include "cpu/exec/template-end.h"
