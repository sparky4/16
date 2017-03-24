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
/*
	input test
*/
#include "src/lib/16_in_1.h"

void probe_dos(){}
void cpu_probe(){}
//int probe_vga(){ return 0; }
//void VGAmodeX(){}

void
main(int argc, char *argv[])
{
	static global_game_variables_t gvar;
	dbg_nointest=0;
	dbg_testkeyin=1;
	dbg_testcontrolnoisy=0;
	start_timer(&gvar);	printf(".");
	//Startup16(&gvar);
	IN_Startup(&gvar);	printf(".");
	//IN_Default(0,&gvar.player,ctrl_Joystick1);
	//IN_SetControlType(0,&gvar.player,ctrl_Joystick1);
	IN_Default(0,&gvar.player[0],ctrl_Keyboard1, &gvar);	printf(".");
	IN_SetControlType(&gvar.player[0],ctrl_Keyboard1);	printf(".");

	gvar.player[0].enti.q=1;
	gvar.player[0].enti.d=2;
	gvar.player[0].enti.speed=4;

//0000	nibbletest(){}
//0000	booleantest(){}
	//printf("dbg_testkeyin=%u	dbg_testcontrolnoisy=%u	dbg_nogvar.playerinpu=%u\nloop if this is not responsive then please KILL or reset machine sorry!!\n", dbg_testkeyin, dbg_testcontrolnoisy, dbg_nogvar.playerinpu);
	printf("\nloop\n");
	while(!IN_KeyDown(sc_Escape))
	{
//0000		shinkutxt(&gvar);
		IN_ReadControl(&gvar.player[0], &gvar);
/*		switch(gvar.player[0].enti.d)
		{
		//right movement
		case 3:
			if(gvar.player[0].enti.q<=(TILEWH/(gvar.player[0].enti.speed)))
			{
				gvar.player[0].enti.q++;
			} else { gvar.player[0].enti.q = 1; gvar.player[0].enti.d = 2; }
		break;

		//left movement
		case 1:
			if(gvar.player[0].enti.q<=(TILEWH/(gvar.player[0].enti.speed)))
			{
				gvar.player[0].enti.q++;
			} else { gvar.player[0].enti.q = 1; gvar.player[0].enti.d = 2; }
		break;

		//down movement
		case 4:
			if(gvar.player[0].enti.q<=(TILEWH/(gvar.player[0].enti.speed)))
			{
				gvar.player[0].enti.q++;
			} else { gvar.player[0].enti.q = 1; gvar.player[0].enti.d = 2; }
		break;

		//up movement
		case 0:
			if(gvar.player[0].enti.q<=(TILEWH/(gvar.player[0].enti.speed)))
			{
				gvar.player[0].enti.q++;
			} else { gvar.player[0].enti.q = 1; gvar.player[0].enti.d = 2; }
		break;
	}*/
		//printf("%u\n", IN_KeyDown(sc_Escape, &gvar));
		//if(gvar.in.Keyboard[sc_Escape])
//			printf("gvar.in.Keyboard[sc_Escape]=%u\n", gvar.in.Keyboard[sc_Escape]);
		//IN_qb(sc_Enter, &gvar);//>0) printf("IN_qb(sc_9)=%u\n", IN_qb(sc_9));
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
			//IN_Ack(){}
		}
	}
	IN_Shutdown(&gvar);
		//Shutdown16(&gvar);
	//printf("%u\n", in.Keyboard[sc_Escape]);
	printf("inputest.exe ");
	printf("version %s\n", VERSION);
	printf("testkeyin=%u\n", dbg_testkeyin);
	printf("testcontrolnoisy=%u\n", dbg_testcontrolnoisy);
}
