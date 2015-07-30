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

#include "src/16.h"

void
main(int argc, char *argv[])
{
	engi_stat_t engi_stat;
	const char *cpus;
	byte *dpal, *gpal;
	player_t player[MaxPlayers];

	engi_stat = ENGI_RUN;

	/* save the palette */
	dpal = modexNewPal();
	modexPalSave(dpal);
	modexFadeOff(4, dpal);
	//printf("pal load\n");
	//gpal = modexNewPal();
	/*modexPalSave(gpal);
	modexSavePalFile("data/g.pal", gpal);*/
	printf("wwww loop wwww\n");
	VGAmodeX(1);
	modexPalBlack();	//so player will not see loadings~
	IN_Startup();
	IN_Default(0,&player,ctrl_Joystick);
	while(ENGI_EXIT != engi_stat)
	{
		IN_ReadControl(0,&player);
		if(IN_KeyDown(sc_Escape)) engi_stat = ENGI_EXIT;
	}
	switch(detectcpu())
	{
		case 0: cpus = "8086/8088 or 186/88"; break;
		case 1: cpus = "286"; break;
		case 2: cpus = "386 or newer"; break;
		default: cpus = "internal error"; break;
	}
	printf("detected CPU type: %s\n", cpus);
	VGAmodeX(0);
	IN_Shutdown();
	modexFadeOn(4, dpal);
}
