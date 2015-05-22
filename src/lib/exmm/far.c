#include<stdio.h>
#include <conio.h>
#include <limits.h>

int main(){
	unsigned short i=0;
	char near w;
	char huge *p =(char far *)0xF0000000;
	//char far *q =(char far *)0x53332225;
	printf("1p=%p\n", p);
	while(p<=0xFFFFFFFF/*0<=i<(_UI16_MAX)*/ && !kbhit())
	{
		printf("*p=%c	%ul\n",*p, i);
//		*p = i;
//		(*p)++;
		p++;
		i++;
	}
	/*while((_UI16_MAX/2)<=i<(_UI16_MAX) && !kbhit())
	{
		printf("*p=%d	%d\n",*p, i);
		/p = i;
		//(*p)++;
		p++;
		i++;
	}*/
	w=104;
	//printf("*q=%d\n",*q);
	//printf(" q=%p\n", q);
	printf(" p=%p\n", p);
	printf(" w=%d\n", w);
	printf(" w=%p\n", w);
	return 0;
}
