/* midi.c
 *
 * Adlib OPL2/OPL3 FM synthesizer chipset test program.
 * Play MIDI file using the OPLx synthesizer (well, poorly anyway)
 * (C) 2010-2012 Jonathan Campbell.
 * Hackipedia DOS library.
 *
 * This code is licensed under the LGPL.
 * <insert LGPL legal text here>
 *
 * Compiles for intended target environments:
 *   - MS-DOS [pure DOS mode, or Windows or OS/2 DOS Box]
 */

#include "src/lib/16_head.h"
#include "src/lib/16_tail.h"
#include "src/lib/16_pm.h"
#include "src/lib/16_ca.h"
#include "src/lib/16_mm.h"
#include "src/lib/16_hc.h"
#include "src/lib/16_dbg.h"
#include "src/lib/16_snd.h"

extern struct glob_game_vars	*ggvv;

void main(int argc,char **argv) {
	static global_game_variables_t gvar;
	unsigned long adv;
	int c;
#ifdef __DEBUG_CA__
	dbg_debugca=1;
#endif
#ifdef __DEBUG_MM_
	dbg_debugmm=1;
#endif
	ggvv=&gvar;
	StartupCAMMPM(&gvar);
	printf("ADLIB FM test program IMFPLAY\n");
	if (argc < 2) {
		printf("You must specify an IMF file to play\n"); ShutdownCAMMPM(&gvar);
		return;
	}

	SD_Initimf(&gvar);

	if (!SD_imf_load_music(argv[1], &gvar)) {
		printf("Failed to load IMF Music\n"); ShutdownCAMMPM(&gvar);
		return;
	}

	SD_StartupTimer(&gvar);

	printf("playing!\n");
	while (1) {
		_cli();
		adv = gvar.ca.sd.irq0_ticks - gvar.ca.sd.ptick;
		if (adv >= 100UL) adv = 100UL;
		gvar.ca.sd.ptick = gvar.ca.sd.irq0_ticks;
		_sti();

		while (adv != 0) {
			SD_imf_tick(&gvar);
			adv--;
		}

		if (kbhit()) {
			c = getch();
			if (c == 0) c = getch() << 8;

			if (c == 27) {
				break;
			}
		}
	}
//0000	printf("contents of the imf_music\n[\n%s\n]\n", gvar.ca.sd.imf_music);

	SD_imf_free_music(&gvar);
	SD_adlib_shut_up();
	shutdown_adlib();
	SD_ShutdownTimer();
	write_8254_system_timer(0);	/* back to normal 18.2Hz */
	ShutdownCAMMPM(&gvar);
}
