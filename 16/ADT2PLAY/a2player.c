/* Output from p2c 1.21alpha-07.Dec.93, the Pascal-to-C translator */
/* From input file "a2player.pas" */


#include <p2c/p2c.h>


#define A2PLAYER_G
#include "a2player.h"


unsigned short opl3port = 0x388;
short error_code = 0;
uchar current_order = 0, current_pattern = 0, current_line = 0, tempo = 50,
      speed = 6;
unsigned short macro_speedup = 1;
boolean irq_mode = false;
uchar max_patterns = 128;
boolean fast_forward = false;
uchar overall_volume = 63, global_volume = 63;
unsigned short song_timer = 0, song_timer_tenths = 0;
Void (*external_irq_hook) PV() = NULL;
long _delay_counter = 0;
unsigned short irq_freq = 50;
boolean irq_initialized = false, timer_fix = true, pattern_break = false,
	pattern_delay = false;
uchar next_line = 0;
tPLAY_STATUS play_status = isStopped;
boolean replay_forbidden = true, force_macro_keyon = false;

#ifndef TIMERINT_H
#include "timerint.h"
#endif

#ifndef PARSERIO_H
#include "parserio.h"
#endif


typedef unsigned short tTRACK_ADDR[20];


#define keyoff_flag     0x80
#define fixed_note_flag  0x90
#define pattern_loop_flag  0xe0
#define pattern_break_flag  0xf0


typedef struct _REC_porta_table {
  unsigned short freq;
  uchar speed;
} _REC_porta_table;

typedef struct _REC_porta_table2 {
  unsigned short freq;
  uchar speed;
} _REC_porta_table2;

typedef struct _REC_arpgg_table {
  uchar state, note, add1, add2;
} _REC_arpgg_table;

typedef struct _REC_arpgg_table2 {
  uchar state, note, add1, add2;
} _REC_arpgg_table2;

typedef struct _REC_vibr_table {
  uchar pos, speed, depth;
  boolean fine;
} _REC_vibr_table;

typedef struct _REC_vibr_table2 {
  uchar pos, speed, depth;
  boolean fine;
} _REC_vibr_table2;

typedef struct _REC_trem_table {
  uchar pos, speed, depth;
  boolean fine;
} _REC_trem_table;

typedef struct _REC_trem_table2 {
  uchar pos, speed, depth;
  boolean fine;
} _REC_trem_table2;

typedef struct _REC_tremor_table {
  short pos;
  unsigned short volume;
} _REC_tremor_table;

typedef struct _REC_tremor_table2 {
  short pos;
  unsigned short volume;
} _REC_tremor_table2;

typedef struct _REC_macro_table {
  unsigned short fmreg_pos, arpg_pos, vib_pos;
  uchar fmreg_count, fmreg_duration, arpg_count, vib_count, vib_delay;
  boolean vib_paused;
  uchar fmreg_table, arpg_table, vib_table, arpg_note;
  unsigned short vib_freq;
} _REC_macro_table;


Static uchar _panning[3] = {
  0x30, 0x10, 0x20
};

Static uchar _instr[12] = {
  0x20, 0x20, 0x40, 0x40, 0x60, 0x60, 0x80, 0x80, 0xe0, 0xe0, 0xc0, 0xbd
};

/* 01 - 02 - 03 - 04 - 05 - 06 - 07 - 08 - 09 - 10 - 11 - 12 - 13 - 14 - 15 - 16 - 17 - 18 - 19 - 20 */

Static tTRACK_ADDR _chmm_n = {
  0x3, 0, 0x4, 0x1, 0x5, 0x2, 0x6, 0x7, 0x8, 0x103, 0x100, 0x104, 0x101,
  0x105, 0x102, 0x106, 0x107, 0x108, BYTE_NULL, BYTE_NULL
};

Static tTRACK_ADDR _chmm_m = {
  0x8, 0, 0x9, 0x1, 0xa, 0x2, 0x10, 0x11, 0x12, 0x108, 0x100, 0x109, 0x101,
  0x10a, 0x102, 0x110, 0x111, 0x112, BYTE_NULL, BYTE_NULL
};

Static tTRACK_ADDR _chmm_c = {
  0xb, 0x3, 0xc, 0x4, 0xd, 0x5, 0x13, 0x14, 0x15, 0x10b, 0x103, 0x10c, 0x104,
  0x10d, 0x105, 0x113, 0x114, 0x115, BYTE_NULL, BYTE_NULL
};

/*                                                                            BD   SD   TT   TC   HH */

Static tTRACK_ADDR _chpm_n = {
  0x3, 0, 0x4, 0x1, 0x5, 0x2, 0x106, 0x107, 0x108, 0x103, 0x100, 0x104, 0x101,
  0x105, 0x102, 0x6, 0x7, 0x8, 0x8, 0x7
};

Static tTRACK_ADDR _chpm_m = {
  0x8, 0, 0x9, 0x1, 0xa, 0x2, 0x110, 0x111, 0x112, 0x108, 0x100, 0x109, 0x101,
  0x10a, 0x102, 0x10, 0x14, 0x12, 0x15, 0x11
};

