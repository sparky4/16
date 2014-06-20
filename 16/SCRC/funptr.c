#include <stdio.h>

/* a function pointer to a void pointer which takes one int argument */
typedef void (*trigger)(int);


/* invokes a list of functions from an array */
void invokeTriggers(trigger *list, int n) {
    int i;
    trigger *tf = list;

    for(i=0; i<n; i++) {
	//invoke each function
	(*tf)(i);
	tf++;
    }
}


void f1(int x) {
     printf("f1: %d\n", x);
}

void f2(int x) {
    printf("f2: %d\n", x);
}


void main()  {
   trigger list[3];

   /* set up the list */
   list[0] = f1;
   list[1] = f2;
   list[2] = f1;

   invokeTriggers(list, 3);

   return;
}


