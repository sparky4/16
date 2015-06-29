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
/*
	input test
*/
#include "src/lib/16_in.h"

void
main(int argc, char *argv[])
{
	player_t player[MaxPlayers];
	inconfig in;
	//word i=0;

	in.KbdDefs[0].button0 = 0x1d;
	in.KbdDefs[0].button1 = 0x38;
	//in.KbdDefs[0].upleft = 0x47;
	in.KbdDefs[0].up = 0x48;
	//in.KbdDefs[0].upright = 0x49;
	in.KbdDefs[0].left = 0x4b;
	in.KbdDefs[0].right = 0x4d;
	//in.KbdDefs[0].downleft = 0x4f;
	in.KbdDefs[0].down = 0x50;
	//in.KbdDefs[0].downright = 0x51;

	if(IN_Startup(&in))
	{
		IN_Default(0,&player,ctrl_Joystick, &in);
		while(!in.Keyboard[sc_Escape])
		{
			IN_ReadControl(0,&player, &in);
			//printf("%u\n", in.Keyboard[sc_Escape]);
			//printf("i=%u\n", i);
			printf("");
			//i++;
		}
		IN_Shutdown(&in);
	}
	//printf("%u\n", in.Keyboard[sc_Escape]);
}