Static tTRACK_ADDR _chpm_c = {
  0xb, 0x3, 0xc, 0x4, 0xd, 0x5, 0x113, 0x114, 0x115, 0x10b, 0x103, 0x10c,
  0x104, 0x10d, 0x105, 0x13, BYTE_NULL, BYTE_NULL, BYTE_NULL, BYTE_NULL
};

Static tTRACK_ADDR _chan_n, _chan_m, _chan_c;
Static uchar def_vibtrem_speed_factor = 1, def_vibtrem_table_size = 32;

Static uchar def_vibtrem_table[256] = {
  0, 24, 49, 74, 97, 120, 141, 161, 180, 197, 212, 224, 235, 244, 250, 253,
  255, 253, 250, 244, 235, 224, 212, 197, 180, 161, 141, 120, 97, 74, 49, 24,
  0, 24, 49, 74, 97, 120, 141, 161, 180, 197, 212, 224, 235, 244, 250, 253,
  255, 253, 250, 244, 235, 224, 212, 197, 180, 161, 141, 120, 97, 74, 49, 24,
  0, 24, 49, 74, 97, 120, 141, 161, 180, 197, 212, 224, 235, 244, 250, 253,
  255, 253, 250, 244, 235, 224, 212, 197, 180, 161, 141, 120, 97, 74, 49, 24,
  0, 24, 49, 74, 97, 120, 141, 161, 180, 197, 212, 224, 235, 244, 250, 253,
  255, 253, 250, 244, 235, 224, 212, 197, 180, 161, 141, 120, 97, 74, 49, 24,
  0, 24, 49, 74, 97, 120, 141, 161, 180, 197, 212, 224, 235, 244, 250, 253,
  255, 253, 250, 244, 235, 224, 212, 197, 180, 161, 141, 120, 97, 74, 49, 24,
  0, 24, 49, 74, 97, 120, 141, 161, 180, 197, 212, 224, 235, 244, 250, 253,
  255, 253, 250, 244, 235, 224, 212, 197, 180, 161, 141, 120, 97, 74, 49, 24,
  0, 24, 49, 74, 97, 120, 141, 161, 180, 197, 212, 224, 235, 244, 250, 253,
  255, 253, 250, 244, 235, 224, 212, 197, 180, 161, 141, 120, 97, 74, 49, 24,
  0, 24, 49, 74, 97, 120, 141, 161, 180, 197, 212, 224, 235, 244, 250, 253,
  255, 253, 250, 244, 235, 224, 212, 197, 180, 161, 141, 120, 97, 74, 49, 24
/* p2c: a2player.pas, line 484: 
 * Note: Line breaker spent 0.0 seconds, 5000 tries on line 167 [251] */
};

Static uchar vibtrem_speed_factor, vibtrem_table_size;
Static uchar vibtrem_table[256];

Static tFM_PARAMETER_TABLE fmpar_table[20];
Static boolean volume_lock[20];
Static unsigned short volume_table[20];
Static unsigned short vscale_table[20];
Static boolean peak_lock[20];
Static boolean pan_lock[20];
Static tADTRACK2_EVENT event_table[20];
Static uchar voice_table[20];
Static uchar modulator_vol[20];
Static uchar carrier_vol[20];
Static unsigned short freq_table[20];
Static unsigned short effect_table[20];
Static unsigned short effect_table2[20];
Static uchar fslide_table[20];
Static uchar fslide_table2[20];
Static unsigned short glfsld_table[20];
Static unsigned short glfsld_table2[20];
Static _REC_porta_table porta_table[20];
Static _REC_porta_table2 porta_table2[20];
Static _REC_arpgg_table arpgg_table[20];
Static _REC_arpgg_table2 arpgg_table2[20];
Static _REC_vibr_table vibr_table[20];
Static _REC_vibr_table2 vibr_table2[20];
Static _REC_trem_table trem_table[20];
Static _REC_trem_table2 trem_table2[20];
Static uchar retrig_table[20];
Static uchar retrig_table2[20];
Static _REC_tremor_table tremor_table[20];
Static _REC_tremor_table2 tremor_table2[20];
Static uchar panning_table[20];
Static unsigned short last_effect[20];
Static unsigned short last_effect2[20];
Static uchar volslide_type[20];
Static boolean event_new[20];
Static uchar notedel_table[20];
Static uchar notecut_table[20];
Static short ftune_table[20];
Static boolean keyoff_loop[20];
Static _REC_macro_table macro_table[20];

Static uchar loopbck_table[20];
Static uchar loop_table[20][256];
Static uchar misc_register;
Static uchar current_tremolo_depth = 0, current_vibrato_depth = 0;

Static boolean speed_update, lockvol, panlock, lockVP;
Static uchar tremolo_depth, vibrato_depth;
Static boolean volume_scaling, percussion_mode;
Static uchar last_order;
Static boolean reset_chan[20];
/* p2c: a2player.pas, line 484: 
 * Warning: Expected BEGIN, found 'assembler' [227] */


extern Void opl2out PP((int reg, int data));
/* p2c: a2player.pas, line 485: Warning: Expected END, found 'asm' [227] */
/* p2c: a2player.pas, line 504: 
 * Warning: Expected a '.', found PROCEDURE [227] */
/* p2c: a2player.pas, line 504: 
 * Warning: Junk at end of input file ignored [277] */




/* End. */
