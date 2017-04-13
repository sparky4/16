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
#define NOVID


#define KEYP IN_Shutdown(&gvar); printf("\n\npress any key to continue!\n"); getch(); IN_Startup(&gvar);

#define BBUFNAME gvar.mm.bufferseg//bigbuffer
//#define INITBBUF static memptr BBUFNAME;
#define BBUFPTR	MEMPTR BBUFNAME

#ifdef __BORLANDC__
#define BBUF		(memptr *)BBUFPTR
#define BBUFSTRING	(memptr *)BBUFNAME
#endif
#ifdef __WATCOMC__
#define BBUF		BBUFNAME
#define BBUFSTRING	BBUF
#endif

////////////////////////////////////////////////////////////////////////////
//#ifdef __BORLANDC__
void VL_Startup (global_game_variables_t *gvar){}
void VL_Shutdown (global_game_variables_t *gvar){}
void VGAmodeX(sword vq, boolean cmem, global_game_variables_t *gv)
{
	printf("VGAmodeX dummy:\n	%Fp	%Fp	%Fp\n", &vq, &cmem, gv);
}

word modexPalOverscan(word col)
{
	//modexWaitBorder();
	outp(PAL_WRITE_REG, 0);  /* start at the beginning of palette */
	outp(PAL_DATA_REG, col);
	return col;
}
void	TL_VidInit(global_game_variables_t *gvar)
{
	gvar->video.old_mode = 3;
}
//#endif

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
//	KEYP
}
#endif

//===========================================================================//

//=======================================//

//	main

//=======================================//
void
main(int argc, char *argv[])
{
	byte w;
								#ifndef NOVID
	boolean			done;
	ScanCode		scan;
								#endif
	static global_game_variables_t gvar;
								#ifdef INITBBUF
	INITBBUF
								#endif
#if 0
//#ifdef __WATCOMC__
	__segment sega;
#endif
	char bakapee1[64] = "data/koishi~.pcx";
	char bakapee2[64] = "data/test.map";
	word baka;

		#ifdef __BORLANDC__
			argc=argc;
		#endif

								#ifdef PRINTBBDUMP
								//0000PRINTBB; KEYP
								#endif
#ifdef __16_PM__
#ifdef __DEBUG_PM__
	dbg_debugpm=1;	//debug pm
#endif
#endif
#ifdef __DEBUG_CA__
	dbg_debugca=1;
#endif
#ifdef __DEBUG_MM__
	dbg_debugmm=1;
#endif

	if(argv[1]){ strcpy(bakapee1, argv[1]);//bakapee1[] = *argv[1];
	if(argv[2]) strcpy(bakapee2, argv[2]); }//bakapee2[] = argv[2]; }
#if 0
	else{
	//{ printf("filename!: "); scanf("%[^\n]", &bakapee); }
		strcpy(bakapee1, "data/koishi~.pcx"); //bakapee1 = "data/koishi~.pcx";
		strcpy(bakapee2, "data/test.map"); //bakapee2 = "data/test.map";
	}
#endif
	printf("bakapee1[%s]\n", bakapee1);
	printf("bakapee2[%s]\n", bakapee2);
	KEYP

								#ifndef NOVID
	Startup16(&gvar);
	// save the palette
								#ifdef __WATCOMC__
	modexPalSave(&gvar.video.dpal); modexFadeOff(4, &gvar.video.dpal); //modexPalBlack();
								#endif
								#else //NOVID
	//printf("main()=%Fp	start MM\n", *argv[0]);
	MM_Startup(&gvar);
								#ifdef __16_PM__
								#ifdef __DEBUG_PM__
									if(dbg_debugpm>0)
									{
								#endif //__DEBUG_PM__
	PM_Startup(&gvar); PM_CheckMainMem(&gvar); PM_UnlockMainMem(&gvar);
								//0000printf("PM Started\n"); KEYP
								#ifdef __DEBUG_PM__
									}
								#endif //__DEBUG_PM__
								#endif //__16_PM__
								#endif //elsed NOVID

	CA_Startup(&gvar);
								#ifdef PRINTBBDUMP
								//0000
PRINTBB; KEYP
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
								#endif //FILEREAD
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
	//	KEYP
	//	DebugMemory_(&gvar, 1);
		if(baka) printf("\nyay!\n");
		else printf("\npoo!\n");
								#ifdef BUFFDUMPPAUSE
		KEYP
								#endif
								#ifdef FILEREAD
	}
								#endif
								#endif	//filereadload

#ifdef __WATCOMC__
#ifndef NOVID
	VGAmodeX(1, 0, &gvar);
//	modexPalUpdate0(&gvar.video.palette);
//	ShapeTest_(&gvar);

	for (done = false;!done;)
	{
		while (!(scan = gvar.in.inst->LastScan))
		{}
	//			SD_Poll();

		IN_ClearKey(scan);
		switch (scan)
		{
//			case sc_Space:
//				MM_ShowMemory(&gvar);
//			break;
//#ifdef __WATCOMC__
			case sc_O:
				modexPalUpdate0(&gvar.video.palette); modexpdump(&gvar.video.page);
			break;
			case sc_P:
				modexpdump(&gvar.video.page[0]);
			break;
			case sc_V:
				VL_PatternDraw(&gvar.video, 0, 1, 1);
			break;
//#endif
			default:
			case sc_Escape:
				done = true;
			break;
		}
	}
	VGAmodeX(0, 0, &gvar);
#endif
#endif
//	MM_ShowMemory(&gvar);
	DebugMemory_(&gvar, 1);
	MM_DumpData(&gvar);
	MM_Report_(&gvar);
	//printf("bakapee1=%s\n", bakapee1);
	//printf("bakapee2=%s\n", bakapee2);
	MM_FreePtr(BBUFPTR, &gvar);
								#ifndef NOVID
	Shutdown16(&gvar);
								#else //novid
								#ifdef __16_PM__
								#ifdef __DEBUG_PM__
									if(dbg_debugpm>0)
								#endif //__DEBUG_PM__
	PM_Shutdown(&gvar);
								#endif //__16_PM__
	CA_Shutdown(&gvar);
	MM_Shutdown(&gvar);
								#endif //NOVID
	IN_Shutdown(&gvar);
	printf("========================================\n");
	printf("near=	%Fp ",	gvar.mm.nearheap);
	printf("far=	%Fp",			gvar.mm.farheap);
	printf("\n");
	printf("&near=	%Fp ",	&(gvar.mm.nearheap));
	printf("&far=	%Fp",		&(gvar.mm.farheap));
	printf("\n");
								#ifdef EXMMVERBOSE
	printf("bigb=	%Fp ",	BBUF);
	//printf("bigbr=	%04x",	BBUF);
	//printf("\n");
	printf("&bigb=%Fp ",		BBUFPTR);
	//printf("&bigb=%04x",		BBUFPTR);
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

//end of program


#if defined(__DEBUG__) && ( defined(__DEBUG_PM__) || defined(__DEBUG_MM__) )
#ifdef __DEBUG_MM__
	printf("debugmm: %u\t", dbg_debugmm);
#endif
#ifdef __DEBUG_PM__
	printf("debugpm: %u", dbg_debugpm);
#endif
	printf("\n");
#endif
	//printf("old_mode=%u	VL_Started=%u", gvar.video.old_mode, gvar.video.VL_Started);
	//printf("based core left:			%lu\n", (dword)_basedcoreleft());
	//printf("huge core left:			%lu\n", (dword)_hugecoreleft());
}
