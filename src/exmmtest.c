/* Project 16 Source Code~
 * Copyright (C) 2012-2016 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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
#include "src/lib/16_pm.h"
#include "src/lib/16_ca.h"
#include "src/lib/16_mm.h"
#include "src/lib/16_hc.h"
#include "src/lib/16_tail.h"
//#include "src/lib/modex16.h"
#pragma hdrstop

#pragma warn -pro
#pragma warn -use

//file load or read definition
#define FILEREAD
//#define EXMMVERBOSE

#define PRINTBB { printf("&main()=	%Fp\n", argv[0]);printf("bigbuffer=	%Fp\n", bigbuffer);printf("&bigbuffer=	%Fp\n", &bigbuffer);printf("bigbuffer=	%04x\n", bigbuffer);printf("&bigbuffer=	%04x\n", &bigbuffer); }

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
//static global_game_variables_t gvar;

void
main(int argc, char *argv[])
{
	byte w=1;
	static global_game_variables_t gvar;
#ifdef __WATCOMC__
	__segment sega;
#endif
	memptr bigbuffer;

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

	gvar.mm.mmstarted=0;
	dbg_debugpm=1;	//debug pm

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
#ifdef __WATCOMC__
	if(dbg_debugpm>0)
	{
		PM_Startup(&gvar);
		//printf("pmstarted ok\n");
		//PM_CheckMainMem(&gvar);
		PM_UnlockMainMem(&gvar);
	}
#endif
	CA_Startup(&gvar);
//	printf("		done!\n");
	//0000PRINTBB;
//	printf("press any key to continue!\n");
//	getch();
#ifdef FILEREAD
for(w=0;w<2;w++)
{
//	bakapeehandle = open(bakapee,O_RDONLY | O_BINARY, S_IREAD);
//	printf("size of big buffer~=%u\n", _bmsize(segu, bigbuffer));
//	if(CA_FarRead(bakapeehandle,(void far *)&bigbuffer,sizeof(bigbuffer),&gvar.mm))
	if(w>0)
	{
		printf("		read\n");
		if(CA_ReadFile(bakapee2, &bigbuffer, &gvar)) baka=1; else baka=0;
	}
#endif
	if(w==0)
	{
	printf("		load\n");
	if(CA_LoadFile(bakapee1, &bigbuffer, &gvar)) baka=1; else baka=0;
	}
//	close(bakapeehandle);
	//hmm functions in cache system use the buffered stuff
#ifdef __WATCOMC__
	printf("\nsize of big buffer~=%u\n", _bmsize(sega, bigbuffer));
#endif

	printf("contents of the buffer\n[\n%s\n]\n", bigbuffer);
	//printf("dark purple = purgable\n");
	//printf("medium blue = non purgable\n");
	//printf("red = locked\n");
	printf("press any key to continue!\n");
	DebugMemory_(&gvar, 1);
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
	MM_FreePtr(&bigbuffer, &gvar);
#ifdef __WATCOMC__
	if(dbg_debugpm>0)
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
	printf("bigb=	%Fp ", bigbuffer);
	//printf("bigbr=	%04x", bigbuffer);
	//printf("\n");
	printf("&bigb=%Fp ", &bigbuffer);
	//printf("&bigb=%04x", &bigbuffer);
	printf("\n");
#endif
	printf("========================================\n");
#ifdef __WATCOMC__
//this is far	printf("Total free:			%lu\n", (dword)(GetFreeSize()));
	printf("Total near free:		%lu\n", (dword)(GetNearFreeSize()));
	printf("Total far free:			%lu\n", (dword)(GetFarFreeSize()));
	heapdump(&gvar);
	segatesuto();
#endif
	printf("Project 16 emmtest.exe. This is just a test file!\n");
	printf("version %s\n", VERSION);
	//printf("core left:			%lu\n", (dword)_coreleft());
	//printf("far core left:			%lu\n", (dword)_farcoreleft());
	//printf("based core left:			%lu\n", (dword)_basedcoreleft());
	//printf("huge core left:			%lu\n", (dword)_hugecoreleft());
}
