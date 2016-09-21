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

#include "src/lib/16_lib.h"

//===========================================================================

/*
==================
=
= DebugMemory
=
==================
*/

void DebugMemory (global_game_variables_t *gvar)
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
	IN_Ack ();
	MM_ShowMemory (gvar);
}

/*
==========================
=
= Quit
=
==========================
*/

void Quit (char *error)
{
	unsigned        finscreen;
	memptr	screen;
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
	  movedata ((unsigned)screen,7,0xb800,0,7*160);
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

//===========================================================================
