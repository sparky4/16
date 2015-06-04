#include "src/lib/mapread.h"

#define DUMP
//#define DUMP_MAP

void
main(int argc, char *argv[])
{
	map_t map;
	short i;
	loadmap("data/test.map", &map);
	#ifdef DUMP
	fprintf(stdout, "map.width=	%d\n", map.width);
	fprintf(stdout, "map.height=	%d\n", map.height);
	#ifdef DUMP_MAP
	for(i=0; i<(map.width*map.height); i++)
	{
		fprintf(stdout, "%04d[%02d]", i, map.data[i]);
		if(i && !(i%map.width)) fprintf(stdout, "\n");
	}
	fprintf(stdout, "\n");
	#endif
	fprintf(stdout, "&map==%Fp\n", &map);
	fprintf(stdout, "&map.tiles==%Fp\n", map.tiles);
	fprintf(stdout, "&map.width==%Fp\n", map.width);
	fprintf(stdout, "&map.height==%Fp\n", map.height);
	fprintf(stdout, "&map.data==%Fp\n", map.data);
	#endif
	fprintf(stdout, "okies~\n");
}
