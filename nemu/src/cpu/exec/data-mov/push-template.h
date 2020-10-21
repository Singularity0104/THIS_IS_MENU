#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
    print_asm_template2();
}

make_instr_helper(r);

#include "cpu/exec/template-end.h"