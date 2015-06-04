#include <conio.h>
#include "src/lib/mapread.h"

//#define DUMP
//#define DUMP_MAP

void
main(int argc, char *argv[])
{
	map_t map;
	short i;
	char *fmt = "Memory available = %u\n";
	char *fmt0 = "Largest Contiguous Block of Memory available = %u\n";

	fprintf(stderr, fmt, _memavl());
	fprintf(stderr, fmt0, _memmax());
	fprintf(stderr, "Size of map var = %u\n", _msize(&map));
	fprintf(stderr, "program always crashes for some reason....");
	getch();

	fprintf(stderr, "loading~\n\n");
	loadmap("data/tes0.map", &map);
	fprintf(stderr, "\nokies~\n");
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
