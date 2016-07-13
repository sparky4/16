#include <stdio.h>
#include <malloc.h>

void baka(unsigned far *z)
{
	unsigned w;
// 	w = _nmalloc(sizeof(unsigned));
	z=(&w);
	for(w=0; w<16;w++)
	{
		printf("z=(%Fp)%u	w=(%Fp)%u\n", z, *z, &w, w);
	}
	printf("\n");
}

void pee(unsigned far *z)
{
	unsigned near *w;
	w = _nmalloc(sizeof(unsigned));
	z=(w);
	for((*w)=0;(*w)<16;(*w)++)
	{
		printf("z=(%Fp)%u	w=(%Fp)%u\n", z, *z, w, *w);
	}
	printf("\n");
}

void main()
{
	unsigned near w;
	unsigned far *z;
	baka(z);
	pee(z);
	printf("&w=%Fp\n", &w);
	printf("&z=%Fp\n", &z);
	printf("*z=%Fp\n", *z);
}
