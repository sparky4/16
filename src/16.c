/* Project 16 Source Code~
 * Copyright (C) 2012-2020 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

#include "src/16.h"

void
main(int argc, char *argv[])
{
	static global_game_variables_t gvar;
	Startup16(&gvar);//initgame equ

	/* save the palette */
	modexPalSave(gvar.video.dpal);
	modexFadeOff(4, gvar.video.dpal);
	modexPalSave(gvar.video.palette);
	modexSavePalFile("data/g.pal", gvar.video.palette);
	VGAmodeX(1, 1, &gvar);
//	modexPalBlack();	//so player will not see loadings~
	IN_Default(0,&gvar.player[0],ctrl_Joystick, &gvar);
	//modexprint(&screen, 32, 32, 1, 2, 0, "a", 1);
	while(1)
	{
		IN_ReadControl(&gvar.player[0], &gvar);
		if(gvar.in.inst->Keyboard[sc_Escape]) break;
		shinku(&gvar);
		_DEBUGF("Serial debug output printf test %u %u %u\n",1U,2U,3U);
	}
	Shutdown16(&gvar);
	VGAmodeX(0, 1, &gvar);
	printf("Project 16 16.exe. This is supposed to be the actual finished game executable!\n");
	printf("version %s\n", VERSION);
	WCPU_cpufpumesg();
	modexFadeOn(4, gvar.video.dpal);
//	InitGame ();//to be defined in 16_tail

//++++	DemoLoop();//to be defined in 16_tail

//++++	Quit(&gvar, "Demo loop exited???");

}
