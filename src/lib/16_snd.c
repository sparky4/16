/* Project 16 Source Code~
 * Copyright (C) 2012-2022 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

#include "src/lib/16_snd.h"

static	void interrupt	(*t0OldService)(void);

void opl2out(word reg, word data)
{
	__asm
	{
		mov     ax,reg
		mov     dx,word ptr [ADLIB_FM_ADDRESS]
		or      ah,ah
		jz      @@1
		add     dx,2
@@1:    out     dx,al
		mov     cx,6
@@2:    in      al,dx
		loop    @@2
		inc     dl
		mov     ax,data
		out     dx,al
		dec     dl
		mov     cx,36
@@3:    in      al,dx
		loop    @@3
	}
}

void opl3out(word reg, word data)
{
	__asm
	{
		mov     ax,reg
		mov     dx,word ptr [ADLIB_FM_ADDRESS]
		or      ah,ah
		jz      @@1
		add     dx,2
@@1:    out     dx,al
		inc     dl
		mov     ax,data
		out     dx,al
		dec     dl
		mov     cx,26
@@2:    in      al,dx
		loop    @@2
	}
}

void opl3exp(word data)
{
	__asm
	{
		mov     ax,data
		mov     dx,word ptr [ADLIB_FM_ADDRESS]
		add     dx,2
		out     dx,al
		mov     cx,6
@@1:    in      al,dx
		loop    @@1
		inc     dl
		mov     al,ah
		out     dx,al
		mov     cx,36
@@2:    in      al,dx
		loop    @@2
	}
}

/* Function: FMResest *******************************************************
*
*     Description:        quick and dirty sound card reset (zeros all
*                         registers).
*
*/
void FMReset(void/*int percusiveMode*/)
{
	int i;

	/* zero all registers */
	for(i = MIN_REGISTER; i < MAX_REGISTER+1; i++) opl2out(i, 0);

	/* allow FM chips to control the waveform of each operator */
	opl2out(0x01, 0x20);

	/* set rhythm enabled (6 melodic voices, 5 percussive) */
	opl2out(0xBD, 0x20);

	//FMSetPercusiveMode(percusiveMode);
} /* End of FMReset */

/* Function: FMKeyOff *******************************************************
*
*     Parameters:        voice - which voice to turn off.
*
*     Description:        turns off the specified voice.
*
*/
void FMKeyOff(int voice)
{
	int regNum;

	/* turn voice off */
	regNum = 0xB0 + voice % 11;//NUMVOICE;
	opl2out(regNum, 0x0E);
} /* End of FMKeyOff */

/* Function: FMKeyOn *******************************************************
*
*     Parameters:        voice - which voice to turn on.
*                         freq - its frequency (note).
*                         octave - its octave.
*
*     Description:        turns on a voice of specfied frequency and
*                         octave.
*
*/
void FMKeyOn(int voice, int freq, int octave)
{
	int regNum, tmp;

	regNum = 0xA0 + voice % 11;//NUMVOICE;
	opl2out(regNum, freq & 0xff);
	regNum = 0xB0 + voice % 11;//NUMVOICE;
	tmp = (freq >> 8) | (octave << 2) | 0x20;
	opl2out(regNum, tmp);
} /* End of FMKeyOn */

