#include "src/lib/ll.h"

#ifdef OTHERMERGELISTSTIFF
int listLength(node_t * item)
{
  node_t * cur = item;
  int size = 0;

  while (cur->next != NULL)
  {
	 ++size;
	 cur = cur->next;
  }

  return size;
}

void print_list(node_t * head)
{
	node_t * current = head;

	while (current->next != NULL)
	{
		printf("[%u]=	%d\n", current->id, current->val);
		current = current->next;
	}
}

void pushe(node_t * head, int val)
{
	node_t * current = head;
	current->id = head->id;
	current->next->id = current->id+1;

	while (current->next != NULL)
	{
		current->next->id = current->id;
		current = current->next;
		current->id++;
	}

	// now we can add a new variable
	current->next = malloc(sizeof(node_t));
	current->next->val = val;
	current->next->next = NULL;
	current->next->id++;
}

void pushs(node_t ** head, int val)
{
	node_t * new_node;
	new_node = malloc(sizeof(node_t));

	new_node->val = val;
	new_node->next = *head;
	*head = new_node;
}

int pop(node_t ** head)
{
	int retval = -1;
	node_t * next_node = NULL;

	if (*head == NULL) {
		return -1;
	}

	next_node = (*head)->next;
	retval = (*head)->val;
	free(*head);
	*head = next_node;

	return retval;
}

int remove_last(node_t * head)
{
	int retval = 0;
	node_t * current;

	/* if there is only one item in the list, remove it */
	if (head->next == NULL) {
		retval = head->val;
		free(head);
		return retval;
	}

	/* get to the last node in the list */
	current = head;
	while (current->next->next != NULL) {
		current = current->next;
	}

	/* now current points to the last item of the list, so let's remove current->next */
	retval = current->next->val;
	free(current->next);
	current->next = NULL;
	return retval;

}

int remove_by_index(node_t ** head, int n)
{
	int i = 0;
	int retval = -1;
	node_t * current = *head;
	node_t * temp_node = NULL;

	if (n == 0) {
		return pop(head);
	}

	for (i = 0; i < n-1; i++) {
		if (current->next == NULL) {
			return -1;
		}
		current = current->next;
	}

	temp_node = current->next;
	retval = temp_node->val;
	current->next = temp_node->next;
	free(temp_node);

	return retval;
}
#else
/* Takes two lists sorted in increasing order, and splices
	their nodes together to make one big sorted list which
	is returned.	*/
struct node* SortedMerge(struct node* a, struct node* b)
{
	/* a dummy first node to hang the result on */
	struct node dummy;

	/* tail points to the last result node  */
	struct node* tail = &dummy;

	/* so tail->next is the place to add new nodes
	  to the result. */
	dummy.next = NULL;
	while (1)
	{
		if (a == NULL)
		{
			/* if either list runs out, use the
				other list */
			tail->next = b;
			break;
		}
		else if (b == NULL)
		{
			tail->next = a;
			break;
		}
		if (a->data <= b->data)
			Movenode(&(tail->next), &a);
		else
			Movenode(&(tail->next), &b);

		tail = tail->next;
	}
	return(dummy.next);
}

struct node* LL_merge(struct node* a, struct node* b)
{
	/* a dummy first node to hang the result on */
	struct node dummy;

	/* tail points to the last result node  */
	struct node* tail = &dummy;

	/* so tail->next is the place to add new nodes
	  to the result. */
	dummy.next = NULL;
	Movenode(&(tail->next), &a);
	a = a->next;
	tail = tail->next;
	while (1)
	{
		if (a == NULL)
		{
			/* if either list runs out, use the
				other list */
			tail->next = b;
			break;
		}
		else if (b == NULL)
		{
			tail->next = a;
			break;
		}
		if (a->data <= b->data)
			Movenode(&(tail->next), &a);
		else
			Movenode(&(tail->next), &b);

		tail = tail->next;
	}
	return(dummy.next);
}

/* The function removes duplicates from a sorted list */
void removeDuplicates(struct node* head)
{
	/* Pointer to traverse the linked list */
	struct node* current = head;
 
	/* Pointer to store the next pointer of a node to be deleted*/
	struct node* next_next;

	/* do nothing if the list is empty */
	if (current == NULL)
		return;
 
	/* Traverse the list till last node */
	while (current->next != NULL)
	{
		/* Compare current node with next node */
		if (current->data == current->next->data)
		{
			/* The sequence of steps is important */
			next_next = current->next->next;
			free(current->next);
			current->next = next_next;
		}
		else /* This is tricky: only advance if no deletion */
		{
		  current = current->next;
		}
	}
}

/* UTILITY FUNCTIONS */
/* Movenode() function takes the node from the front of the
	source, and move it to the front of the dest.
	It is an error to call this with the source list empty.

	Before calling Movenode():
	source == {1, 2, 3}
	dest == {1, 2, 3}

	Affter calling Movenode():
	source == {2, 3}
	dest == {1, 1, 2, 3} */
void Movenode(struct node** destRef, struct node** sourceRef)
{
	/* the front source node  */
	struct node* newnode = *sourceRef;
	assert(newnode != NULL);

	/* Advance the source pointer */
	*sourceRef = newnode->next;

	/* Link the old dest off the new node */
	newnode->next = *destRef;

	/* Move dest to point to the new node */
	*destRef = newnode;
}
#endif

/*	Function to insert a node at the beginging of the
	linked list */
void pushll(struct node** head_ref, int new_data)
{
	/* allocate node */
	struct node* new_node =
		(struct node*) malloc(sizeof(struct node));

	/* put in the data  */
	new_node->data = new_data;

	/* link the old list off the new node */
	new_node->next = (*head_ref);

	/* move the head to point to the new node */
	(*head_ref)	= new_node;
}

/* Function to print nodes in a given linked list */
void printList(struct node *node)
{
	while (node!=NULL)
	{
		printf("%d ", node->data);
		node = node->next;
	}
}
#endif
