#include "cpu/exec/template-start.h"

#define instr movcr

static void do_execute() {
	printf("%d\n", op_src->reg);
	print_asm_template2();
}

make_instr_helper(r2rm)
make_instr_helper(rm2r)



#include "cpu/exec/template-end.h"
