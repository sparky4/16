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
	//extern struct inconfig inpu;
	static boolean testkeyin=true;
	static boolean testcontrolnoisy=true;
	IN_Startup();
	IN_Default(0,&player,ctrl_Joystick);
	//while(!IN_KeyDown(sc_Escape))
	while(!IN_KeyDown(sc_Escape))
	{
		IN_ReadControl(0,&player);
		//printf("%u\n", IN_KeyDown(sc_Escape));
		//printf("%u\n", IN_qb(sc_Escape));
	}
	IN_Shutdown();
	//printf("%u\n", in.Keyboard[sc_Escape]);
	printf("%u\n", testkeyin);
}
