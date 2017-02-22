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
/*
	exmm test
*/
#include "src/lib/16_head.h"
#include "src/lib/16_tail.h"
#include "src/lib/16_pm.h"
#include "src/lib/16_ca.h"
#include "src/lib/16_mm.h"
#include "src/lib/16_hc.h"
//#include "src/lib/16_vl.h"
#pragma hdrstop

#pragma warn -pro
#pragma warn -use

//file load or read definition
#define FILEREAD
//#define EXMMVERBOSE
//#ifdef __BORLANDC__
#define BUFFDUMP
//#endif

#define BBUF bigbuffer//gvar.ca.tinf[0]

void VGAmodeX(sword vq, boolean cmem, global_game_variables_t *gv)
{
	printf("VGAmodeX dummy:\n	%Fp	%Fp	%Fp\n", &vq, &cmem, gv);
}
#define PRINTBB {\
	printf("&main()=	%Fp\n", argv[0]);\
	printf("BBUF:\n");\
	printf("	%Fp\t", BBUF);\
	printf("&%Fp\n", &BBUF);\
	printf("	     %04x\t", BBUF);\
	printf("&     %04x\n", &BBUF);\
}
	//printf("&main()=	%Fp\n", *argv[0]);
	//printf("bigbuffer=	%Fp\n", bigbuffer);
	//printf("&bigbuffer=	%Fp\n", &bigbuffer);
	//printf("bigbuffer=	%04x\n", bigbuffer);
	//printf("&bigbuffer=	%04x\n", &bigbuffer);
#ifdef __WATCOMC__
void segatesuto()
{
	__segment screen;
	char __based( void ) * scrptr;

	screen = 0xB800;
	scrptr = 0;
	printf( "Top left character is '%c'.\n", *(screen:>scrptr) );
// 	printf("Next string is: [");
// 	while(*scrptr<16)
// 	{
// 		printf("%c", *(screen:>scrptr));
// 		//printf("\b");
// 		scrptr++;
//
// 	}
// 	printf("]\n");
//	printf("press any key to continue!\n");
//	getch();
}
#endif

void
main(int argc, char *argv[])
{
	byte w=1;
	static global_game_variables_t gvar;
	memptr BBUF;
//#ifdef __WATCOMC__
//	__segment sega;
//#endif
	//static byte bakapee[64];
	char *bakapee1, *bakapee2;
	word baka;

#ifdef __BORLANDC__
	argc=argc;
#endif
	//file name //
	bakapee1 = malloc(64);
	bakapee2 = malloc(64);
	//file name //

#ifdef __16_PM__
#ifdef __DEBUG_PM__
	dbg_debugpm=1;	//debug pm
#endif
#endif

	//PRINTBB
	if(argv[1]){ bakapee1 = argv[1];
	if(argv[2]) bakapee2 = argv[2]; }
	else{
	//{ printf("filename!: "); scanf("%[^\n]", &bakapee); }
		bakapee1 = "data/koishi~.pcx";
		bakapee2 = "data/test.map";
	}
	//printf("main()=%Fp	start MM\n", *argv[0]);
	MM_Startup(&gvar);
	//printf("ok\n");
#ifdef __16_PM__
#ifdef __DEBUG_PM__
	if(dbg_debugpm>0)
	{
#endif
		PM_Startup(&gvar); PM_CheckMainMem(&gvar); PM_UnlockMainMem(&gvar);
#ifdef __DEBUG_PM__
	}
#endif
#endif
	CA_Startup(&gvar);
//	printf("		done!\n");
	//0000
	PRINTBB; printf("press any key to continue!\n"); getch();
#ifdef FILEREAD
for(w=0;w<2;w++)
{
//	printf("size of big buffer~=%u\n", _bmsize(segu, BBUF));
	if(w>0)
	{
		printf("		read\n");
		if(CA_ReadFile(bakapee2, &BBUF, &gvar)) baka=1; else baka=0;
	}
#endif
	if(w==0)
	{
		printf("		load\n");
		if(CA_LoadFile(bakapee1, &BBUF, &gvar)) baka=1; else baka=0;
	}
//#ifdef __WATCOMC__
//	printf("\nsize of big buffer~=%u\n", _bmsize(sega, BBUF));
//#endif
#ifdef BUFFDUMP
	printf("contents of the buffer\n[\n%s\n]\n", (BBUF));
#endif// #else
	PRINTBB;
// #endif
	//printf("dark purple = purgable\n");
	//printf("medium blue = non purgable\n");
	//printf("red = locked\n");
//	printf("press any key to continue!\n");
//	DebugMemory_(&gvar, 1);
	if(baka) printf("\nyay!\n");
	else printf("\npoo!\n");
	printf("press any key to continue!\n");
	getch();
#ifdef FILEREAD
}
#endif
	DebugMemory_(&gvar, 1);
	MM_DumpData(&gvar);
	MM_Report_(&gvar);
	//printf("bakapee1=%s\n", bakapee1);
	//printf("bakapee2=%s\n", bakapee2);
	MM_FreePtr(&BBUF, &gvar);
#ifdef __16_PM__
#ifdef __DEBUG_PM__
	if(dbg_debugpm>0)
#endif
		PM_Shutdown(&gvar);
#endif
	CA_Shutdown(&gvar);
	MM_Shutdown(&gvar);
	free(bakapee1); free(bakapee2);
	printf("========================================\n");
	printf("near=	%Fp ", gvar.mm.nearheap);
	printf("far=	%Fp", gvar.mm.farheap);
	printf("\n");
	printf("&near=	%Fp ", &(gvar.mm.nearheap));
	printf("&far=	%Fp", &(gvar.mm.farheap));
	printf("\n");
#ifdef EXMMVERBOSE
	printf("bigb=	%Fp ", BBUF);
	//printf("bigbr=	%04x", BBUF);
	//printf("\n");
	printf("&bigb=%Fp ", &BBUF);
	//printf("&bigb=%04x", &BBUF);
	printf("\n");
	printf("========================================\n");
#endif
	printf("\n");
#ifdef __WATCOMC__
//this is far	printf("Total free:			%lu\n", (dword)(GetFreeSize()));
	printf("Total near free:		%lub\n", (dword)(GetNearFreeSize()));
	printf("Total far free:			%lub\n", (dword)(GetFarFreeSize()));
	heapdump(&gvar);
//	segatesuto();
#endif
#ifdef __BORLANDC__
	//printf("core left:			%lu\n", (dword)_coreleft());
	//printf("far core left:			%lu\n", (dword)_farcoreleft());
	printf("core left:			%lu\n", (dword)coreleft());
	printf("far core left:			%lu\n", (dword)farcoreleft());
#endif
	printf("Project 16 exmmtest.exe. This is just a test file!\n");
	printf("version %s\n", VERSION);
	//printf("\n");

	//printf("based core left:			%lu\n", (dword)_basedcoreleft());
	//printf("huge core left:			%lu\n", (dword)_hugecoreleft());
}
