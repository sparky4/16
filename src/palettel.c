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

#include "src/lib/16_vl.h"
#include "src/lib/bakapee.h"

char *bakapee;
word i;

void
main(int argc, char *argv[])
{
	static global_game_variables_t gvar;
	bakapee = malloc(64);
	if(argv[1]) bakapee = argv[1];
	else bakapee = "data/default.pal";
	modexPalSave(&(gvar.video.palette));
	VGAmodeX(1, 1, &gvar);
	gvar.video.page[0] = modexDefaultPage(&gvar.video.page[0]);
	modexPalBlack();

	modexLoadPalFile(bakapee, &(gvar.video.palette));
	modexPalUpdate(&(gvar.video.palette));
	modexFadeOn(1, &(gvar.video.palette));
	modexpdump(&gvar.video.page[0]);
	getch();
	VGAmodeX(0, 0, &gvar);
	modexPalUpdate(&(gvar.video.palette));
	/*for(i=0;i<768;i++)
	{
		printf("%02X ", pal[i]);
	}*/
	free(bakapee);
}