/* Function: FMSetVoice *****************************************************
*
*     Parameters:        voiceNum - which voice to set.
*                         ins - instrument to set voice.
*
*     Description:        sets the instrument of a voice.
*
*/
void FMSetVoice(int voiceNum, FMInstrument *ins){
	int opCellNum, cellOffset;

	voiceNum %= 11;//NUMVOICE;
	cellOffset = voiceNum % 3 + ((voiceNum / 3) << 3);

	/* set sound characteristic */
	opCellNum = 0x20 + (char)cellOffset;
	opl2out(opCellNum, ins->SoundCharacteristic[0]);
	opCellNum += 3;
	opl2out(opCellNum, ins->SoundCharacteristic[1]);

	/* set level/output */
	opCellNum = 0x40 + (char)cellOffset;
	opl2out(opCellNum, ins->Level[0]);
	opCellNum += 3;
	opl2out(opCellNum, ins->Level[1]);

	/* set Attack/Decay */
	opCellNum = 0x60 + (char)cellOffset;
	opl2out(opCellNum, ins->AttackDecay[0]);
	opCellNum += 3;
	opl2out(opCellNum, ins->AttackDecay[1]);

	/* set Sustain/Release */
	opCellNum = 0x80 + (char)cellOffset;
	opl2out(opCellNum, ins->SustainRelease[0]);
	opCellNum += 3;
	opl2out(opCellNum, ins->SustainRelease[1]);

	/* set Wave Select */
	opCellNum = 0xE0 + (char)cellOffset;
	opl2out(opCellNum, ins->WaveSelect[0]);
	opCellNum += 3;
	opl2out(opCellNum, ins->WaveSelect[1]);

	/* set Feedback/Selectivity */
	opCellNum = (byte)0xC0 + (byte)voiceNum;
	opl2out(opCellNum, ins->Feedback);
} /* End of FMSetVoice */


//newer sd


struct glob_game_vars	*ggvv;
// WARNING: subroutine call in interrupt handler. make sure you compile with -zu flag for large/compact memory models
void interrupt SDL_irq0()
{
	ggvv->ca.sd.irq0_ticks++;
	if ((ggvv->ca.sd.irq0_cnt += ggvv->ca.sd.irq0_add) >= ggvv->ca.sd.irq0_max) {
		ggvv->ca.sd.irq0_cnt -= ggvv->ca.sd.irq0_max;
		t0OldService();
	}
	else {
		p8259_OCW2(0,P8259_OCW2_NON_SPECIFIC_EOI);
	}
}

void SD_Initimf(global_game_variables_t *gvar)
{
	if (!init_adlib()) {
		printf("Cannot init library\n");
		return;
	}
	if (!probe_8254()) { /* we need the timer to keep time with the music */
		printf("8254 timer not found\n");
		return;
	}

	gvar->ca.sd.imf_delay_countdown=0;
	gvar->ca.sd.imf_music=
	gvar->ca.sd.imf_play_ptr=
	gvar->ca.sd.imf_music_end=NULL;

	SD_adlib_shut_up();
	shutdown_adlib_opl3(); // NTS: Apparently the music won't play otherwise
}

void SD_imf_reset_music(global_game_variables_t *gvar)
{
	gvar->ca.sd.imf_music = gvar->ca.sd.imf_play_ptr = gvar->ca.sd.imf_music_end = NULL;
	gvar->ca.sd.imf_delay_countdown = 0;
}

void SD_StartupTimer(global_game_variables_t *gvar)
{
	gvar->ca.sd.irq0_ticks=
	gvar->ca.sd.irq0_cnt = 0;
	gvar->ca.sd.irq0_add = 182;
	gvar->ca.sd.irq0_max = 1000; /* about 18.2Hz */
	gvar->ca.sd.tickrate = 700;

	write_8254_system_timer(T8254_REF_CLOCK_HZ / gvar->ca.sd.tickrate);
	t0OldService = _dos_getvect(8);	/*IRQ0*/
	_dos_setvect(8,SDL_irq0);

	_cli();
	gvar->ca.sd.irq0_ticks = gvar->ca.sd.ptick = 0;
	_sti();
}

void SD_ShutdownTimer()
{
	_dos_setvect(8,t0OldService);
}

void SD_imf_free_music(global_game_variables_t *gvar)
{
#ifndef SD_USESCAMMPM
	if (gvar->ca.sd.imf_music) free(gvar->ca.sd.imf_music);
#else
	MM_FreePtr(MEMPTRCONV gvar->ca.audiosegs[0], gvar);	//TODO make behave like id engine
#endif
	SD_imf_reset_music(gvar);
}

