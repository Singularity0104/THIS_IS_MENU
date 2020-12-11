#include "monitor/monitor.h"
#include "cpu/helper.h"
#include <setjmp.h>
#include "monitor/watchpoint.h"

#define MYOUTPUT 1

#if MYOUTPUT
const char poem[] = "\
\n                            _ooOoo_ \
\n                           o8888888o \
\n                           88\" . \"88 \
\n                           (| -_- |) \
\n                            O\\ = /O \
\n                        ____/`---'\\____ \
\n                      .   ' \\\\| |// `. \
\n                       / \\\\||| 1 |||// \\ \
\n                     / _||||| -9- |||||- \\ \
\n                       | | \\\\\\ 9 /// | | \
\n                     | \\_| ''\\-8-/'' | | \
\n                      \\ .-\\__ `0` ___/-. / \
\n                   ___`. .' /--2--\\ `. . __ \
\n                .\"\" '< `.___\\_<0>_/___.' >'\"\". \
\n               | | : `- \\`.;`\\ 2 /`;.`/ - ` : | | \
\n                 \\ \\ `-. \\_ __\\ /__ _/ .-` / / \
\n         ======`-.____`-.___\\_____/___.-`____.-'====== \
\n                            `=---=' \
\n                                        \
\n                                        \
\n      _    _                    _   _         ____ \
\n     | |  | |                  | \\ | |       |  _ \\ \
\n     | |__| | ___  _ __   ___  |  \\| | ___   | |_) |_   _  __ _ \
\n     |  __  |/ _ \\| '_ \\ / _ \\ | . ` |/ _ \\  |  _ <| | | |/ _` | \
\n     | |  | | (_) | |_) |  __/ | |\\  | (_) | | |_) | |_| | (_| | \
\n     |_|  |_|\\___/| .__/ \\___| |_| \\_|\\___/  |____/ \\__,_|\\__, | \
\n                  | |                                      __/ | \
\n                  |_|                                     |___/ \
\n                                        \
\n                                        \
\n                                        ";
uint32_t poem_length = sizeof(poem) - 1;
#endif

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
#if MYOUTPUT
	uint32_t poem_index = 0;
#endif
	for(; n > 0; n --) {
#ifdef DEBUG
		swaddr_t eip_temp = cpu.eip;
#if MYOUTPUT
		if((n & 0x3ff) == 0) {
			/* Output some dots while executing the program. */
			printf("\033[1;%dm%c\033[0m", 31 + (poem_index % 6), poem[poem_index % poem_length]);
			poem_index++;
		}
#else
		if((n & 0xffff) == 0) {
			/* Output some dots while executing the program. */
			fputc('.', stderr);
		}
#endif
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
