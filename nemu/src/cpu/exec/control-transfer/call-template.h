#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
	
}

make_instr_helper(i)

#include "cpu/exec/template-end.h"