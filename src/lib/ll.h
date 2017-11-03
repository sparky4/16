#ifndef __LL_H__
#define __LL_H__
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "src/lib/16_tail.h"

//#define OTHERMERGELISTSTIFF

/* Link list node_t */
typedef struct node
{
	struct node	*prev;
	rgb_t	d;
	int data;
	struct node	*next;
	word	id;
} node_t;

#ifdef OTHERMERGELISTSTIFF
int listLength(node_t * item);
void print_list(node_t * head);
void pushe(node_t * head, int val);
void pushs(node_t ** head, int val);
int pop(node_t ** head);
int remove_last(node_t * head);
int remove_by_index(node_t ** head, int n);
#else
/* pull off the front node of the source and put it in dest */
void Movenode(struct node** destRef, struct node** sourceRef);
struct node* SortedMerge(struct node* a, struct node* b);
struct node* LL_merge(struct node* a, struct node* b);
void pushll(struct node** head_ref, int new_data);
void printList(struct node *node);
void removeDuplicates(struct node* head);
#endif

#endif
