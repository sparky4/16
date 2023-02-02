/* Project 16 Source Code~
 * Copyright (C) 2012-2023 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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
	simple mm test
*/
#include "src/exmmtest.h"

////////////////////////////////////////////////////////////////////////////

#ifdef BBUFNAME
#undef BBUFNAME
#endif
#define BBUFNAME bufferseg

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
	static memptr BBUFNAME;

	char bakapee1[64] = FILENAME_1;
	char bakapee2[64] = FILENAME_2;

		#ifdef __BORLANDC__
			argc=argc;
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

	StartupCAMMPM(&gvar);
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

//0000++++
	MM_ShowMemory(&gvar);

	DebugMemory_(&gvar, 1);
	MM_DumpData(&gvar);
	KEYP
	MM_Report_(&gvar);
	//printf("bakapee1=%s\n", bakapee1);
	//printf("bakapee2=%s\n", bakapee2);
	MM_FreePtr(BBUFPTR, &gvar);
	ShutdownCAMMPM(&gvar);
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

	HC_heapdump(&gvar);
	printf("Project 16 ");
#ifdef __WATCOMC__
	printf("mmtest");
#endif
#ifdef __BORLANDC__
	printf("bcmm");
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
