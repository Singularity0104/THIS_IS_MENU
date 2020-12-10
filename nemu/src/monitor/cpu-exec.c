#include "monitor/monitor.h"
#include "cpu/helper.h"
#include <setjmp.h>
#include "monitor/watchpoint.h"

const char poem[] = "I really wanna stop\
But I just gotta taste for it\
I feel like I could fly with the ball on the moon\
So honey hold my hand you like making me wait for it\
I feel I could die walking up to the room oh yeah\
Late night watching television\
But how we get in this position\
It's way too soon I know this isn't love\
But I need to tell you something\
I really really really really really really like you\
And I want you do you want me do you want me too\
I really really really really really really like you\
And I want you do you want me do you want me too\
Oh did I say too much\
I'm so in my head\
When we're out of touch\
I really really really really really really like you\
And I want you do you want me do you want me too\
It's like everything you say is a sweet revelation\
All I wanna do is get into your head\
Yeah we could stay alone you and me and this temptation\
Sipping on your lips hanging on by thread baby\
Late night watching television\
But how we get in this position\
It's way too soon I know this isn't love\
But I need to tell you something\
I really really really really really really like you\
And I want you do you want me do you want me too\
I really really really really really really like you\
And I want you do you want me do you want me too\
Oh did I say too much\
I'm so in my head\
When we're out of touch\
I really really really really really really like you\
And I want you do you want me do you want me too\
Who gave you eyes like that\
Said you could keep them\
I don't know how to act\
The way I should be leaving\
I'm running out of time\
Going out of my mind\
I need to tell you something\
Yeah I need to tell you something\
Yeah I really really really really really really like you\
And I want you do you want me do you want me too\
I really really really really really really like you\
And I want you do you want me do you want me too\
Oh did I say too much\
I'm so in my head\
When we're out of touch\
I really really really really really really like you\
And I want you do you want me do you want me too\
I really really really really really really like you\
And I want you do you want me do you want me too\
I really really really really really really like you\
And I want you do you want me do you want me too";
// const int poem_length = strlen(poem);


/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INSTR_TO_PRINT 10

int nemu_state = STOP;

int exec(swaddr_t);

char assembly[80];
char asm_buf[128];

/* Used with exception handling. */
jmp_buf jbuf;

void print_bin_instr(swaddr_t eip, int len) {
	int i;
	int l = sprintf(asm_buf, "%8x:   ", eip);
	for(i = 0; i < len; i ++) {
		l += sprintf(asm_buf + l, "%02x ", instr_fetch(eip + i, 1));
	}
	sprintf(asm_buf + l, "%*.s", 50 - (12 + 3 * len), "");
}

/* This function will be called when an `int3' instruction is being executed. */
void do_int3() {
	printf("\nHit breakpoint at eip = 0x%08x\n", cpu.eip);
	nemu_state = STOP;
}

/* Simulate how the CPU works. */
void cpu_exec(volatile uint32_t n) {
	if(nemu_state == END) {
		printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
		return;
	}
	nemu_state = RUNNING;

#ifdef DEBUG
	volatile uint32_t n_temp = n;
#endif

	setjmp(jbuf);

	for(; n > 0; n --) {
#ifdef DEBUG
		swaddr_t eip_temp = cpu.eip;
		if((n & 0xffff) == 0) {
			/* Output some dots while executing the program. */
			// fputc('.', stderr);
			fputs("[]", stderr);

		}
#endif

		/* Execute one instruction, including instruction fetch,
		 * instruction decode, and the actual execution. */
		int instr_len = exec(cpu.eip);

		cpu.eip += instr_len;

#ifdef DEBUG
		print_bin_instr(eip_temp, instr_len);
		strcat(asm_buf, assembly);
		Log_write("%s\n", asm_buf);
		if(n_temp < MAX_INSTR_TO_PRINT) {
			printf("%s\n", asm_buf);
		}
#endif

		/* TODO: check watchpoints here. */


#ifdef HAS_DEVICE
		extern void device_update();
		device_update();
#endif

		if(checkpoint() == true) {
			nemu_state = STOP;
		}

		if(nemu_state != RUNNING) { return; }
	}

	if(nemu_state == RUNNING) { nemu_state = STOP; }
}
