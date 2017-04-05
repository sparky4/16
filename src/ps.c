#include <stdio.h>
#include "src/lib/16_head.h"

#define PRINT_OPAQUE_STRUCT(p)  print_mem((p), sizeof(*(p)))

void print_mem(void const *vp, size_t n)
{
	size_t i;
	unsigned char const *p = vp;
	for (i=0; i<n; i++)
		printf("%02x\n", p[i]);
	putchar('\n');
};

void main()
{
	static global_game_variables_t gvar;
	PRINT_OPAQUE_STRUCT(&gvar);
}
