/* Project 16 Source Code~
 * Copyright (C) 2012-2018 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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
#include "src/exmmtest.h"

////////////////////////////////////////////////////////////////////////////
#ifdef NOVID
void VL_Startup (global_game_variables_t *gvar){ gvar=gvar; }
void VL_Shutdown (global_game_variables_t *gvar){ gvar=gvar; }
void VGAmodeX(sword vq, boolean cmem, global_game_variables_t *gv)
{
	printf("VGAmodeX dummy:\n	%Fp	%Fp	%Fp\n", &vq, &cmem, gv);
}

word
VL_modexPalOverscan(byte *p, word col)
{
	int i;
	//modexWaitBorder();
//	vga_wait_for_vsync();
	outp(PAL_WRITE_REG, 0);  /* start at the beginning of palette */
	for(i=col; i<(3+col); i++)
	{
		outp(PAL_DATA_REG, p[i]);
	}
//	modexPalSave(p);
	return col;
}
void	TL_VidInit(global_game_variables_t *gvar)
{
	gvar->video.old_mode = 3;
}

void VL_print(const byte *str, nibble pagenum, global_game_variables_t *gvar){ printf("%s\n", str); }

#ifdef __WATCOMC__
void VL_ShowPage(page_t *page, boolean vsync, boolean sr){}
void modexClearRegion(page_t *page, int x, int y, int w, int h, byte color){}
void modexprint(page_t *page, sword x, sword y, word t, boolean tlsw, word color, word bgcolor, boolean vidsw, const byte *str){ printf("%s\n", str); }
void modexpdump(nibble pagenum, global_game_variables_t *gvar){}
#endif
#endif

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

#ifdef SCROLLLOAD
#define FILENAME_1	"data/spri/chikyuu.vrs"
#define FILENAME_2	"data/test.map"
#else
#define FILENAME_1	"data/spri/chikyuu.sht"
#define FILENAME_2	"data/test.map"
#endif

//===========================================================================//

//=======================================//

//	main

//=======================================//
void
main(int argc, char *argv[])
{
	static global_game_variables_t gvar;
								#ifdef INITBBUF
	INITBBUF
								#endif

	char bakapee1[64] = FILENAME_1;
	char bakapee2[64] = FILENAME_2;

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

	printf("bakapee1[%s]\n", bakapee1);
	printf("bakapee2[%s]\n", bakapee2);
								#ifdef EXMMVERBOSE__
	printf("coreleft():		%u\n", coreleft());
	printf("farcoreleft():		%ld\n", farcoreleft());
								#endif
	printf("stackavail()=%u\n", stackavail());
	KEYP

								#ifndef NOVID
	Startup16(&gvar);
	// save the palette
	modexPalSave(&gvar.video.dpal); //modexFadeOff(4, &gvar.video.dpal); //modexPalBlack();
								#else //NOVID
	StartupCAMMPM(&gvar);
								#endif //elsed NOVID
								#ifdef PRINTBBDUMP
								//0000
PRINTBB; KEYP
								#endif

	IN_Default(0,&gvar.player[0],ctrl_Keyboard1, &gvar);
	IN_SetControlType(&gvar.player[0],ctrl_Keyboard1);

	{
	byte w;	word baka;
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
								#ifdef BUFFDUMP
		{
			size_t file_s;
			FILE *fh;

			if(!w)	fh = fopen(bakapee1, "r");
			else	fh = fopen(bakapee2, "r");
			file_s = filesize(fh);
			fclose(fh);
		printf("contents of the buffer\n[\n%.*s\n]\n", file_s, BBUFSTRING);
#if 0
//0000
// 			mmblocktype far *scan;
// 			scan = gvar.mm.mmhead;
// 			while (scan->useptr != &BBUFNAME && scan)
// 			{
// 				scan = scan->next;
// 			}
// 			printf("\n	%Fp	%Fp\n", scan->useptr, &BBUFNAME);
			printf("\nstrlen of buffer = %zu\n", strlen(BBUFSTRING));
			printf("length of buffer = %zu\n", file_s);
//			printf("length of buffer = %lu\n", scan->length);
#endif
		}
								#endif
								#ifdef PRINTBBDUMP
		PRINTBB;
								#endif

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
	}

							#ifdef SCROLLLOAD
	CA_loadmap(bakapee2, &gvar.map, &gvar);	//BREAKS!
//	newloadmap(bakapee2, &gvar.map);
	VRS_LoadVRS(bakapee1, &gvar.player[0].enti, &gvar);
//	HC_heapdump(&gvar);
	KEYP
							#endif	//scrollload

#ifndef NOVID
	VGAmodeX(8, 0, &gvar); VL_LoadPalFileCore(&gvar.video.palette, &gvar);
	VL_ClearVideo (0);
	modexHiganbanaPageSetup(&gvar);
	gvar.video.page[0].dx = gvar.video.page[0].dy = 0;
//	VL_modexPalScramble(&gvar.video.palette);
	{
		unsigned temp;
		temp = BDOFSCONV gvar.video.BOFS;
		gvar.video.BOFS = gvar.video.DOFS;
		MU_IntroScreen(&gvar);
		gvar.video.BOFS = (byte __far *)temp;
	}
//	ShapeTest_(&gvar);
#endif
	MM_ShowMemory(&gvar);
#if 0
	{
	boolean			done;
	ScanCode		scan;
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
				VL_modexPalScramble(&gvar.video.palette); modexpdump(&gvar.video.page);
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
}
#endif
#ifndef NOVID
	if(gvar.video.VL_Started)
		VL_Shutdown (&gvar);//VGAmodeX(0, 0, gvar);
	modexFadeOn(4, gvar.video.dpal);
#endif
	DebugMemory_(&gvar, 1);
	MM_DumpData(&gvar);
	KEYP
	MM_Report_(&gvar);
	//printf("bakapee1=%s\n", bakapee1);
	//printf("bakapee2=%s\n", bakapee2);
	MM_FreePtr(BBUFPTR, &gvar);
								#ifndef NOVID
	Shutdown16(&gvar);
								#else //novid
	ShutdownCAMMPM(&gvar);
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
	printf("coreleft():		%u\n", coreleft());
	printf("farcoreleft():		%ld\n", farcoreleft());
								#endif
#ifdef __WATCOMC__
//this is far	printf("Total free:			%lu\n", (dword)(HC_GetFreeSize()));
//super buggy	printf("HC_coreleft():			%u\n", HC_coreleft());
//	printf("HC_farcoreleft():			%lu\n", (dword)HC_farcoreleft());
	//printf("HC_GetNearFreeSize():		%u\n", HC_GetNearFreeSize());
	//printf("HC_GetFarFreeSize():			%lu\n", (dword)HC_GetFarFreeSize());
//	segatesuto();
#endif
#ifdef __BORLANDC__
//	printf("HC_coreleft:			%lu\n", (dword)HC_coreleft());
//	printf("HC_farcoreleft:			%lu\n", (dword)HC_farcoreleft());
//	printf("HC_Newfarcoreleft():		%lu\n", (dword)HC_Newfarcoreleft());
#endif
	HC_heapdump(&gvar);
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
//	printf("curr_mode=%u\n", gvar.video.curr_mode);
//	VL_PrintmodexmemInfo(&gvar.video);
	//printf("old_mode=%u	VL_Started=%u", gvar.video.old_mode, gvar.video.VL_Started);
	//printf("based core left:			%lu\n", (dword)_basedcoreleft());
	//printf("huge core left:			%lu\n", (dword)_hugecoreleft());
}
