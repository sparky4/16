/* Project 16 Source Code~
 * Copyright (C) 2012-2017 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
 *
 * This file is part of Project 16.
 *
 * Project 16 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Project 16 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
/*
 * palllist experiment
 */
/*
 * An experiment where I make 2 small linked list which points to 2 arrays
 * one is the "hardware palette" array
 * the other is "image palette" array
 * and their respectable pointers point to their [i] stuff
 *
 * the palette updater stuff i want to make is this:
 *
 * VGA		IMG
 * 1-			2-
 * 2-			4-
 * 3-			0-
 * 4-			9-
 * 5-			0-(I will figure this out later if  there is dup numbs)
 * 6-
 * 7-
 * 8-
 *
 *		MERGE THEM INTO
 *
 * VGA
 * 1-
 * 2->IMG[0]
 * 3-
 * 4->IMG[1]
 * 5
 * 6
 * 7
 * 8
 * 9->IMG[3]
 * 0->IMG[2]->IMG[4]
 *
 * i am going to work on a really big area of code it will be on the refresh system, the walking/scrolling system, things that use showpage, adding disableing 8087 functions if no 8087 detected, and a bunch of other things i cannot remember off the top of my head because i am BURNT. I need a small break~ -- -- -- -- update! i am working on this i made ZC_WALK and i am going to move to VRL/VRS soon! .
 * ==== PRE SHOWPAGE TO SHOWMV CONVERSION ==== i am going to work on a really big area of code it will be on the refresh system, the walking/scrolling system, things that use showpage, adding disableing 8087 functions if no 8087 detected, and a bunch of other things i cannot remember off the top of my head because i am BURNT. I need a small break~ -- -- -- -- update! i am working on this i made ZC_WALK and i am going to move to VRL/VRS soon!
 */
#include "src/lib/ll.h"

#if 0
//def OTHERMERGELISTSTIFF

void
main(int argc, char *argv[])
{
	int i;
	node_t * vga = NULL;			node_t * imgpal = NULL;
	vga = malloc(sizeof(node_t));	imgpal = malloc(sizeof(node_t));
	vga->val = 24;			imgpal->val = 9;
	vga->next=NULL;			imgpal->next=NULL;
	vga->id=0;				imgpal->id=0;

	printf("	");
	for(i=1; i<=
		//PAL_SIZE
		6
		; i++)
	{
		if(!(i%3)) printf("\n	");
		printf("%d,", i);
		pushe(vga, i);
	}

	printf("\n\n	");

	for(i=4; i>0; i--)
	{
		if(!(i%3)) printf("\n	");
		printf("%d,", i);
		pushe(imgpal, i);
	}
	printf("\n");

	printf("size of vga = %d\n", listLength(vga));
//#if 0
	printf("\n=======================\n");
	printf("vga list printings\n=======================\n");
	print_list(vga);
	printf("\n=======================\n");
	printf("imgpal list printings\n=======================\n");
	print_list(imgpal);
//	printf("\n=======================\n");
//#endif
	i=1;//skip overscan
	while(i!=listLength(imgpal))
	{
		
	}
	free(vga);
	free(imgpal);
}
#else
#if 0
/* C/C++ program to merge two sorted linked lists */
//	from http://www.geeksforgeeks.org/merge-two-sorted-linked-lists/

/* Drier program to test above functions*/
void main()
{
	/* Start with the empty list */
	struct node* res = NULL;
	struct node* a = NULL;
	struct node* b = NULL;

	/* Let us create two sorted linked lists to test
	  the functions
	   Created lists, a: 5->10->15,  b: 2->3->20 */
	pushll(&a, 15);
	pushll(&a, 10);
	pushll(&a, 4);
	pushll(&a, 3);
	pushll(&a, 2);
	pushll(&a, 1);
	pushll(&a, 0);

	pushll(&b, 20);
	pushll(&b, 3);
	pushll(&b, 2);
	pushll(&b, 4);

	printf("\n");
	printf("The 2 Linked List are: \n");
	printList(a);	printf("\n");
	printList(b);	printf("\n");

	/* Remove duplicates from linked list */
	res = SortedMerge(a, b);
//	res = LL_merge(a, b);
	
	printf("Merged Linked List is: \n");
	printList(res);
}
#endif
/* C Program to remove duplicates from a sorted linked list */

/* Drier program to test above functions*/
void main()
{
	/* Start with the empty list */
	struct node* head = NULL;
   
	/* Let us create a sorted linked list to test the functions
	 Created linked list will be 11->11->11->13->13->20 */
	pushll(&head, 20);
	pushll(&head, 13);
	pushll(&head, 13);
	pushll(&head, 11);
	pushll(&head, 11);
	pushll(&head, 11);

	printf("\n Linked list before duplicate removal  ");
	printList(head);

	/* Remove duplicates from linked list */
	removeDuplicates(head);

	printf("\n Linked list after duplicate removal ");
	printList(head);
}
#endif
