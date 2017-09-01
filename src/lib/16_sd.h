/* Project 16 Source Code~
 * Copyright (C) 2012-2017 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

#ifndef __16_SD_H_
#define __16_SD_H_

#include "src/lib/16_head.h"
#include "src/lib/16_tail.h"
#include <hw/8254/8254.h>		/* 8254 timer */
#include <hw/adlib/adlib.h>
#include <hw/vga/vga.h>
#include <hw/dos/dos.h>
#include <hw/8259/8259.h>

#define SD_USESCAMMPM

#define MIN_REGISTER			0x01
#define MAX_REGISTER			0xF5
#define ADLIB_FM_ADDRESS	0x388	/* adlib address/status register */
#define ADLIB_FM_DATA		0x389	/* adlib data register           */

/*
* FM Instrument definition for .SBI files - SoundBlaster instrument
* - these are the important parts - we will skip the header, but since
*   I am not sure where it starts and ends so I have had to guess.
*   However it SEEMS! to work. Each array has two values, one for
*   each operator.
*/
typedef struct{
	byte SoundCharacteristic[2];    /* modulator frequency multiple...  */
	byte Level[2];                  /* modulator frequency level...     */
	byte AttackDecay[2];            /* modulator attack/decay...        */
	byte SustainRelease[2];         /* modulator sustain/release...     */
	byte WaveSelect[2];             /* output waveform distortion       */
	byte Feedback;                  /* feedback algorithm and strength  */
} FMInstrument;

void opl2out(word reg, word data);
void opl3out(word reg, word data);
void opl3exp(word data);

void FMReset(void/*int percusiveMode*/);
void FMKeyOff(int voice);
void FMKeyOn(int voice, int freq, int octave);
void FMSetVoice(int voiceNum, FMInstrument *ins);

void SD_Initimf(global_game_variables_t *gvar);
void SD_imf_reset_music(global_game_variables_t *gvar);
void	SD_StartupTimer(global_game_variables_t *gvar),
		SD_ShutdownTimer();
void SD_imf_free_music(global_game_variables_t *gvar);
int SD_imf_load_music(const char *path, global_game_variables_t *gvar);
void interrupt SD_irq0(void);
void SD_imf_tick(global_game_variables_t *gvar);
void SD_adlib_shut_up();

#endif /*__16_SND_H_*/
