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
//	CursorInfo control;
	player_t player[MaxPlayers];
	hwconfig hw;
	player.KbdDefs = {0x1d,0x38,0x47,0x48,0x49,0x4b,0x4d,0x4f,0x50,0x51};

	extern boolean Keyboard[NumCodes];
	IN_Startup();
	IN_Default(0,&player,ctrl_Joystick);
//	IN_Default(0,ctrl_Joystick);
	while(!IN_qb(sc_Escape))
	{
		IN_ReadControl(0,&control);
//		printf("%u\n", Keyboard[sc_Escape]);

	}
	IN_Shutdown();
}
