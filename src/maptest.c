#include "src/lib/mapread.c"

int main()
{
	map_t map;
	loadmap("data/test.map", &map);
	return 0;
}
