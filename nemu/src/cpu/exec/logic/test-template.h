#include "cpu/exec/template-start.h"

#define instr test

static void do_execute() {
    // if(op_src->val & op_dest->val)
}

make_instr_helper(r2rm)

#include "cpu/exec/template-end.h"
