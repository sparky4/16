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
#include <malloc.h>		// for _memavl()
#include "src/lib/16_head.h"
#include "src/lib/16_tail.h"
#include "src/lib/16_pm.h"
#include "src/lib/16_ca.h"
#include "src/lib/16_mm.h"
#include "src/lib/16_hc.h"
//#include "src/lib/16_vl.h"
#include "src/lib/16_dbg.h"

#pragma hdrstop

#pragma warn -pro
#pragma warn -use

//file load or read definition
#define FILEREADLOAD
#define FILEREAD
//#define EXMMVERBOSE
//#define BUFFDUMPPAUSE
//#define EXMMVERBOSE__
//	#define PRINTBBDUMP
#define BUFFDUMP

#define BBUFNAME gvar.ca.tinf[0]//bigbuffer
//#define INITBBUF static memptr BBUFNAME;
#define BBUFPTR	&BBUFNAME

#ifdef __BORLANDC__
#define BBUF		(memptr *)BBUFPTR//
#define BBUFSTRING	(memptr *)BBUFNAME
#endif
#ifdef __WATCOMC__
#define BBUF		BBUFNAME
#define BBUFSTRING	BBUF
#endif


void VGAmodeX(sword vq, boolean cmem, global_game_variables_t *gv)
{
	printf("VGAmodeX dummy:\n	%Fp	%Fp	%Fp\n", &vq, &cmem, gv);
}

//printf("*	%Fp\t", *BBUF);
//printf("*	     %04x\t", *BBUF);
#define PRINTBB {\
	printf("-------------------------------------------------------------------------------\n");\
	printf("&main()=	%Fp\n", argv[0]);\
	printf("buffer:\n");\
	printf("	%Fp\t", BBUF);\
	printf("&%Fp\n", BBUFPTR);\
	printf("	     %04x\t", BBUF);\
	printf("&     %04x\n", BBUFPTR);\
	printf("-------------------------------------------------------------------------------\n");\
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
	byte w;
	static global_game_variables_t gvar;
#ifdef INITBBUF
	INITBBUF
#endif
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
	bakapee1 = malloc(24);
	bakapee2 = malloc(24);
	//file name //

#ifdef __16_PM__
#ifdef __DEBUG_PM__
	dbg_debugpm=1;	//debug pm
#endif
#endif

#ifdef __DEBUG_CA__
	dbg_debugca=1;
#endif
#ifdef PRINTBBDUMP
//0000PRINTBB; printf("press any key to continue!\n"); getch();
#endif
	if(argv[1]){ bakapee1 = argv[1];
	if(argv[2]) bakapee2 = argv[2]; }
	else{
	//{ printf("filename!: "); scanf("%[^\n]", &bakapee); }
		bakapee1 = "data/koishi~.pcx";
		bakapee2 = "data/test.map";
	}
	//printf("main()=%Fp	start MM\n", *argv[0]);
	MM_Startup(&gvar); //printf("ok\n");
#ifdef __16_PM__
#ifdef __DEBUG_PM__
	if(dbg_debugpm>0)
	{
#endif
		PM_Startup(&gvar); PM_CheckMainMem(&gvar); PM_UnlockMainMem(&gvar);
//0000		printf("PM Started\n"); printf("press any key to continue!\n"); getch();
#ifdef __DEBUG_PM__
	}
#endif
#endif

#ifdef __DEBUG_MM__
	dbg_debugmm=0;
#endif
	CA_Startup(&gvar);
//	printf("		done!\n");
#ifdef PRINTBBDUMP
//0000
PRINTBB; printf("\n\npress any key to continue!\n"); getch();
#endif
#ifdef __DEBUG_MM__
	dbg_debugmm=1;
#endif

	w=0;
#ifdef FILEREADLOAD
#ifdef FILEREAD
	for(;w<2;w++)
	{
	//	printf("size of big buffer~=%u\n", _bmsize(segu, BBUF));
		if(w>0)
		{
			printf("======================================read=====================================\n");
			if(CA_ReadFile(bakapee2, BBUFPTR, &gvar)) baka=1; else baka=0;
			printf("====================================read end===================================\n");
		}
#endif
		if(w==0)
		{
			printf("======================================load=====================================\n");
			if(CA_LoadFile(bakapee1, BBUFPTR, &gvar)) baka=1; else baka=0;
			printf("====================================load end===================================\n");
		}
//#ifdef __WATCOMC__
//	printf("\nsize of big buffer~=%u\n", _bmsize(sega, BBUF));
//#endif
#ifdef BUFFDUMP
		printf("contents of the buffer\n[\n%s\n]\n", BBUFSTRING);
#endif// #else
#ifdef PRINTBBDUMP
		PRINTBB;
#endif
//endif // BUFFDUMP

		//printf("dark purple = purgable\n");
		//printf("medium blue = non purgable\n");
		//printf("red = locked\n");
	//	printf("press any key to continue!\n");
	//	DebugMemory_(&gvar, 1);
		if(baka) printf("\nyay!\n");
		else printf("\npoo!\n");
#ifdef BUFFDUMPPAUSE
		printf("press any key to continue!\n"); getch();
#endif
#ifdef FILEREAD
	}
#endif
#ifndef BUFFDUMPPAUSE
	printf("press any key to continue!\n"); getch();
#endif
#endif	//filereadload
	DebugMemory_(&gvar, 1);
	MM_DumpData(&gvar);
	MM_Report_(&gvar);
	//printf("bakapee1=%s\n", bakapee1);
	//printf("bakapee2=%s\n", bakapee2);
	MM_FreePtr(BBUFPTR, &gvar);
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
	printf("&bigb=%Fp ", BBUFPTR);
	//printf("&bigb=%04x", BBUFPTR);
	printf("\n");
#endif
	printf("========================================\n");

#ifdef EXMMVERBOSE__
	printf("coreleft():			%u\n", _memavl());
	printf("farcoreleft():			%lu\n", (dword)HC_farcoreleft());
#endif
#ifdef __WATCOMC__
//this is far	printf("Total free:			%lu\n", (dword)(HC_GetFreeSize()));
//super buggy	printf("HC_coreleft():			%u\n", HC_coreleft());
//	printf("HC_farcoreleft():			%lu\n", (dword)HC_farcoreleft());
	//printf("HC_GetNearFreeSize():		%u\n", HC_GetNearFreeSize());
	//printf("HC_GetFarFreeSize():			%lu\n", (dword)HC_GetFarFreeSize());
	HC_heapdump(&gvar);
//	segatesuto();
#endif
/*#ifdef __BORLANDC__
	//printf("core left:			%lu\n", (dword)HC_coreleft());
	//printf("far core left:			%lu\n", (dword)HC_farcoreleft());
//	printf("\nfarcoreleft():			%lu\n", farcoreleft());
#endif*/
	printf("Project 16 ");
#ifdef __WATCOMC__
	printf("exmmtest");
#endif
#ifdef __BORLANDC__
	printf("bcexmm");
#endif
	printf(".exe. This is just a test file!\n");
	printf("version %s\n", VERSION);
#if defined(__DEBUG__) && ( defined(__DEBUG_PM__) || defined(__DEBUG_MM__) )
#ifdef __DEBUG_MM__
	printf("debugmm: %u\t", dbg_debugmm);
#endif
#ifdef __DEBUG_PM__
	printf("debugpm: %u", dbg_ddebugpm);
#endif
	printf("\n");
#endif
	//printf("based core left:			%lu\n", (dword)_basedcoreleft());
	//printf("huge core left:			%lu\n", (dword)_hugecoreleft());
}
