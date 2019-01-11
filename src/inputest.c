/* Project 16 Source Code~
 * Copyright (C) 2012-2019 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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
#include "src/lib/16_tail.h"

#define INPUTEST_JOYSTICKPADTEST
//#define INPUTEST_MOUSETEST

void
main(int argc, char *argv[])
{
	static global_game_variables_t gvar;
#ifdef __DEBUG_InputMgr__
	dbg_testkeyin=0;
	dbg_testcontrolnoisy=1;
	dbg_joymousedelta=0;
#endif
	start_timer(&gvar);
	//Startup16(&gvar);
	IN_Startup(&gvar);
	IN_Default(0,&gvar.player[0],
				#if !defined(INPUTEST_JOYSTICKPADTEST) && !defined(INPUTEST_MOUSETEST)
								ctrl_Keyboard1
				#endif
				#if defined(INPUTEST_JOYSTICKPADTEST) && !defined(INPUTEST_MOUSETEST)
								ctrl_Joystick1
				#endif
				#if !defined(INPUTEST_JOYSTICKPADTEST) && defined(INPUTEST_MOUSETEST)
								ctrl_Mouse
				#endif
												, &gvar);
	IN_SetControlType(&gvar.player[0],
				#if !defined(INPUTEST_JOYSTICKPADTEST) && !defined(INPUTEST_MOUSETEST)
								ctrl_Keyboard1
				#endif
				#if defined(INPUTEST_JOYSTICKPADTEST) && !defined(INPUTEST_MOUSETEST)
								ctrl_Joystick1
				#endif
				#if !defined(INPUTEST_JOYSTICKPADTEST) && defined(INPUTEST_MOUSETEST)
								ctrl_Mouse
				#endif
												);

	gvar.player[0].enti.q=1;
	gvar.player[0].enti.d=2;
	gvar.player[0].enti.spt=4;

	//printf("dbg_testkeyin=%u	dbg_testcontrolnoisy=%u	dbg_nogvar.playerinpu=%u\nloop if this is not responsive then please KILL or reset machine sorry!!\n", dbg_testkeyin, dbg_testcontrolnoisy, dbg_nogvar.playerinpu);
	while(!gvar.in.inst->Keyboard[sc_Escape])
	{
//0000		shinkutxt(&gvar);
		IN_ReadControl(&gvar.player[0], &gvar);
		switch(gvar.player[0].enti.d)
		{
		//right movement
		case 3:
			if(gvar.player[0].enti.q<=(TILEWH/(gvar.player[0].enti.spt)))
			{
				gvar.player[0].enti.q++;
			} else { gvar.player[0].enti.q = 1; gvar.player[0].enti.d = 2; }
		break;

		//left movement
		case 1:
			if(gvar.player[0].enti.q<=(TILEWH/(gvar.player[0].enti.spt)))
			{
				gvar.player[0].enti.q++;
			} else { gvar.player[0].enti.q = 1; gvar.player[0].enti.d = 2; }
		break;

		//down movement
		case 4:
			if(gvar.player[0].enti.q<=(TILEWH/(gvar.player[0].enti.spt)))
			{
				gvar.player[0].enti.q++;
			} else { gvar.player[0].enti.q = 1; gvar.player[0].enti.d = 2; }
		break;

		//up movement
		case 0:
			if(gvar.player[0].enti.q<=(TILEWH/(gvar.player[0].enti.spt)))
			{
				gvar.player[0].enti.q++;
			} else { gvar.player[0].enti.q = 1; gvar.player[0].enti.d = 2; }
		break;
	}
		//printf("%u\n", gvar.in.inst->Keyboard[sc_Escape]);
		//if(
		if(gvar.in.inst->Keyboard[88])	//speed
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
	IN_Shutdown(&gvar);
		//Shutdown16(&gvar);
	//printf("%u\n", in.Keyboard[sc_Escape]);
	printf("inputest.exe ");
	printf("version %s\n", VERSION);
#ifdef __DEBUG_InputMgr__
	printf("testkeyin=%u\n", dbg_testkeyin);
	printf("testcontrolnoisy=%u\n", dbg_testcontrolnoisy);
	printf("dbg_joymousedelta=%u\n", dbg_joymousedelta);
#endif
	printf("JoysPresent={%d,%d}\n", gvar.in.JoysPresent[0], gvar.in.JoysPresent[1]);
	printf("MousePresent=%d\n", gvar.in.MousePresent);
}
