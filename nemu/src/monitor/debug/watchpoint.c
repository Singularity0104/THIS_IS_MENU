#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
		wp_pool[i].is_free = true;
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP *new_wp() {
	bool find = false;
	int i;
	for(i = 0; i < 32; i++) {
		if(wp_pool[i].is_free == true) {
			find = true;
			wp_pool[i].is_free = false;
			if(head == NULL) {
				wp_pool[i].NO = 1;
				head = &wp_pool[i];
			}
			else {
				WP *tmp = head;
				while(tmp->next != NULL) {
					tmp = tmp->next;
				}
				tmp->next = &wp_pool[i];
				wp_pool[i].NO = tmp->NO + 1;
			}
			wp_pool[i].next = NULL;
			break;
		}
	}
	Assert(find == true, "Can't find a free wp!");
	return &wp_pool[i];
}

void free_wp(WP *wp) {
	Assert(wp->is_free == false, "WP has already been free!");
	wp->is_free = true;
	WP *tmp = wp->next;
	while(tmp != NULL) {
		tmp->NO--;
	}
	if(head == wp) {
		head = wp->next;
	}
	else {
		tmp = head;
		while(tmp != NULL && tmp->next != wp) {
			tmp = tmp->next;
		}
		Assert(tmp != NULL, "ERROR! Can't find wp!");
		tmp->next = wp->next;
	}
}

bool checkpoint() {
	bool change = false;
	WP *tmp = head;
	while(tmp != NULL) {
		bool success = true;
		int cur = expr(tmp->exp, &success);
		printf("NO %d  exp  %s\n", tmp->NO, tmp->exp);
		Assert(success == true, "ERROR!");
		if(cur != tmp->res) {
			change = false;
			printf("Watchpoint NO %2.2d:\n", tmp->NO);
			printf("Expression: %s\n", tmp->exp);
			printf("Old value: %d\n", tmp->res);
			printf("New value: %d\n", cur);
			tmp->res = cur;
		}
	}
	return change;
}

void delete_NO(int no) {
	WP *tmp = head;
	while(tmp != NULL) {
		if(tmp->NO == no) {
			free_wp(tmp);
			break;
		}
	}
	Assert(tmp != NULL, "Delete filed");
	printf("Successfully delete watchpoint NO %2.2d\n", no);
	return;
}