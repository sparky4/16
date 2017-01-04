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

#include "src/16.h"

engi_stat_t engi_stat;
byte *dpal, *gpal;
player_t player[MaxPlayers];

void
main(int argc, char *argv[])
{
	static global_game_variables_t gvar;
	Startup16(&gvar);

	engi_stat = ENGI_RUN;

	/* save the palette */
	dpal = modexNewPal();
	modexPalSave(dpal);
	modexFadeOff(4, dpal);
	gpal = modexNewPal();
	modexPalSave(gpal);
	modexSavePalFile("data/g.pal", gpal);
	VGAmodeX(1, 1, &gvar);
//	modexPalBlack();	//so player will not see loadings~
	IN_Default(0,&player,ctrl_Joystick);
	//modexprint(&screen, 32, 32, 1, 2, 0, "a", 1);
	while(ENGI_EXIT != engi_stat)
	{
		IN_ReadControl(0,&player);
		if(IN_KeyDown(sc_Escape)) engi_stat = ENGI_EXIT;
		shinku(&gvar);
		_DEBUGF("Serial debug output printf test %u %u %u\n",1U,2U,3U);
	}
	Shutdown16(&gvar);
	VGAmodeX(0, 1, &gvar);
	printf("Project 16 16.exe. This is supposed to be the actual finished game executable!\n");
	printf("version %s\n", VERSION);
	WCPU_cpufpumesg();
	modexFadeOn(4, dpal);
}
