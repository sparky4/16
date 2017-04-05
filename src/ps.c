#include <stdio.h>
#include "src/lib/16_head.h"

void main()
{
	static global_game_variables_t gvar;
	printf("video:\n");
	PRINT_OPAQUE_STRUCT(&gvar.video);
	printf("ca:\n");
	PRINT_OPAQUE_STRUCT(&gvar.ca);
	printf("pm:\n");
	PRINT_OPAQUE_STRUCT(&gvar.pm);
	printf("mm:\n");
	PRINT_OPAQUE_STRUCT(&gvar.mm);
	printf("mmi:\n");
	PRINT_OPAQUE_STRUCT(&gvar.mmi);
	printf("sizeof gvar=%d\n", sizeof(gvar));
//	handle
//	kurokku
//	in
	//player
	//mv
}
