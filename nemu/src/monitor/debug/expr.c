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
	memset(tokens, 0, sizeof(tokens));
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
						if(nr_token != 0 && (tokens[nr_token - 1].type == NUM || tokens[nr_token - 1].type == NENUM || tokens[nr_token - 1].type == '(' || tokens[nr_token - 1].type == ')')) {
							tokens[nr_token].type = '-';
							nr_token++;
							tokens[nr_token].type = NUM;
							Assert(substr_len - 1 <= 32, "Token too long!");
							strncpy(tokens[nr_token].str, substr_start + 1, substr_len - 1);
							/*test point*/
							// printf("success!  %s\n", tokens[nr_token].str);
						}
						else {
							tokens[nr_token].type = NENUM;
							Assert(substr_len <= 32, "Token too long!");
							strncpy(tokens[nr_token].str, substr_start, substr_len);
							/*test point*/
							// printf("success!  %s\n", tokens[nr_token].str);
						}
						nr_token++;
						break;
					case NUM:
						tokens[nr_token].type = NUM;
						Assert(substr_len <= 32, "Token too long!");
						strncpy(tokens[nr_token].str, substr_start, substr_len);
						/*test point*/
						// printf("success!  %s\n", tokens[nr_token].str);
						nr_token++;
						break;
					case '+':
						tokens[nr_token].type = '+';
						nr_token++;
						break;
					case '-':
						if(nr_token == 0) {
							tokens[nr_token].type = NUM;
							strncpy(tokens[nr_token].str, "0", 1);
							nr_token++;
						}
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
	if(tokens[p].type == '(') {
		int i;
		for(i = p + 1; i<= q - 1; i++) {
			if(tokens[i].type == '(') {
				top++;
			}
			else if(tokens[i].type == ')') {
				top--;
			}
		}
		if(tokens[q].type == ')' && top == 0) {
			return true;
		}
	}
	return false;
}

int eval(int p, int q, bool *success) {
	if(*success == false) {
		return 0;
	}
	
	/*test point*/
	// printf("tokens\n");
	// int i;
	// for(i = p; i <= q; i++) {
	// 	printf("%c %s", tokens[i].type, tokens[i].str);
	// }
	// printf("\n");

	if(p > q) {
		*success = false;
		printf("ERROR_1!\n");
		return 0;
	}
	else if(p == q) {
		if(!(tokens[p].type == NUM || tokens[p].type == NENUM)) {
			*success = false;
			printf("ERROR_2!\n");
			return 0;
		}
		int i;
		int e = 1;
		int sum = 0;
		for(i = 31; i >= 0; i--) {
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
	else if (check_parentheses(p, q) == true) {
		return eval(p + 1, q - 1, success);
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
					*success = false;
					printf("ERROR_3!\n");
					return 0;
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
					top++;
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
					top++;
				}
			}

			/*test point*/
			// int j;
			// for(j = 0; j < 32; j++){
			// 	printf("%d ", stack_i[j]);
			// }
			// printf("\n");
			// for(j = 0; j < 32; j++){
			// 	printf("%c ", stack[j]);
			// }
			// printf("----top%d --i%d\n", top, i);

		}
		
		/*test point*/
		// printf("top at %d\n", top);
		
		if(!(top == 1 && stack[0] != '(' && stack[0] != ')')) {
			*success = false;
			printf("ERROR_4!\n");
			return 0;
		}
		int op = stack_i[0];
		int op_type = stack[0];

		/*test point*/
		// printf("op\n%d%c %d\n", op_type, op_type, op);

		int val_1 = eval(p, op - 1, success);
		int val_2 = eval(op + 1, q, success);
		switch(op_type) {
			case '+': return val_1 + val_2;break;
			case '-': return val_1 - val_2;break;
			case '*': return val_1 * val_2;break;
			case '/': return val_1 / val_2;break;
			default: 
				*success = false;
				printf("ERROR_5!\n");
				return 0;
		}
	}
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	/* TODO: Insert codes to evaluate the expression. */
	int res = eval(0, nr_token - 1, success);
	return res;
	//panic("please implement me");
	//return 0;
}

//p ((((((4+5)*7)-8)*6)-76+4)-99+-9)