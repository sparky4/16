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
	scroll16 library~
*/
#include "src/lib/scroll16.h"

/*void animatePlayer(player_t *player,word playnum, short scrolloffsetswitch, int ls, bitmap_t *bmp)
{
	sword dire=32; //direction
	sword qq; //scroll offset

	if(scrolloffsetswitch==0) qq = 0;
	else qq = ((player[playnum]->p)*player[playnum]->speed);
	switch (player[playnum]->dir)
	{
		case 0:
			//up
			dire*=player[playnum]->dir;
			player[playnum]->x=player[playnum]->x-4;
			player[playnum]->y=player[playnum]->y-qq-TILEWH;
		break;
		case 1:
			// right
			dire*=(player[playnum]->dir-2);
			player[playnum]->x=player[playnum]->x+qq-4;
			player[playnum]->y=player[playnum]->y-TILEWH;
		break;
		case 2:
		break;
		case 3:
			//down
			dire*=(player[playnum]->dir-2)
			player[playnum]->x=player[playnum]->x-4;
			player[playnum]->y=player[playnum]->y+qq-TILEWH;
		break;
		case 4:
			//left
			dire*=(player[playnum]->dir+2)
			player[playnum]->x=player[playnum]->x-qq-4;
			player[playnum]->y=player[playnum]->y-TILEWH;
		break;
	}
//	modexCopyPageRegion(dest->page, src->page, x-4, y-4, x-4, y-4, 28, 40);
//	if(2>ls && ls>=1) { modexDrawSpriteRegion(dest->page, x, y, 48, dire, 24, 32, bmp); }else
//	if(3>ls && ls>=2) { modexDrawSpriteRegion(dest->page, x, y, 24, dire, 24, 32, bmp); }else
//	if(4>ls && ls>=3) { modexDrawSpriteRegion(dest->page, x, y, 0, dire, 24, 32, bmp); }else
//	if(5>ls && ls>=4) { modexDrawSpriteRegion(dest->page, x, y, 24, dire, 24, 32, bmp); }
	//TODO: mask copy //modexCopyPageRegion(dest->page, src->page, x-4, y-4, x-4, y-4, 28, 40);
	//modexClearRegion(top->page, 66, 66, 2, 40, 0);
	//modexCopyPageRegion(dest->page, top->page, 66, 66, 66, 66, 2, 40);
	//turn this off if XT
	//XTif(detectcpu() > 0)
//	modexWaitBorder();
}*/
