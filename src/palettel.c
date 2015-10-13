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

#include "src/lib/modex16.h"
#include "src/lib/bakapee.h"

global_game_variables_t gvar;
byte *pal;
char *bakapee;
word i;
page_t page;

void
main(int argc, char *argv[])
{
	page = modexDefaultPage();
	//page.width += 32;
	//page.height += 32;
	pal = modexNewPal();
	bakapee = malloc(64);
	if(argv[1]) bakapee = argv[1];
	else bakapee = "data/default.pal";
//	modexPalSave(pal);
	VGAmodeX(1, &gvar);
	modexPalBlack();

	modexLoadPalFile(bakapee, &pal);
	modexPalUpdate1(pal);
	modexFadeOn(1, pal);
	pdump(&page);
	getch();
	VGAmodeX(0, &gvar);
	for(i=0;i<768;i++)
	{
		printf("%02X ", pal[i]);
	}
}
