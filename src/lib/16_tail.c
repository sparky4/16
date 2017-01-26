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
#include "src/lib/16text.h"

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
		//printf("WARNING: Failed to initialize DEBUG output\n");
#endif
	}
	_DEBUG("Serial debug output started\n"); // NTS: All serial output must end messages with newline, or DOSBox-X will not emit text to log
	_DEBUGF("Serial debug output printf test %u %u %u\n",1U,2U,3U);

	textInit();
#endif
	gvar->mm.mmstarted=0;
	gvar->pm.PMStarted=0;
	MM_Startup(gvar);
#ifdef __WATCOMC__
#ifdef __DEBUG_InputMgr__
	if(!dbg_nointest)
#endif
	IN_Startup(gvar);
#endif
	PM_Startup(gvar);
	PM_UnlockMainMem(gvar);
	CA_Startup(gvar);
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
	PM_Shutdown(gvar);
#ifdef __WATCOMC__
#ifdef __DEBUG_InputMgr__
	if(!dbg_nointest)
#endif
	IN_Shutdown(gvar);
#endif
	CA_Shutdown(gvar);
	MM_Shutdown(gvar);
}


//===========================================================================

/*
====================
=
= ReadConfig
=
====================
*/

/*void ReadConfig(void)
{
	int                     file;
	SDMode          sd;
	SMMode          sm;
	SDSMode         sds;


	if ( (file = open(configname,O_BINARY | O_RDONLY)) != -1)
	{
	//
	// valid config file
	//
		read(file,Scores,sizeof(HighScore) * MaxScores);

		read(file,&sd,sizeof(sd));
		read(file,&sm,sizeof(sm));
		read(file,&sds,sizeof(sds));

		read(file,&mouseenabled,sizeof(mouseenabled));
		read(file,&joystickenabled,sizeof(joystickenabled));
		read(file,&joypadenabled,sizeof(joypadenabled));
		read(file,&joystickprogressive,sizeof(joystickprogressive));
		read(file,&joystickport,sizeof(joystickport));

		read(file,&dirscan,sizeof(dirscan));
		read(file,&buttonscan,sizeof(buttonscan));
		read(file,&buttonmouse,sizeof(buttonmouse));
		read(file,&buttonjoy,sizeof(buttonjoy));

		read(file,&viewsize,sizeof(viewsize));
		read(file,&mouseadjustment,sizeof(mouseadjustment));

		close(file);

		if (sd == sdm_AdLib && !AdLibPresent && !SoundBlasterPresent)
		{
			sd = sdm_PC;
			sd = smm_Off;
		}

		if ((sds == sds_SoundBlaster && !SoundBlasterPresent) ||
			(sds == sds_SoundSource && !SoundSourcePresent))
			sds = sds_Off;

		if (!MousePresent)
			mouseenabled = false;
		if (!JoysPresent[joystickport])
			joystickenabled = false;

		MainMenu[6].active=1;
		MainItems.curpos=0;
	}
	else
	{
	//
	// no config file, so select by hardware
	//
		if (SoundBlasterPresent || AdLibPresent)
		{
			sd = sdm_AdLib;
			sm = smm_AdLib;
		}
		else
		{
			sd = sdm_PC;
			sm = smm_Off;
		}

		if (SoundBlasterPresent)
			sds = sds_SoundBlaster;
		else if (SoundSourcePresent)
			sds = sds_SoundSource;
		else
			sds = sds_Off;

		if (MousePresent)
			mouseenabled = true;

		joystickenabled = false;
		joypadenabled = false;
		joystickport = 0;
		joystickprogressive = false;

		viewsize = 15;
		mouseadjustment=5;
	}

	SD_SetMusicMode (sm);
	SD_SetSoundMode (sd);
	SD_SetDigiDevice (sds);
}*/


/*
====================
=
= WriteConfig
=
====================
*/

