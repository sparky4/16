#include <stdio.h>

void
main(int argc, char *argv[])
{
	int x=420;
	int huge *ptr=&x;

	//ptr=&x;
	printf("ptr=%Fp\n", ptr);
	printf("*ptr=%d\n", *ptr);
	printf("&x=%Fp\n", &x);
	printf("x=%d\n", x);
}
