/* Project 16 Source Code~
 * Copyright (C) 2012-2015 sparky4 & pngwen & andrius4669
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
#include "src/lib/16_snd.h"

void main(int argc, char near *argv[])
{
	static FMInstrument testInst =
{
0x00, 0x01,	/* modulator frequency multiple... 0x20 */
0x00, 0x00,	/* modulator frequency level...    0x40 */
0xF0, 0xF0,	/* modulator attack/decay...       0x60 */
0x73, 0x73,	/* modulator sustain/release...    0x80 */
0x03, 0x00,	/* output waveform distortion      0xE0 */
0x36,				/* feedback algorithm and strength 0xC0 */
};

	IN_Startup();
	FMReset();
	FMSetVoice(0, &testInst);
	printf("p");
	while(!IN_qb(1))
	{
		if(IN_qb(44))
		{
			printf("e");
			FMKeyOn(0, 0x106, 4);
		}
		else
		{
			FMKeyOff(0);
		}
	}
	printf("!\n");
	IN_Shutdown();
}