/*void WriteConfig(void)
{
	int                     file;

	file = open(configname,O_CREAT | O_BINARY | O_WRONLY,
				S_IREAD | S_IWRITE | S_IFREG);

	if (file != -1)
	{
		write(file,Scores,sizeof(HighScore) * MaxScores);

		write(file,&SoundMode,sizeof(SoundMode));
		write(file,&MusicMode,sizeof(MusicMode));
		write(file,&DigiMode,sizeof(DigiMode));

		write(file,&mouseenabled,sizeof(mouseenabled));
		write(file,&joystickenabled,sizeof(joystickenabled));
		write(file,&joypadenabled,sizeof(joypadenabled));
		write(file,&joystickprogressive,sizeof(joystickprogressive));
		write(file,&joystickport,sizeof(joystickport));

		write(file,&dirscan,sizeof(dirscan));
		write(file,&buttonscan,sizeof(buttonscan));
		write(file,&buttonmouse,sizeof(buttonmouse));
		write(file,&buttonjoy,sizeof(buttonjoy));

		write(file,&viewsize,sizeof(viewsize));
		write(file,&mouseadjustment,sizeof(mouseadjustment));

		close(file);
	}
}*/

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
	//IN_Ack ();
#endif
	if(q) MM_ShowMemory (gvar);
}

/*
==========================
=
= ClearMemory
=
==========================
*/

void ClearMemory (global_game_variables_t *gvar)
{
	PM_UnlockMainMem(gvar);
	//snd
	MM_SortMem (gvar);
}

/*
==========================
=
= Quit
=
==========================
*/

void Quit (global_game_variables_t *gvar, char *error)
{
	//unsigned        finscreen;
	memptr	screen=0;

	ClearMemory (gvar);
	if (!*error)
	{
// #ifndef JAPAN
// 		CA_CacheGrChunk (ORDERSCREEN);
// 		screen = grsegs[ORDERSCREEN];
// #endif
// 		WriteConfig ();
	}
	else
	{
// 		CA_CacheGrChunk (ERRORSCREEN);
// 		screen = grsegs[ERRORSCREEN];
	}
	Shutdown16(gvar);

	if (error && *error)
	{
		movedata((unsigned)screen,7,0xb800,0,7*160);
		gotoxy (10,4);
		fprintf(stderr, "%s\n", error);
		gotoxy (1,8);
		exit(1);
	}
	else
	if (!error || !(*error))
	{
		clrscr();
#ifndef JAPAN
		movedata ((unsigned)screen,7,0xb800,0,4000);
		gotoxy(1,24);
#endif
//asm	mov	bh,0
//asm	mov	dh,23	// row
//asm	mov	dl,0	// collumn
//asm	mov ah,2
//asm	int	0x10
	}

	exit(0);
}

//===========================================================================

void walktypeinfo(player_t *player, word pn)
{
	switch(player[pn].walktype)
	{
		case 0:	//bump
			sprintf(global_temp_status_text, "%s", "b");
		break;
		case 1:	//walk no scroll
			sprintf(global_temp_status_text, "%s", "w");
		break;
		case 2:	//scrolling walk
			sprintf(global_temp_status_text, "%s", "s");
		break;
	}
}

//
// for mary4 (XT)
// this is from my XT's BIOS
// http://www.phatcode.net/downloads.php?id=101
//
void turboXT(byte bakapee)
{
	//byte bakapee = 12;
	__asm {
	push	ax
	push	bx
	push	cx
	in	al, 61h 			; Read equipment flags
	xor	al, bakapee			;   toggle speed
	out	61h, al 			; Write new flags back

	mov	bx, 0F89h			; low pitch blip
	and	al, 4				; Is turbo mode set?
	jz	@@do_beep
	mov	bx, 52Eh			; high pitch blip

@@do_beep:
	mov	al, 10110110b		; Timer IC 8253 square waves
	out	43h, al 			;   channel 2, speaker
	mov	ax, bx
	out	42h, al 			;   send low order
	mov	al, ah				;   load high order
	out	42h, al 			;   send high order
	in	al, 61h 			; Read IC 8255 machine status
	push	ax
	or	al, 00000011b
	out	61h, al 			; Turn speaker on
	mov	cx, 2000h
@@delay:
	loop    @@delay
	pop	ax
	out	61h, al 			; Turn speaker off
	pop	cx
	pop	bx
	pop	ax
	}
}

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
