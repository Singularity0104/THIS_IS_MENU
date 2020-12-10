#include "monitor/monitor.h"
#include "cpu/helper.h"
#include <setjmp.h>
#include "monitor/watchpoint.h"

const char poem[] = "I really wanna stop\
\nBut I just gotta taste for it\
\nI feel like I could fly with the ball on the moon\
\nSo honey hold my hand you like making me wait for it\
\nI feel I could die walking up to the room oh yeah\
\nLate night watching television\
\nBut how we get in this position\
\nIt's way too soon I know this isn't love\
\nBut I need to tell you something\
\nI really really really really really really like you\
\nAnd I want you do you want me do you want me too\
\nI really really really really really really like you\
\nAnd I want you do you want me do you want me too\
\nOh did I say too much\
\nI'm so in my head\
\nWhen we're out of touch\
\nI really really really really really really like you\
\nAnd I want you do you want me do you want me too\
\nIt's like everything you say is a sweet revelation\
\nAll I wanna do is get into your head\
\nYeah we could stay alone you and me and this temptation\
\nSipping on your lips hanging on by thread baby\
\nLate night watching television\
\nBut how we get in this position\
\nIt's way too soon I know this isn't love\
\nBut I need to tell you something\
\nI really really really really really really like you\
\nAnd I want you do you want me do you want me too\
\nI really really really really really really like you\
\nAnd I want you do you want me do you want me too\
\nOh did I say too much\
\nI'm so in my head\
\nWhen we're out of touch\
\nI really really really really really really like you\
\nAnd I want you do you want me do you want me too\
\nWho gave you eyes like that\
\nSaid you could keep them\
\nI don't know how to act\
\nThe way I should be leaving\
\nI'm running out of time\
\nGoing out of my mind\
\nI need to tell you something\
\nYeah I need to tell you something\
\nYeah I really really really really really really like you\
\nAnd I want you do you want me do you want me too\
\nI really really really really really really like you\
\nAnd I want you do you want me do you want me too\
\nOh did I say too much\
\nI'm so in my head\
\nWhen we're out of touch\
\nI really really really really really really like you\
\nAnd I want you do you want me do you want me too\
\nI really really really really really really like you\
\nAnd I want you do you want me do you want me too\
\nI really really really really really really like you\
\nAnd I want you do you want me do you want me too\n\n";
uint32_t poem_length = sizeof(poem) - 1;


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

	uint32_t poem_index = 0;

	for(; n > 0; n --) {
#ifdef DEBUG
		swaddr_t eip_temp = cpu.eip;
		// if((n & 0xffff) == 0) {
		// 	/* Output some dots while executing the program. */
		// 	fputc('.', stderr);
		// }
		if((n & 0x3fff) == 0) {
			/* Output some dots while executing the program. */
			printf("\033[1;35m%c\033[0m", poem[poem_index % poem_length]);
			poem_index++;
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
