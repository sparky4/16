/* midi.h
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

#ifndef	__MIDI__
#define	__MIDI__

#include <stdio.h>
#include <conio.h> /* this is where Open Watcom hides the outp() etc. functions */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>
#include <ctype.h>
#include <fcntl.h>
#include <math.h>
#include <dos.h>

//#include "src/lib/doslib/vga.h"
#include "src/lib/doslib/dos.h"
#include "src/lib/16_head.h"
#include "src/lib/doslib/8254.h"		/* 8254 timer */
#include "src/lib/doslib/8259.h"
//#include "src/lib/doslib/vgagui.h"
//#include "src/lib/doslib/vgatty.h"
#include "src/lib/doslib/adlib.h"

/* one per OPL channel */
struct midi_note {
	unsigned char		note_number;
	unsigned char		note_velocity;
	unsigned char		note_track;	/* from what MIDI track */
	unsigned char		note_channel;	/* from what MIDI channel */
	unsigned int		busy:1;		/* if occupied */
};

struct midi_channel {
	unsigned char		program;
};

struct midi_track {
	/* track data, raw */
	unsigned char*		raw;		/* raw data base */
	unsigned char*		fence;		/* raw data end (last byte + 1) */
	unsigned char*		read;		/* raw data read ptr */
	/* state */
	unsigned long		us_per_quarter_note; /* Microseconds per quarter note (def 120 BPM) */
	unsigned long		us_tick_cnt_mtpq; /* Microseconds advanced (up to 10000 us or one unit at 100Hz) x ticks per quarter note */
	unsigned long		wait;
	unsigned char		last_status;	/* MIDI last status byte */
	unsigned int		eof:1;		/* we hit the end of the track */
};

#define MIDI_MAX_CHANNELS	16
#define MIDI_MAX_TRACKS		64

extern struct midi_note		midi_notes[ADLIB_FM_VOICES];
extern struct midi_channel		midi_ch[MIDI_MAX_CHANNELS];
extern struct midi_track		midi_trk[MIDI_MAX_TRACKS];

static void (interrupt *old_irq0)();
static volatile unsigned long irq0_ticks=0;
static volatile unsigned int irq0_cnt=0,irq0_add=0,irq0_max=0;
static volatile unsigned char	midi_playing=0;

int load_midi_file(const char *path);
void interrupt irq0();
void adlib_shut_up();
void midi_reset_tracks();
void midi_reset_channels();
void midi_tick();

#endif /* __MIDI__ */
