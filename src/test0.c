#include <stdio.h>
#include <malloc.h>

void main()
{
	unsigned w;
	unsigned far *z;
// 	w = _nmalloc(sizeof(unsigned));
	z=(&w);
	for(w=0; w<16;w++)
	{
		printf("z=(%Fp)%u	w=(%Fp)%u\n", z, *z, &w, w);
	}
}
