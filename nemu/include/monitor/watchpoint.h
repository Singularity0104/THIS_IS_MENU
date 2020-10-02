#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */
	bool is_free;
	uint32_t res;
	int hit;
	char exp[128];

} WP;

WP *new_wp();
void free_wp(WP *wp);
bool checkpoint();
void delete_NO(int no);
void show_wp();

#endif
