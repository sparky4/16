/* Project 16 Source Code~
 * Copyright (C) 2012-2017 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
 *
 * This file is part of Project 16.
 *
 * Project 16 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Project 16 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#include "src/lib/16_map.h"

#define DUMP
#define DUMP_MAP

void
main(int argc, char *argv[])
{
	static global_game_variables_t gvar;
	static map_t map;
#ifdef DUMP
#ifdef DUMP_MAP
	short i;
	word k;
#endif
#endif
	char *fmt = "Memory available = %u\n";
	char *fmt0 = "Largest Contiguous Block of Memory available = %u\n";

	MM_Startup(&gvar);
	PM_Startup(&gvar);
	//printf("pmstarted ok\n");
	//PM_CheckMainMem(&gvar);
	PM_UnlockMainMem(&gvar);
	CA_Startup(&gvar);

	fprintf(stderr, fmt, _memavl());
	fprintf(stderr, fmt0, _memmax());
	fprintf(stderr, "Size of map var = %u\n", _msize(&(gvar.ca.camap.mapsegs)));
	//fprintf(stderr, "program always crashes for some reason....\n");
	getch();

	//loadmap("data/test.map", &map);
	//newloadmap("data/test.map", &map);
//	CA_loadmap("data/test.map", &map, &gvar);
	CA_loadmap("data/newtest.map", &map, &gvar);
#ifdef DUMP
	fprintf(stdout, "map.width=	%d\n", map.width);
	fprintf(stdout, "map.height=	%d\n", map.height);
	getch();
#ifdef DUMP_MAP
	//if(map.width*map.height != 1200)
	for(k=0;k<MAPLAYERS;k++)
	{
		printf("maplayer: %u\n[\n", k);
		for(i=0; i<(map.width*map.height); i++)
		{
			//fprintf(stdout, "%04d[%02d]", i, map.data[i]);
			//fprintf(stdout, "%c", map.MAPDATAPTK[i]+44);
			fprintf(stdout, "%c", map.layerdata[k].data[i]+44);
			if(!((i+1)%map.width)){
				//fprintf(stdout, "[%d]", i);
				fprintf(stdout, "\n"); }
		}
		fprintf(stdout, "]\n");
		getch();
	}
#else
	//fprintf(stderr, "contents of the buffer\n[\n%s\n]\n", (gvar.ca.camap.mapsegs));
#endif
	/*fprintf(stdout, "&main()=%Fp\n", *argv[0]);
	fprintf(stdout, "&map==%Fp\n", &map);
	fprintf(stdout, "&map.tiles==%Fp\n", map.tiles);
	fprintf(stdout, "&map.width==%Fp\n", map.width);
	fprintf(stdout, "&map.height==%Fp\n", map.height);
	fprintf(stdout, "&map.data==%Fp\n", map.data);*/
#endif
	//fprintf(stdout, "okies~\n");
	MM_FreePtr(&(gvar.ca.camap.mapsegs), &gvar);
	PM_Shutdown(&gvar);
	CA_Shutdown(&gvar);
	MM_Shutdown(&gvar);
}
