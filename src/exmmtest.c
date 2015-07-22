/* Project 16 Source Code~
 * Copyright (C) 2012-2015 sparky4 & pngwen & andrius4669
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
/*
	input test
*/
#include "src/lib/16_ca.h"
#include "src/lib/16_mm.h"
//#include "src/lib/modex16.h"

#define FILERL
//#define FILEINIT

void
main(int argc, char *argv[])
{
	static mminfo_t mm; mminfotype mmi;
	static memptr	bigbuffer;
	static __segment segu;
	static char *bakapee;
	word baka;
	//static page_t screen;

	printf("&main()=%Fp\n", *argv[0]);
	printf("&argc=%Fp\n", FP_SEG(&argc));
	printf("&segu=%Fp\n", segu);

	bakapee = _nmalloc(64);
//	memset(bakapee, 0, 64);
	mm.mmstarted=0;

	if(argv[1]) bakapee = argv[1];
	else bakapee = "data/koishi~~.pcx";

	textInit();

	/* setup camera and screen~ */
	//bug!!!
	//screen = modexDefaultPage();
	/*screen.width += (16*2);
	screen.height += (16*2);*/

	printf("&main()=%Fp	start MM\n", *argv[0]);
	MM_Startup(&mm, &mmi);
	//segu=mmi.segu;
	printf("		done!\n");
	/*if(FP_SEG(*argv[0])==0)
	{
		MM_Report(&screen, &mm, &mmi);
		MM_Shutdown(&mm);
		printf("&main()=%Fp\n", *argv[0]);
		printf("&main() == %u\n", FP_SEG(*argv[0]));
		exit(-5);
	}*/
	printf("&main()=%Fp\n", *argv[0]);
	printf("&argc=%Fp\n", FP_SEG(&argc));
	printf("&segu=%Fp\n", segu);
#ifdef FILERL
#ifdef FILEINIT
	printf("		read\n");
	if(CA_ReadFile(bakapee, &bigbuffer, &mm/*, &mmi*/))
#else
	printf("		load\n");
	if(CA_LoadFile(bakapee, &bigbuffer, &mm, &mmi))
#endif
		baka=1;
	else
		baka=0;
#endif
	//MM_GetPtr(&bigbuffer, mmi.nearheap, &mm, &mmi);
	//hmm functions in cache system use the buffered stuff
	printf("size of big buffer~=%u\n", _bmsize(segu, bigbuffer));
	printf("dark purple = purgable\n");
	printf("medium blue = non purgable\n");
	printf("red = locked\n");
	getch();
	//modexEnter();
	//modexShowPage(&screen);
	MM_ShowMemory(/*&screen, */&mm);
	//getch();
	MM_DumpData(&mm);
	//modexLeave();
	MM_Report(&mm, &mmi);
	printf("		stop!\n");
	MM_FreePtr(&bigbuffer, &mm);
	MM_Shutdown(&mm);
	printf("		done!\n");
#ifdef FILERL
	free(bakapee);
	//printf("_bios_memsize=%u\n", _bios_memsize());
	if(baka) printf("\nyay!\n");
	else printf("\npoo!\n");
#endif
}
