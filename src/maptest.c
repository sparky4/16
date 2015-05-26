#include "src/lib/mapread.h"

#define DUMP
#define DUMP_MAP

void
main(int argc, char *argv[])
{
	map_t map;
	short i;
	//int lm;
	loadmap("data/test.map", &map);
	//fprintf(stdout, "loadmap=	%d\n\n", lm);
	#ifdef DUMP
	fprintf(stdout, "map.width=	%d\n", map.width);
	fprintf(stdout, "map.height=	%d\n", map.height);
	#ifdef DUMP_MAP
	for(i=0; i<(map.width*map.height); i++)
	{
		fprintf(stdout, "%04d[%02d]", i, map.data[i]);
		if(i && !(i%map.width)) fprintf(stdout, "\n");
	}
	#endif
	#endif
}
