/* miditest.c
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

#include "src/lib/midi.h"

int main(int argc,char **argv) {
	unsigned long ptick;
	int i,c;

	printf("ADLIB FM test program\n");
	if (argc < 2) {
		printf("You must specify a MIDI file to play\n");
		return 1;
	}

	/*if (!probe_vga()) {
		printf("Cannot init VGA\n");
		return 1;
	}*/
	if (!init_adlib()) {
		printf("Cannot init library\n");
		return 1;
	}
	if (!probe_8254()) { // we need the timer to keep time with the music
		printf("8254 timer not found\n");
		return 1;
	}

	for (i=0;i < MIDI_MAX_TRACKS;i++) {
		midi_trk[i].raw = NULL;
		midi_trk[i].read = NULL;
		midi_trk[i].fence = NULL;
	}

	if (load_midi_file(argv[1]) == 0) {
		printf("Failed to load MIDI\n");
		return 1;
	}

	write_8254_system_timer(T8254_REF_CLOCK_HZ / 100); // tick faster at 100Hz please
	irq0_cnt = 0;
	irq0_add = 182;
	irq0_max = 1000; // about 18.2Hz
	old_irq0 = _dos_getvect(8);//IRQ0
	_dos_setvect(8,irq0);

	adlib_shut_up();
	midi_reset_channels();
	midi_reset_tracks();
	_cli();
	irq0_ticks = ptick = 0;
	_sti();
	midi_playing = 1;

	while (1) {
		unsigned long adv;

		_cli();
		adv = irq0_ticks - ptick;
		if (adv >= 100UL) adv = 100UL;
		ptick = irq0_ticks;
		_sti();

		while (adv != 0) {
			midi_tick();
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

	midi_playing = 0;
	adlib_shut_up();
	shutdown_adlib();
	_dos_setvect(8,old_irq0);
	write_8254_system_timer(0); // back to normal 18.2Hz

	for (i=0;i < MIDI_MAX_TRACKS;i++) {
		if (midi_trk[i].raw) {
#if TARGET_MSDOS == 16 && (defined(__LARGE__) || defined(__COMPACT__))
			_dos_freemem(FP_SEG(midi_trk[i].raw)); // NTS: Because we allocated with _dos_allocmem
#else
			free(midi_trk[i].raw);
#endif
			midi_trk[i].raw = NULL;
		}
		midi_trk[i].fence = NULL;
		midi_trk[i].read = NULL;
	}

	return 0;
}
