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

#include <stdio.h>

#include "src/lib/16_in.h"
//#include "src/lib/16_snd.h"
#include "src/lib/doslib/adlib.h"
#include "src/lib/doslib/8254.h"		/* 8254 timer */

static unsigned int musical_scale[18] = {
	0x1B0,			/* E */
	0x1CA,			/* F */
	0x1E5,			/* f# */
	0x202,			/* G */
	0x220,			/* G# */
	0x241,			/* A */
	0x263,			/* A# */
	0x287,			/* B */
	0x2AE,			/* C */

	0x2B0,			/* E */
	0x2CA,			/* F */
	0x2E5,			/* f# */
	0x302,			/* G */
	0x320,			/* G# */
	0x341,			/* A */
	0x363,			/* A# */
	0x387,			/* B */
	0x3AE,			/* C */
};

void main(int argc, char near *argv[])
{
	word i;
//	static FMInstrument testInst =
//{
//0x00, 0x01,	/* modulator frequency multiple... 0x20 */
//0x00, 0x00,	/* modulator frequency level...    0x40 */
//0xF0, 0xF0,	/* modulator attack/decay...       0x60 */
//0x73, 0x73,	/* modulator sustain/release...    0x80 */
//0x03, 0x00,	/* output waveform distortion      0xE0 */
//0x36,				/* feedback algorithm and strength 0xC0 */
//};

	IN_Startup();
	//FMReset();
	//FMSetVoice(0, &testInst);
	if(!init_adlib())
	{
		printf("Cannot init library\n");
		exit(-5);
	}

	if (adlib_fm_voices > 9)
		printf("OPL3!\n");
//		vga_bios_set_80x50_text();

	memset(adlib_fm,0,sizeof(adlib_fm));
	memset(&adlib_reg_bd,0,sizeof(adlib_reg_bd));
	for (i=0;i < adlib_fm_voices;i++) {
		struct adlib_fm_operator *f;
		f = &adlib_fm[i].mod;
		f->ch_a = f->ch_b = f->ch_c = f->ch_d = 1;
		f = &adlib_fm[i].car;
		f->ch_a = f->ch_b = f->ch_c = f->ch_d = 1;
	}

	for (i=0;i < /*adlib_fm_voices*/1;i++) {
		struct adlib_fm_operator *f;

		f = &adlib_fm[i].mod;
		f->mod_multiple = 1;
		f->total_level = 63 - 16;
		f->attack_rate = 15;
		f->decay_rate = 0;
		f->sustain_level = 7;
		f->release_rate = 7;
		f->f_number = musical_scale[i%18];
		f->octave = 4;
		f->key_on = 0;

		f = &adlib_fm[i].car;
		f->mod_multiple = 1;
		f->total_level = 63 - 16;
		f->attack_rate = 15;
		f->decay_rate = 0;
		f->sustain_level = 7;
		f->release_rate = 7;
		f->f_number = 0;
		f->octave = 0;
		f->key_on = 0;
	}

	adlib_apply_all();

	printf("press Z!  to noise\npress ESC to quit");
	printf("p");
	while(!IN_qb(1))
	{
		if(IN_qb(44))
		{
			printf("e");
			adlib_fm[0].mod.key_on = 1;
			//FMKeyOn(0, 0x106, 4);
		}
		else
		{
			adlib_fm[0].mod.key_on = 0;
			//FMKeyOff(0);
		}
		adlib_update_groupA0(0,&adlib_fm[0]);
	}
	printf("!\n");
	shutdown_adlib();
	IN_Shutdown();
}
