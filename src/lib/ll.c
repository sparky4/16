#include "src/lib/ll.h"

void print_list(node_t * head)
{
	node_t * current = head;

	while (current != NULL)
	{
		printf("%d\n", current->val);
		current = current->next;
	}
}

void pushe(node_t * head, int val)
{
	node_t * current = head;
	while (current->next != NULL)
	{
		current = current->next;
	}

	// now we can add a new variable
	current->next = malloc(sizeof(node_t));
	current->next->val = val;
	current->next->next = NULL;
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
