/* Project 16 Source Code~
 * Copyright (C) 2012-2023 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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
void main(int argc, char *argv[]) {
	static global_game_variables_t gvar;			char bakapee[64] = "data/default.pal";
	if(argv[1]) strcpy(bakapee, argv[1]); modexPalSave(&gvar.video.palette);// modexPalSave(&gvar.video.dpal); modexFadeOff(4, &gvar.video.dpal); modexPalBlack();

	TL_VidInit(&gvar);	VGAmodeX(1, 0, &gvar);
	modexSavePalFile(bakapee, &gvar.video.palette);
	PALPROGRAMSNEWPAGE
	VL_ShowPage(&gvar.video.page[0], 1, 0);
	modexpdump(0, &gvar);
	while(!kbhit()){ /*VL_modexPalScramble(&gvar.video.palette);*/ }
//	modexPalSave(&gvar.video.palette);
	VGAmodeX(0, 0, &gvar);
}
