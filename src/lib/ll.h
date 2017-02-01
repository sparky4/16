#ifndef __LL_H__
#define __LL_H__
#include <stdio.h>
#include "src/lib/16_tail.h"

typedef struct node
{
	struct node	*prev;
	rgb_t	d;
	int val;
	struct node	*next;
} node_t;

void print_list(node_t * head);
void pushe(node_t * head, int val);
void pushs(node_t ** head, int val);
int pop(node_t ** head);
int remove_last(node_t * head);
int remove_by_index(node_t ** head, int n);

#endif
