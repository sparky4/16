#include <stdio.h>

void
main(int argc, char *argv[])
{
	int x=420;
	int huge *ptr=&x;
	int *ptr0=&x;

	//ptr=&x;
	printf("&main()=%Fp\n", *argv[0]);
	printf("ptr0=%Fp\n", ptr0);
	//printf("&ptr0=%Fp\n", &ptr0);
	printf("*ptr0=%d\n", *ptr0);
	printf("ptr=%Fp\n", ptr);
	//printf("&ptr=%Fp\n", &ptr);
	printf("*ptr=%d\n", *ptr);
	printf("address of *ptr=%Fp\n", *ptr);
	printf("&x=%Fp\n", &x);
	printf("x=%d\n", x);
}