int SD_imf_load_music(const char *path, global_game_variables_t *gvar)
{
	unsigned long len;
	unsigned char buf[8];
	int fd;

#ifndef SD_USESCAMMPM
	SD_imf_free_music(gvar);
#else
	SD_imf_reset_music(gvar);
#endif

	fd = open(path,O_RDONLY|O_BINARY);
	if (fd < 0) return 0;

	len = lseek(fd,0,SEEK_END);
	lseek(fd,0,SEEK_SET);
	read(fd,buf,2);
	if (buf[0] != 0 || buf[1] != 0) // type 1 IMF
		len = *((uint16_t*)buf);
	else
		lseek(fd,0,SEEK_SET);

	if (len == 0 || len > 65535UL) {
		close(fd);
		return 0;
	}
	len -= len & 3;

#ifndef SD_USESCAMMPM
	gvar->ca.sd.imf_music = malloc(len);
#else
	MM_GetPtr(MEMPTRCONV gvar->ca.audiosegs[0],len, gvar);
	gvar->ca.sd.imf_music = (struct imf_entry *)gvar->ca.audiosegs[0];
#endif
	if (gvar->ca.sd.imf_music == NULL) {
		close(fd);
		return 0;
	}
	read(fd,gvar->ca.sd.imf_music,len);
	close(fd);

	gvar->ca.sd.imf_play_ptr = gvar->ca.sd.imf_music;
	gvar->ca.sd.imf_music_end = gvar->ca.sd.imf_music + (len >> 2UL);
	return 1;
}

void SD_imf_tick(global_game_variables_t *gvar)
{
	if (gvar->ca.sd.imf_delay_countdown == 0) {
		do {
			adlib_write(gvar->ca.sd.imf_play_ptr->reg,gvar->ca.sd.imf_play_ptr->data);
			gvar->ca.sd.imf_delay_countdown = gvar->ca.sd.imf_play_ptr->delay;
			gvar->ca.sd.imf_play_ptr++;
			if (gvar->ca.sd.imf_play_ptr == gvar->ca.sd.imf_music_end)
			{
//				printf("replay\n");
				gvar->ca.sd.imf_play_ptr = gvar->ca.sd.imf_music;
			}
		} while (gvar->ca.sd.imf_delay_countdown == 0);
	}
	else {
		gvar->ca.sd.imf_delay_countdown--;
	}
}

void SD_adlib_shut_up() {
	int i;

	memset(adlib_fm,0,sizeof(adlib_fm));
	memset(&adlib_reg_bd,0,sizeof(adlib_reg_bd));
	for (i=0;i < adlib_fm_voices;i++) {
		struct adlib_fm_operator *f;
		f = &adlib_fm[i].mod;
		f->ch_a = f->ch_b = f->ch_c = f->ch_d = 1;
		f = &adlib_fm[i].car;
		f->ch_a = f->ch_b = f->ch_c = f->ch_d = 1;
	}

	for (i=0;i < adlib_fm_voices;i++) {
		struct adlib_fm_operator *f;

		f = &adlib_fm[i].mod;
		f->mod_multiple = 1;
		f->total_level = 63 - 16;
		f->attack_rate = 15;
		f->decay_rate = 4;
		f->sustain_level = 0;
		f->release_rate = 8;
		f->f_number = 400;
		f->sustain = 1;
		f->octave = 4;
		f->key_on = 0;

		f = &adlib_fm[i].car;
		f->mod_multiple = 1;
		f->total_level = 63 - 16;
		f->attack_rate = 15;
		f->decay_rate = 4;
		f->sustain_level = 0;
		f->release_rate = 8;
		f->f_number = 0;
		f->sustain = 1;
		f->octave = 0;
		f->key_on = 0;
	}

	adlib_apply_all();
}
