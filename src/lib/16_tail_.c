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
 * 16 tail library
 */

#include "src/lib/16_tail.h"
#include "src/lib/16_vl.h"

//===========================================================================

/*
====================
=
= TL_DosLibStartup
=
====================
*/

void TL_DosLibStartup(global_game_variables_t *gvar)
{
	if(gvar->DLStarted)
		return;

	// DOSLIB: check our environment
	probe_dos();

	// DOSLIB: what CPU are we using?
	// NTS: I can see from the makefile Sparky4 intends this to run on 8088 by the -0 switch in CFLAGS.
	//	  So this code by itself shouldn't care too much what CPU it's running on. Except that other
	//	  parts of this project (DOSLIB itself) rely on CPU detection to know what is appropriate for
	//	  the CPU to carry out tasks. --J.C.
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
		//printf("WARNING: Failed to initialize DEBUG output\n");
#endif
	}
	_DEBUG("Serial debug output started\n"); // NTS: All serial output must end messages with newline, or DOSBox-X will not emit text to log
	_DEBUGF("Serial debug output printf test %u %u %u\n",1U,2U,3U);

	textInit();
	gvar->DLStarted = true;
}

//===========================================================================

/*
====================
=
= TL_VidInit
=
====================
*/

void	TL_VidInit(global_game_variables_t *gvar)
{
	start_timer(gvar);	//i do not remeber why this is here wwww i think it should be somewhere else www

	if(!gvar->DLStarted) TL_DosLibStartup(gvar);

	// get old video mode
	//in.h.ah = 0xf;
	//int86(0x10, &in, &out);
	if(!gvar->video.old_mode) gvar->video.old_mode = VL_vgaGetMode();//out.h.al;
	gvar->video.VL_Initiated = 1;
}

//===========================================================================

/*
=============================================================================

						MUSIC STUFF

=============================================================================
*/


/*
=================
=
= StopMusic
=
=================
*/

void PL_StopMusic(global_game_variables_t *gvar)
{
	int	i;

	SD_MusicOff();
	for (i = 0;i < LASTMUSIC;i++)
		if (gvar->ca.audiosegs[STARTMUSIC + i])
		{
			MM_SetPurge(MEMPTRCONV gvar->ca.audiosegs[STARTMUSIC + i],3, gvar);
			MM_SetLock(MEMPTRCONV gvar->ca.audiosegs[STARTMUSIC + i],false, gvar);
		}
}

//==========================================================================


/*
=================
=
= StartMusic
=
=================
*/

void PL_StartMusic(global_game_variables_t *gvar)
{
	musicnames	chunk;

	SD_MusicOff();
	chunk = 0;//++++songs[gamestate.mapon+gamestate.episode*10];

//	if ((chunk == -1) || (MusicMode != smm_AdLib))
//DEBUG control panel		return;

//++++	MM_BombOnError (false,gvar);
//++++	CA_CacheAudioChunk(STARTMUSIC + chunk, gvar);
//++++	MM_BombOnError (true,gvar);
//++++	if (gvar->mm.mmerror)
//++++		gvar->mm.mmerror = false;
//++++	else
//++++	{
		MM_SetLock(MEMPTRCONV gvar->ca.audiosegs[STARTMUSIC + chunk],true, gvar);
		SD_StartMusic((MusicGroup far *)gvar->ca.audiosegs[STARTMUSIC + chunk]);
//++++	}
}
