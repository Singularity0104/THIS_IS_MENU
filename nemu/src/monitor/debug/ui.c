#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);

/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_help(char *args);

static int cmd_s(char *args) {
	uint32_t step = 1;
	if(args != NULL){
		step = 0;
		int size_tmp = strlen(args);
		int i, e;
		for(i = 0, e = 1; i < size_tmp; i++, e *= 10) {
			Assert(args[i] >= '0' && args[i] <= '9', "The step must be numbers!");
			step += e * (uint32_t)(args[i] - '0');
		}
	}
	cpu_exec(step);
	return 0;
}

static int cmd_i(char *args) {
	Assert(strlen(args) == 1 && (args[0] == 'r' || args[0] == 'w'), "Please input correct commends!");
	if(args[0] == 'r') {
		printf("%-8s0x%-16x%-16u\n", "eax", cpu.eax, cpu.eax);
		printf("%-8s0x%-16x%-16u\n", "ecx", cpu.ecx, cpu.ecx);
		printf("%-8s0x%-16x%-16u\n", "edx", cpu.edx, cpu.edx);
		printf("%-8s0x%-16x%-16u\n", "ebx", cpu.ebx, cpu.ebx);
		printf("%-8s0x%-16x%-16u\n", "esp", cpu.esp, cpu.esp);
		printf("%-8s0x%-16x%-16u\n", "ebp", cpu.ebp, cpu.ebp);
		printf("%-8s0x%-16x%-16u\n", "esi", cpu.esi, cpu.esi);
		printf("%-8s0x%-16x%-16u\n", "edi", cpu.edi, cpu.edi);
	}
	return 0;
}

static int cmd_p(char *args) {
	bool success = true;
	int res = expr(args, &success);
	if(success){
		printf("ans = %d\n", res);
	}
	else {
		printf("Invalid expression!\n");
	}
	return 0;
}

static int cmd_x(char *args) {
	int index = 0;
	int i;
	bool first_num = false;
	for(i = 0; i < strlen(args); i++) {
		if(first_num == true && args[i] == ' ') {
			index = i;
			break;
		}
		if(args[i] >= '0' && args[i] <= '9') {
			first_num = true;
		}
	}
	int e = 1;
	int sum = 0;
	for(i = index; i >= 0; i--) {
		if(args[i] == ' ') {
			continue;
		}
		else {
			sum += e * (int)(args[i] - '0');
			e *= 10;
		}
	}
	bool success = true;
	uint32_t add = expr(args + index, &success);
	Assert(success == true, "ERROR!");
	for(i = 0; i < sum; i++) {
		uint32_t mem = swaddr_read(add, 4);
		printf("0x%-12x    0x%-12.8x\n", add, mem);
		add += 4;
	}
	return 0;
}

static int cmd_w(char *args) {
	WP *new;
	new = new_wp();
	strncpy(new->exp, args, 128);
	bool success = true;
	new->res = expr(args, &success);
	Assert(success == true, "ERROR!");
	printf("Set watchpoint NO %2.2d\n", new->NO);
	printf("Expression: %s\n", new->exp);
	return 0;
}

static int cmd_d(char *args) {
	int i;
	for(i = 0; i < strlen(args); i++) {
		if(args[i] != ' ' && (args[i] < '0' || args[i] > '9')) {
			printf("Invalid commend!\n");
			return 0;
		}
	}
	bool success = true;
	int no = expr(args, &success);
	Assert(success == true, "ERROR!");
	delete_NO(no);
	return 0;
}

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{ "si", "Execute the program step by step", cmd_s},
	{ "info", "Print the register or the watch point", cmd_i},
	{ "p", "Evalute expression", cmd_p},
	{ "x", "Print memory", cmd_x},
	{ "w", "Set a watchpoint", cmd_w},
	{ "d", "Delete a watchpoint", cmd_d}

	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
