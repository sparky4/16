#include<stdio.h>
#include <conio.h>
#include <limits.h>

int main(){
	unsigned long i=0;
	char near w;
	char huge *p =(char far *)0xD0000000;
	//char far *q =(char far *)0x53332225;
	printf("1p=%p	%ul\n", p, i);
	while(p<=(char far *)0xDFFFFFFF/*i<262144UL*/ && !kbhit())
	{
		printf("p=%p=%x	%ul\n", p, *p, i);
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
