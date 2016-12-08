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
 * 16 library
 */

#include "src/lib/16_tail.h"

/*
==========================
=
= Startup16
=
= Load a few things right away
=
==========================
*/

void Startup16(global_game_variables_t *gvar)
{
#ifdef __WATCOMC__
	start_timer(gvar);

	// DOSLIB: check our environment
	probe_dos();

	// DOSLIB: what CPU are we using?
	// NTS: I can see from the makefile Sparky4 intends this to run on 8088 by the -0 switch in CFLAGS.
	//      So this code by itself shouldn't care too much what CPU it's running on. Except that other
	//      parts of this project (DOSLIB itself) rely on CPU detection to know what is appropriate for
	//      the CPU to carry out tasks. --J.C.
	cpu_probe();

	// DOSLIB: check for VGA
	if (!probe_vga()) {
		printf("VGA probe failed\n");
		return;
	}
	// hardware must be VGA or higher!
	if (!(vga_state.vga_flags & VGA_IS_VGA)) {
		printf("This program requires VGA or higher graphics hardware\n");
		return;
	}

	if (_DEBUG_INIT() == 0) {
#ifdef DEBUGSERIAL
		printf("WARNING: Failed to initialize DEBUG output\n");
#endif
	}
	_DEBUG("Serial debug output started\n"); // NTS: All serial output must end messages with newline, or DOSBox-X will not emit text to log
	_DEBUGF("Serial debug output printf test %u %u %u\n",1U,2U,3U);
#endif
	gvar->mm.mmstarted=0;
	gvar->pm.PMStarted=0;
#ifdef __DEBUG__
	if(!dbg_nommpmca)
#endif
	MM_Startup(gvar);
if(!dbg_noplayerinpu)
	IN_Startup(gvar);
#ifdef __DEBUG__
	if(!dbg_nommpmca){
#endif
	PM_Startup(gvar);
	PM_UnlockMainMem(gvar);
	CA_Startup(gvar);
#ifdef __DEBUG__
	}
#endif
}

//===========================================================================

/*
==========================
=
= Shutdown16
=
= Shuts down all ID_?? managers
=
==========================
*/

void Shutdown16(global_game_variables_t *gvar)
{
#ifdef __DEBUG__
	if(!dbg_nommpmca)
#endif
	PM_Shutdown(gvar);
if(!dbg_noplayerinpu)
	IN_Shutdown(gvar);
#ifdef __DEBUG__
	if(!dbg_nommpmca){
#endif
	CA_Shutdown(gvar);
	MM_Shutdown(gvar);
#ifdef __DEBUG__
	}
#endif
}


//===========================================================================

/*
==================
=
= DebugMemory
=
==================
*/

void DebugMemory_(global_game_variables_t *gvar, boolean q)
{
	/*VW_FixRefreshBuffer ();
	US_CenterWindow (16,7);

	US_CPrint ("Memory Usage");
	US_CPrint ("------------");
	US_Print ("Total     :");
	US_PrintUnsigned (mminfo.mainmem/1024);
	US_Print ("k\nFree      :");
	US_PrintUnsigned (MM_UnusedMemory()/1024);
	US_Print ("k\nWith purge:");
	US_PrintUnsigned (MM_TotalFree()/1024);
	US_Print ("k\n");
	VW_UpdateScreen();*/
	if(q){
	printf("========================================\n");
	printf("		DebugMemory_\n");
	printf("========================================\n");}
	if(q) { printf("Memory Usage\n");
	printf("------------\n"); }else printf("	%c%c", 0xD3, 0xC4);
	printf("Total:	"); if(q) printf("	"); printf("%uk", gvar->mmi.mainmem/1024);
	if(q) printf("\n"); else printf("	");
	printf("Free:	"); if(q) printf("	"); printf("%uk", MM_UnusedMemory(gvar)/1024);
	if(q) printf("\n"); else printf("	");
	printf("With purge:"); if(q) printf("	"); printf("%uk\n", MM_TotalFree(gvar)/1024);
	if(q) printf("------------\n");
#ifdef __WATCOMC__
	IN_Ack ();
#endif
	if(q) MM_ShowMemory (gvar);
}
#ifdef __WATCOMC__
/*
==========================
=
= Quit
=
==========================
*/

void Quit (char *error)
{
	//unsigned        finscreen;
	memptr	screen=0;
	union REGS in, out;

	//ClearMemory ();
	if (!*error)
	{
	 //WriteConfig ();
	}
	else
	{
	 //CA_CacheGrChunk (ERRORSCREEN);
	 //screen = grsegs[ERRORSCREEN];
	}

	//ShutdownId ();
	IN_Shutdown();
	//modexLeave();
	in.h.ah = 0x00;
	in.h.al = 0x3;
	int86(0x10, &in, &out);

	if (error && *error)
	{
	  //movedata ((unsigned)screen,7,0xb800,0,7*160);
	  //gotoxy (10,4);
	  fprintf(stderr, "%s\n", error);
	  //gotoxy (1,8);
	  exit(1);
	}
	else
	if (!error || !(*error))
	{
		//clrscr();
		//#ifndef JAPAN
		movedata ((unsigned)screen,7,0xb800,0,4000);
		//gotoxy(1,24);
		//#endif
//asm	mov	bh,0
//asm	mov	dh,23	// row
//asm	mov	dl,0	// collumn
//asm	mov ah,2
//asm	int	0x10
	}

	exit(0);
}
#endif

//===========================================================================

const char *nibble_to_binary(nibble x)
{
	static char b[9];
	int z;

	b[0] = '\0';
	for (z = 8; z > 0; z >>= 1)
	{
		strcat(b, ((x & z) == z) ? "1" : "0");
	}
	return b;
}

const char *boolean_to_binary(boolean x)
{
	static char b[9];
	int z;

	b[0] = '\0';
	for (z = 1; z > 0; z >>= 1)
	{
		strcat(b, ((x & z) == z) ? "1" : "0");
	}
	return b;
}

void nibbletest()
{
	nibble pee;
	printf("nibbletest\n");
	/* nibble to binary string */
	for(pee=0;pee<18;pee++)
		printf("	%u %s\n", pee, nibble_to_binary(pee));
	printf("	sizeof(nibble)=%s\n", nibble_to_binary(sizeof(nibble)));
	printf("end of nibble test\n");
}

void booleantest()
{
	boolean pee;
	printf("booleantest\n");
	/* boolean to binary string */
	for(pee=0;pee<4;pee++)
		printf("	%u %s\n", pee, boolean_to_binary(pee));
	printf("	sizeof(boolean)=%s\n", boolean_to_binary(sizeof(boolean)));
	printf("end of boolean test\n");
}
