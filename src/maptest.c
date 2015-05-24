#include "src/lib/mapread.c"

void
main(int argc, char *argv[])
{
	map_t map;
	//loadmap("data/test.map", &map);
	fprintf(stderr, "%d\n", map.data[0]);
}
