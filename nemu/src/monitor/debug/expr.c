#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ, NENUM, NUM

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */
	{" +",	NOTYPE},				// spaces
	{"-[0-9][0-9]*", NENUM},
	{"[0-9][0-9]*", NUM},
	{"\\+", '+'},					// plus
	{"-", '-'},
	{"\\*", '*'},
	{"/", '/'},
	{"==", EQ},
	{"\\(", '('},
	{"\\)", ')'},						// equal
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */
				Assert(nr_token < 32, "Expression too long!");
				switch(rules[i].token_type) {
					case NOTYPE:
						break;
					case NENUM:
						if(nr_token == 0 || (tokens[nr_token - 1].type != NUM && tokens[nr_token - 1].type != NENUM)) {
							tokens[nr_token].type = NENUM;
							Assert(substr_len <= 32, "Token too long!");
							strncpy(tokens[nr_token].str, substr_start, substr_len);
							printf("success!  %s\n", tokens[nr_token].str);
						}
						else {
							tokens[nr_token].type = '-';
							nr_token++;
							tokens[nr_token].type = NUM;
							Assert(substr_len - 1 <= 32, "Token too long!");
							strncpy(tokens[nr_token].str, substr_start + 1, substr_len - 1);
							printf("success!  %s\n", tokens[nr_token].str);
						}
						nr_token++;
						break;
					case NUM:
						tokens[nr_token].type = NUM;
						Assert(substr_len <= 32, "Token too long!");
						strncpy(tokens[nr_token].str, substr_start, substr_len);
						printf("success!  %s\n", tokens[nr_token].str);
						nr_token++;
						break;
					case '+':
						tokens[nr_token].type = '+';
						nr_token++;
						break;
					case '-':
						tokens[nr_token].type = '-';
						nr_token++;
						break;
					case '*':
						tokens[nr_token].type = '*';
						nr_token++;
						break;
					case '/':
						tokens[nr_token].type = '/';
						nr_token++;
						break;
					case EQ:
						tokens[nr_token].type = EQ;
						nr_token++;
						break;
					case '(':
						tokens[nr_token].type = '(';
						nr_token++;
						break;
					case ')':
						tokens[nr_token].type = ')';
						nr_token++;
						break;
					default: panic("please implement me");
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

bool check_parentheses(int p, int q){
	int top = 0;
	if(tokens[p].type != '(') {
		return false;
	}
	int i;
	for(i = p + 1; i<= q - 1; i++) {
		if(tokens[i].type == '(') {
			top++;
		}
		if(tokens[i].type == ')') {
			top--;
		}
	}
	if(tokens[p].type == ')' && top == 0) {
		return true;
	}
	return false;
}

int eval(int p, int q) {
	printf("tokens\n");
	int i;
	for(i = p; i <= q; i++) {
		printf("%c %s", tokens[i].type, tokens[i].str);
	}
	printf("\n");

	if(p > q) {
		Assert(0, "ERROR_1!");	
	}
	else if(p == q) {
		Assert(tokens[p].type == NUM || tokens[p].type == NENUM, "Invalid expression!");
		int i;
		int e = 1;
		int sum = 0;
		for(i = 31; i >= 0; i--) {
			printf("woshishabi\n");
			char tmp = tokens[p].str[i];
			if(tmp == '\0') {
				continue;
			}
			else if(tmp >= '0' && tmp <= '9') {
				sum += e * (int)(tmp - '0');
				e *= 10;
			}
			else if(i == 0 && tmp == '-') {
				sum = -sum;
			}
		}
		return sum;
	}
	else if (check_parentheses(p, q) == true){
		return eval(p + 1, q - 1);
	}
	else {
		int stack[32] = {};
		int stack_i[32] = {};
		int top = 0;
		int i;
		for(i = p; i <= q; i++) {
			int tmp = tokens[i].type;
			if((top == 0 && tmp != NENUM && tmp != NUM) || tmp == '(') {
				stack[top] = tmp;
				stack_i[top] = i;
				top++;
			}
			else if(tmp == ')') {
				while(top > 0){
					top--;
					if(stack[top] == '(') {
						break;
					}
				}
				if(stack[top] != '(') {
					Assert(0, "ERROR_2!");
				}
			}
			else if(tmp == '+' || tmp == '-') {
				if(stack[top - 1] == '(') {
					stack[top] = tmp;
					stack_i[top] = i;
					top++;
				}
				else {
					top--;
					stack[top] = tmp;
					stack_i[top] = i;
				}
			}
			else if(tmp == '*' || tmp == '/') {
				if(stack[top - 1] == '(') {
					stack[top] = tmp;
					stack_i[top] = i;
					top++;
				}
				else if(stack[top - 1] == '*' || stack[top - 1] == '/') {
					top--;
					stack[top] = tmp;
					stack_i[top] = i;
				}
			}

			int i;
			for(i = 0; i < 32; i++){
				printf("%d ", stack_i[i]);
			}
			printf("\n");
			for(i = 0; i < 32; i++){
				printf("%c ", stack[i]);
			}
			printf("----top%d\n", top);

		}
		printf("top at %d\n", top);
		Assert(top == 1, "ERROR_3!");
		Assert(stack[0] != '(', "ERROR_4!");
		Assert(stack[0] != ')', "ERROR_5!");
		// Assert(top == 1 && stack[0] != '(' && stack[0] != ')', "ERROR_3!");
		int op = stack_i[0];
		int op_type = stack[0];

		printf("op\n%d%c %d\n", op_type, op_type, op);

		int val_1 = eval(p, op - 1);
		int val_2 = eval(op + 1, q);
		switch(op_type) {
			case '+': return val_1 + val_2;break;
			case '-': return val_1 - val_2;break;
			case '*': return val_1 * val_2;break;
			case '/': return val_1 / val_2;break;
			default: Assert(0, "ERROR_4!");
		}
	}
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	/* TODO: Insert codes to evaluate the expression. */
	int res = eval(0, nr_token - 1);
	return res;
	//panic("please implement me");
	//return 0;
}