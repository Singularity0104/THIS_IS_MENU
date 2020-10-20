#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
	print_asm_template2();
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"