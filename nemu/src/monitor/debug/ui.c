#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include "common.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <elf.h>
#include "../../memory/cache.h"

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
		int i;
		for(i = 0; i < strlen(args); i++) {
			if(args[i] != ' ' && (args[i] < '0' || args[i] > '9')) {
				printf("Invalid commend!\n");
				return 0;
			}
		}
		bool success = true;
		step = expr(args, &success);
		Assert(success == true, "ERROR!");
	}
	cpu_exec(step);
	return 0;
}

static int cmd_i(char *args) {
	if(!(args != NULL && strlen(args) == 1 && (args[0] == 'r' || args[0] == 'w'))) {
		printf("Invalid commend!\n");
		return 0;
	}
	if(args[0] == 'r') {
		printf("%-8s0x%-16x%-16u\n", "eax", cpu.eax, cpu.eax);
		printf("%-8s0x%-16x%-16u\n", "ecx", cpu.ecx, cpu.ecx);
		printf("%-8s0x%-16x%-16u\n", "edx", cpu.edx, cpu.edx);
		printf("%-8s0x%-16x%-16u\n", "ebx", cpu.ebx, cpu.ebx);
		printf("%-8s0x%-16x%-16u\n", "esp", cpu.esp, cpu.esp);
		printf("%-8s0x%-16x%-16u\n", "ebp", cpu.ebp, cpu.ebp);
		printf("%-8s0x%-16x%-16u\n", "esi", cpu.esi, cpu.esi);
		printf("%-8s0x%-16x%-16u\n", "edi", cpu.edi, cpu.edi);
		printf("%-8s0x%-16x%-16u\n", "eip", cpu.eip, cpu.eip);
	}
	else if(args[0] == 'w') {
		show_wp();
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
		uint32_t mem = swaddr_read(add, 4, R_DS);
		printf("0x%-12x  Bytes: %-2.2x %-2.2x %-2.2x %-2.2x  0x%-12.8x\n", add, (mem & 0xff), ((mem >> 8) & 0xff), ((mem >> 16) & 0xff), ((mem >> 24) & 0xff), mem);
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

static int cmd_b(char *args) {
	if(cpu.eip <= 0x100000) {
		printf("No Stack\n");
		return 0;
	}
	uint32_t cnt = 0;
	uint32_t cur_eip = cpu.eip;
	uint32_t cur_ebp = cpu.ebp;
	uint32_t cur_ret = 0;
	char *strtab = GETstrtab();
	Elf32_Sym *symtab = GETsymtab();
	int nr_symtab_entry = GETnr_symtab_entry();
	uint32_t offset = 0;
	uint32_t cur_arg = 0;
	int i;
	for(i = 0; i < nr_symtab_entry; i++) {
		if((symtab[i].st_info & 0xf) == STT_FUNC) {
			if(cur_eip >= symtab[i].st_value && cur_eip <= symtab[i].st_value + symtab[i].st_size) {
				offset = symtab[i].st_name;
				printf("%02d 0x%08x~0x%08x %s\n", cnt, symtab[i].st_value, symtab[i].st_value + symtab[i].st_size, strtab + symtab[i].st_name);
				cur_arg = swaddr_read(cur_ebp + 8, 4, R_SS);
				printf("	arg_1 0x%08x %d\n", cur_arg, cur_arg);
				cur_arg = swaddr_read(cur_ebp + 12, 4, R_SS);
				printf("	arg_2 0x%08x %d\n", cur_arg, cur_arg);
				cur_arg = swaddr_read(cur_ebp + 16, 4, R_SS);
				printf("	arg_3 0x%08x %d\n", cur_arg, cur_arg);
				cur_arg = swaddr_read(cur_ebp + 20, 4, R_SS);
				printf("	arg_4 0x%08x %d\n", cur_arg, cur_arg);
			}
		}
	}
	cnt++;
	while(cur_ebp != 0) {
		cur_ret = swaddr_read(cur_ebp + 4, 4, R_SS);
		int i;
		for(i = 0; i < nr_symtab_entry; i++) {
			if((symtab[i].st_info & 0xf) == STT_FUNC) {
				if(cur_ret >= symtab[i].st_value && cur_ret <= symtab[i].st_value + symtab[i].st_size) {
					offset = symtab[i].st_name;
					printf("%02d 0x%08x~0x%08x %s\n", cnt, symtab[i].st_value, symtab[i].st_value + symtab[i].st_size, strtab + symtab[i].st_name);
					cur_arg = swaddr_read(cur_ebp + 8, 4, R_SS);
					printf("	arg_1 0x%08x %d\n", cur_arg, cur_arg);
					cur_arg = swaddr_read(cur_ebp + 12, 4, R_SS);
					printf("	arg_2 0x%08x %d\n", cur_arg, cur_arg);
					cur_arg = swaddr_read(cur_ebp + 16, 4, R_SS);
					printf("	arg_3 0x%08x %d\n", cur_arg, cur_arg);
					cur_arg = swaddr_read(cur_ebp + 20, 4, R_SS);
					printf("	arg_4 0x%08x %d\n", cur_arg, cur_arg);
				}
			}
		}
		cur_ebp = swaddr_read(cur_ebp, 4, R_SS);
		cnt++;
	}
	return 0;
}

static int cmd_t(char *args) {
	printf("memory read: %lu\n", memread);
	printf("cache1 hit: %lu\n", cache_1_hit);
	printf("cache1 miss: %lu\n", cache_1_miss);
	double cache_1_rate = (double)cache_1_hit / (double)(cache_1_hit + cache_1_miss);
	printf("cache1 rate: %.4f\n", cache_1_rate);
	printf("cache2 hit: %lu\n", cache_2_hit);
	printf("cache2 miss: %lu\n", cache_2_miss);
	double cache_2_rate = (double)cache_2_hit / (double)(cache_2_hit + cache_2_miss);
	printf("cache2 rate: %.4f\n", cache_2_rate);
	printf("total time: %lu\n", memtime);
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
	{ "d", "Delete a watchpoint", cmd_d},
	{ "bt", "Print function", cmd_b},
	{ "time", "Print memory/cache time", cmd_t}

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
