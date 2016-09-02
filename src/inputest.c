/* Project 16 Source Code~
 * Copyright (C) 2012-2016 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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
	global_game_variables_t gvar;
	player_t player[MaxPlayers];
	//extern struct inconfig inpu;
	testkeyin=1;
	testcontrolnoisy=1;
	IN_Startup();
	IN_Default(0,&player,ctrl_Joystick1);
	//IN_SetControlType(0,&player,ctrl_Joystick1);
	//while(!IN_KeyDown(sc_Escape))
	player[0].q=1;
	player[0].d=2;
	player[0].persist_aniframe=0;
	player[0].speed=4;
	start_timer(&gvar);
	while(!IN_KeyDown(sc_Escape))
	{
		//shinkutxt(&gvar);
		IN_ReadControl(0,&player);
		#define INC_PER_FRAME if(player[0].q&1) player[0].persist_aniframe++; if(player[0].persist_aniframe>4) player[0].persist_aniframe = 1;
		switch(player[0].d)
		{
		//right movement
		case 3:
			if(player[0].q<=(TILEWH/(player[0].speed)))
			{
				INC_PER_FRAME;
				player[0].q++;
			} else { player[0].q = 1; player[0].d = 2; }
		break;

		//left movement
		case 1:
			if(player[0].q<=(TILEWH/(player[0].speed)))
			{
				INC_PER_FRAME;
				player[0].q++;
			} else { player[0].q = 1; player[0].d = 2; }
		break;

		//down movement
		case 4:
			if(player[0].q<=(TILEWH/(player[0].speed)))
			{
				INC_PER_FRAME;
				player[0].q++;
			} else { player[0].q = 1; player[0].d = 2; }
		break;

		//up movement
		case 0:
			if(player[0].q<=(TILEWH/(player[0].speed)))
			{
				INC_PER_FRAME;
				player[0].q++;
			} else { player[0].q = 1; player[0].d = 2; }
		break;
	}
		//printf("%u\n", IN_KeyDown(sc_Escape));
		//if(
		IN_qb(sc_9);//>0) printf("IN_qb(sc_9)=%u\n", IN_qb(sc_9));
		if(IN_KeyDown(88))	//speed
		{
			switch(gvar.kurokku.fpscap)
			{
				case 0:
					gvar.kurokku.fpscap=1;
				break;
				case 1:
					gvar.kurokku.fpscap=0;
				break;
			}
			//IN_Ack();
		}
	}
	IN_Shutdown();
	//printf("%u\n", in.Keyboard[sc_Escape]);
	printf("inputest.exe ");
	printf("version %s\n", VERSION);
	printf("testkeyin=%u\n", testkeyin);
	printf("testcontrolnoisy=%u\n", testcontrolnoisy);
}
