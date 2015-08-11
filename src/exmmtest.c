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
	exmm test
*/
//#include <stdio.h>
//#include <bios.h>

#include "src/lib/16_head.h"
#include "src/lib/16_ca.h"
#include "src/lib/16_mm.h"
#include "src/lib/16_hc.h"
//#include "src/lib/modex16.h"
#pragma hdrstop

#pragma warn -pro
#pragma warn -use

//file load or read definition
#define FILERL
//#define FILEREAD

void
main(int argc, char *argv[])
{
	global_game_variables_t gvar;
	mminfo_t mm; mminfotype mmi;
#ifdef __WATCOMC__
	__segment sega;
	void __based(sega)* bigbuffer;
#endif
#ifdef __BORLANDC__
	memptr bigbuffer;
#endif
#ifdef FILERL
	//static byte bakapee[64];
	char *bakapee;
	word baka;
#endif
	//static page_t screen;

#ifdef __BORLANDC__
	argc=argc;
#endif
	//printf("&main()=	%Fp\n", *argv[0]);
	//printf("bigbuffer=	%Fp\n", bigbuffer);
	//printf("&bigbuffer=	%Fp\n", &bigbuffer);
	//printf("bigbuffer=	%04x\n", bigbuffer);
	//printf("&bigbuffer=	%04x\n", &bigbuffer);

#ifdef FILERL
	bakapee = malloc(64);
#endif
	mm.mmstarted=0;

#ifdef FILERL
//	printf("filename!: ");
//	scanf("%[^\n]", &bakapee);
	if(argv[1]) bakapee = argv[1];
	else bakapee = "data/koishi~.pcx";
#endif

//	textInit();

	// setup camera and screen~
	//bug!!!
	//screen = modexDefaultPage();
	//screen.width += (16*2);
	//screen.height += (16*2);

//	printf("main()=%Fp	start MM\n", *argv[0]);
	MM_Startup(&mm, &mmi);
	//PM_Startup();
	//PM_UnlockMainMem();
	CA_Startup(&gvar);
//	printf("		done!\n");
	//printf("&main()=	%Fp\n", *argv[0]);
	//printf("bigbuffer=	%Fp\n", bigbuffer);
	//printf("&bigbuffer=	%Fp\n", &bigbuffer);
	//printf("bigbuffer=	%04x\n", bigbuffer);
	//printf("&bigbuffer=	%04x\n", &bigbuffer);
//	getch();
#ifdef FILERL
//	bakapeehandle = open(bakapee,O_RDONLY | O_BINARY, S_IREAD);
//	printf("size of big buffer~=%u\n", _bmsize(segu, bigbuffer));
//	if(CA_FarRead(bakapeehandle,(void far *)&bigbuffer,sizeof(bigbuffer),&mm))
#ifdef FILEREAD
	printf("		read\n");
	if(CA_ReadFile(bakapee, &bigbuffer, &mm))
#else
	printf("		load\n");
	if(CA_LoadFile(bakapee, &bigbuffer, &mm, &mmi))
#endif
		baka=1;
	else
		baka=0;
//	close(bakapeehandle);
	//hmm functions in cache system use the buffered stuff
#ifdef __WATCOMC__
	printf("size of big buffer~=%u\n", _bmsize(sega, bigbuffer));
#endif
#endif
	//printf("dark purple = purgable\n");
	//printf("medium blue = non purgable\n");
	//printf("red = locked\n");
	printf("press any key to continue!\n");
	getch();
	//++++modexEnter();
	//++++modexShowPage(&screen);
	MM_ShowMemory(&gvar, &mm);
	//getch();
	MM_DumpData(&mm);
	//++++modexLeave();
	MM_Report(&mm, &mmi);
//	printf("		stop!\n");
#ifdef FILERL
	MM_FreePtr(&bigbuffer, &mm);
#endif
	//PM_Shutdown();
	CA_Shutdown(&gvar);
	MM_Shutdown(&mm);
//	printf("		done!\n");
#ifdef FILERL
	free(bakapee);
	if(baka) printf("\nyay!\n");
	else printf("\npoo!\n");
#endif
	printf("========================================\n");
	printf("near=%Fp	", mm.nearheap);
	printf("far=%Fp	\n", mm.farheap);
	printf("near=%Fp	", &(mm.nearheap));
	printf("far=%Fp	\n", &(mm.farheap));
	printf("bigbuffer=	%Fp\n", bigbuffer);
	printf("&bigbuffer=	%Fp\n", &bigbuffer);
	printf("bigbuffer=	%04x\n", bigbuffer);
	printf("&bigbuffer=	%04x\n", &bigbuffer);
	//printf("\n\n");
	//printf("Total free:			%lu\n", (dword)(GetFreeSize()));
	//printf("Total near free:		%lu\n", (dword)(GetNearFreeSize()));
	//printf("Total far free:			%lu\n", (dword)(GetFarFreeSize()));
	//getch();
//	printf("\n");
#ifdef __WATCOMC__
	heapdump(&gvar);
#endif
	//printf("core left:			%lu\n", (dword)_coreleft());
	//printf("far core left:			%lu\n", (dword)_farcoreleft());
	//printf("based core left:			%lu\n", (dword)_basedcoreleft());
	//printf("huge core left:			%lu\n", (dword)_hugecoreleft());
}
