#include<stdio.h>

int main(){
	char far *p =(char far *)0x55550005;
	char far *q =(char far *)0x53332225;
	*p = 80;
	(*p)++;
	printf("%d",*q);
	printf("%d",*p);
	return 0;
}