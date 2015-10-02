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

void walk(map_view_t *pip, player_t *player, word pn)
{
	#define INC_PER_FRAME if(player[pn].q&1) player[pn].persist_aniframe++; if(player[pn].persist_aniframe>4) player[pn].persist_aniframe = 1;
	switch(player[pn].d)
	{
		//right movement
		case 3:
			if(pip[0].tx >= 0 && pip[0].tx+20 < pip[0].map->width && player[pn].tx == pip[0].tx + 10 &&
			!(pip[0].map->data[(player[pn].tx)+(pip[0].map->width*(player[pn].ty-1))] == 0))//!(player[pn].tx+1 == TRIGGX && player[pn].ty == TRIGGY))	//collision detection!
			{
				if(player[pn].q<=(TILEWH/(player[pn].speed)))
				{
					INC_PER_FRAME;
					animatePlayer(pip, player, pn, 1);
					mapScrollRight(pip, player, 0, pn);
					mapScrollRight(pip, player, 1, pn);
					modexShowPage(pip[1].page);
					player[pn].q++;
				} else { player[pn].q = 1; player[pn].d = 2; player[pn].tx++; }
			}
			else if(player[pn].tx < pip[0].map->width && !(pip[0].map->data[(player[pn].tx)+(pip[0].map->width*(player[pn].ty-1))] == 0))//!(player[pn].tx+1 == TRIGGX && player[pn].ty == TRIGGY))
			{
				if(player[pn].q<=(TILEWH/(player[pn].speed)))
				{
					INC_PER_FRAME;
					player[pn].x+=(player[pn].speed);
					animatePlayer(pip, player, pn, 0);
					modexShowPage(pip[1].page);
					player[pn].q++;
				} else { player[pn].q = 1; player[pn].d = 2; player[pn].tx++; }
			}
			else
			{
				modexCopyPageRegion(pip[1].page, pip[0].page, player[pn].x-4, player[pn].y-TILEWH, player[pn].x-4, player[pn].y-TILEWH, 24, 32);
#ifdef SPRITE
				modexDrawSpriteRegion(pip[1].page, player[pn].x-4, player[pn].y-TILEWH, 24, 32, 24, 32, &player[pn].data);
#else
				modexClearRegion(pip[1].page, player[pn].x-4, player[pn].y-TILEWH, 24, 32, 14);
#endif
				modexShowPage(pip[1].page);
				player[pn].d = 2;
			}
			player[pn].triggerx = player[pn].tx+1;
			player[pn].triggery = player[pn].ty;
		break;

		//left movement
		case 1:
			if(pip[0].tx > 0 && pip[0].tx+20 <= pip[0].map->width && player[pn].tx == pip[0].tx + 10 &&
			!(pip[0].map->data[(player[pn].tx-2)+(pip[0].map->width*(player[pn].ty-1))] == 0))//!(player[pn].tx-1 == TRIGGX && player[pn].ty == TRIGGY))	//collision detection!
			{
				if(player[pn].q<=(TILEWH/(player[pn].speed)))
				{
					INC_PER_FRAME;
					animatePlayer(pip, player, pn, 1);
					mapScrollLeft(pip, player, 0, pn);
					mapScrollLeft(pip, player, 1, pn);
					modexShowPage(pip[1].page);
					player[pn].q++;
				} else { player[pn].q = 1; player[pn].d = 2; player[pn].tx--; }
			}
			else if(player[pn].tx > 1 && !(pip[0].map->data[(player[pn].tx-2)+(pip[0].map->width*(player[pn].ty-1))] == 0))//!(player[pn].tx-1 == TRIGGX && player[pn].ty == TRIGGY))
			{
				if(player[pn].q<=(TILEWH/(player[pn].speed)))
				{
					INC_PER_FRAME;
					player[pn].x-=(player[pn].speed);
					animatePlayer(pip, player, pn, 0);
					modexShowPage(pip[1].page);
					player[pn].q++;
				} else { player[pn].q = 1; player[pn].d = 2; player[pn].tx--; }
			}
			else
			{
				modexCopyPageRegion(pip[1].page, pip[0].page, player[pn].x-4, player[pn].y-TILEWH, player[pn].x-4, player[pn].y-TILEWH, 24, 32);
#ifdef SPRITE
				modexDrawSpriteRegion(pip[1].page, player[pn].x-4, player[pn].y-TILEWH, 24, 96, 24, 32, &player[pn].data);
#else
				modexClearRegion(pip[1].page, player[pn].x-4, player[pn].y-TILEWH, 24, 32, 10);
#endif
				modexShowPage(pip[1].page);
				player[pn].d = 2;
			}
			player[pn].triggerx = player[pn].tx-1;
			player[pn].triggery = player[pn].ty;
		break;

		//down movement
		case 4:
			if(pip[0].ty >= 0 && pip[0].ty+15 < pip[0].map->height && player[pn].ty == pip[0].ty + 8 &&
			!(pip[0].map->data[(player[pn].tx-1)+(pip[0].map->width*(player[pn].ty))] == 0))//!(player[pn].tx == TRIGGX && player[pn].ty+1 == TRIGGY))	//collision detection!
			{
				if(player[pn].q<=(TILEWH/(player[pn].speed)))
				{
					INC_PER_FRAME;
					animatePlayer(pip, player, pn, 1);
					mapScrollDown(pip, player, 0, pn);
					mapScrollDown(pip, player, 1, pn);
					modexShowPage(pip[1].page);
					player[pn].q++;
				} else { player[pn].q = 1; player[pn].d = 2; player[pn].ty++; }
			}
			else if(player[pn].ty < pip[0].map->height && !(pip[0].map->data[(player[pn].tx-1)+(pip[0].map->width*(player[pn].ty))] == 0))//!(player[pn].tx == TRIGGX && player[pn].ty+1 == TRIGGY))
			{
				if(player[pn].q<=(TILEWH/(player[pn].speed)))
				{
					INC_PER_FRAME;
					player[pn].y+=(player[pn].speed);
					animatePlayer(pip, player, pn, 0);
					modexShowPage(pip[1].page);
					player[pn].q++;
				} else { player[pn].q = 1; player[pn].d = 2; player[pn].ty++; }
			}
			else
			{
				modexCopyPageRegion(pip[1].page, pip[0].page, player[pn].x-4, player[pn].y-TILEWH, player[pn].x-4, player[pn].y-TILEWH, 24, 32);
#ifdef SPRITE
				modexDrawSpriteRegion(pip[1].page, player[pn].x-4, player[pn].y-TILEWH, 24, 64, 24, 32, &player[pn].data);
#else
				modexClearRegion(pip[1].page, player[pn].x-4, player[pn].y-TILEWH, 24, 32, 9);
#endif
				modexShowPage(pip[1].page);
				player[pn].d = 2;
			}
			player[pn].triggerx = player[pn].tx;
			player[pn].triggery = player[pn].ty+1;
		break;

		//up movement
		case 0:
			if(pip[0].ty > 0 && pip[0].ty+15 <= pip[0].map->height && player[pn].ty == pip[0].ty + 8 &&
			!(pip[0].map->data[(player[pn].tx-1)+(pip[0].map->width*(player[pn].ty-2))] == 0))//!(player[pn].tx == TRIGGX && player[pn].ty-1 == TRIGGY))	//collision detection!
			{
				if(player[pn].q<=(TILEWH/(player[pn].speed)))
				{
					INC_PER_FRAME;
					animatePlayer(pip, player, pn, 1);
					mapScrollUp(pip, player, 0, pn);
					mapScrollUp(pip, player, 1, pn);
					modexShowPage(pip[1].page);
					player[pn].q++;
				} else { player[pn].q = 1; player[pn].d = 2; player[pn].ty--; }
			}
			else if(player[pn].ty > 1 && !(pip[0].map->data[(player[pn].tx-1)+(pip[0].map->width*(player[pn].ty-2))] == 0))//!(player[pn].tx == TRIGGX &&  player[pn].ty-1 == TRIGGY))
			{
				if(player[pn].q<=(TILEWH/(player[pn].speed)))
				{
					INC_PER_FRAME;
					player[pn].y-=(player[pn].speed);
					animatePlayer(pip, player, 0, pn);
					modexShowPage(pip[1].page);
					player[pn].q++;
				} else { player[pn].q = 1; player[pn].d = 2; player[pn].ty--; }
			}
			else
			{
				modexCopyPageRegion(pip[1].page, pip[0].page, player[pn].x-4, player[pn].y-TILEWH, player[pn].x-4, player[pn].y-TILEWH, 24, 32);
#ifdef SPRITE
				modexDrawSpriteRegion(pip[1].page, player[pn].x-4, player[pn].y-TILEWH, 24, 0, 24, 32, &player[pn].data);
#else
				modexClearRegion(pip[1].page, player[pn].x-4, player[pn].y-TILEWH, 24, 32, 12);
#endif
				modexShowPage(pip[1].page);
				player[pn].d = 2;
			}
			player[pn].triggerx = player[pn].tx;
			player[pn].triggery = player[pn].ty-1;
		break;
	}
}

/*map_t
allocMap(int w, int h) {
	map_t result;

	result.width =w;
	result.height=h;
	result.data = malloc(sizeof(byte) * w * h);
	//result.data = (byte *)alloc_emem(((int)sizeof(byte) * w * h)/1024);
	if(isEMS() || checkEMS())
	{
		XMOVE mm;
		//emmhandle = mallocEMS(coretotalEMS());//alloc_emem((int)sizeof(map))
		mm.length=sizeof(result);
		mm.sourceH=0;
		mm.sourceOff=ptr2long(&result);
		mm.destH=emmhandle;
		mm.destOff=0;
		ist = move_emem(&mm);
		if(!ist){ dealloc_emem(emmhandle); exit(5); }
		printf("%d\n", coretotalEMS());
	}

	return result;
}*/

/*void
initMap(map_t *map) {
	// just a place holder to fill out an alternating pattern
	int x, y, xx, yy;
	int i, q;
//	int tile = 1;
	//if(!isEMS() || !checkEMS())
//		map->tiles = malloc(sizeof(tiles_t));
	//else
	//	map->tiles = (tiles_t *)alloc_emem(sizeof(tiles_t));

	 //create the tile set
	//if(!isEMS() || !checkEMS())
//		map->tiles->data = malloc(sizeof(bitmap_t));
	//else
	//	map->tiles->data = (bitmap_t *)alloc_emem(sizeof(bitmap_t));
//	map->tiles->data->width = (TILEWH);
//	map->tiles->data->height= TILEWH;
	//if(!isEMS() || !checkEMS())
//		map->tiles->data->data = malloc((TILEWH*2)*TILEWH);
	//else
	//	map->tiles->data->data = (byte *)alloc_emem((TILEWH*2)*TILEWH);
//	map->tiles->tileHeight = TILEWH;
//	map->tiles->tileWidth =TILEWH;
//	map->tiles->rows = 1;
//	map->tiles->cols = 1;//2;

	q=0;
	//for(y=0; y<map->height; y++) {
	//for(x=0; x<map->width; x++) {
	i=0;
	for(yy=0; yy<TILEWH; yy++) {
	for(xx=0; xx<(TILEWH); xx++) {
		//if(x<TILEWH){
		  map->tiles->data->data[i+1] = map->data[q];//28;//0x24;
//		  printf("[%d]", map->tiles->data->data[i]);
		//}else{
		  //map->tiles->data->data[i] = map->data[q];//0;//0x34;
		  //printf("]%d[==[%d]", i, map->tiles->data->data[i]);
		//}
		i++;
	}
//	printf("\n");
	}
//	printf("[%d]", map->data[q]);
	q++;
//	}
	//printf("\n\n");
//	}

	i=0;
	for(y=0; y<map->height; y++) {
		for(x=0; x<map->width; x++) {
//			map->data[i]=255;
			printf("[%d]", map->data[i]);
			//tile = tile ? 0 : 1;
			i++;
		}
		//tile = tile ? 0 : 1;
	}
}*/

void near mapScrollRight(map_view_t *mv, player_t *player, word id, word plid)
{
	word x, y;  /* coordinate for drawing */

	/* increment the pixel position and update the page */
	mv[id].page->dx += player[plid].speed;

	/* check to see if this changes the tile */
	if(mv[id].page->dx >= mv[id].dxThresh )
	{
	/* go forward one tile */
	mv[id].tx++;
	/* Snap the origin forward */
	mv[id].page->data += 4;
	mv[id].page->dx = mv[id].map->tiles->tileWidth;
	}

	/* draw the next column */
	x= SCREEN_WIDTH + mv[id].map->tiles->tileWidth;
	if(player[plid].q%4)
		if(id==0)
			mapDrawCol(&mv[0], mv[0].tx + 20 , mv[0].ty-1, x, player, mv->page->dx);
		else
			modexCopyPageRegion(mv[id].page, mv[0].page, x, 0, x, 0, mv[id].map->tiles->tileWidth, mv[id].map->tiles->tileHeight*17);
	//}
}


void near mapScrollLeft(map_view_t *mv, player_t *player, word id, word plid)
{
	word x, y;  /* coordinate for drawing */

	/* increment the pixel position and update the page */
	mv[id].page->dx -= player[plid].speed;

	/* check to see if this changes the tile */
	if(mv[id].page->dx == 0)
	{
	/* go backward one tile */
	mv[id].tx--;

	/* Snap the origin backward */
	mv[id].page->data -= 4;
	mv[id].page->dx = mv[id].map->tiles->tileWidth;
	}

	/* draw the next column */
	x= 0;
	if(player[plid].q%4)
		if(id==0)
			mapDrawCol(&mv[0], mv[0].tx - 1, mv[0].ty-1, x, player, mv->page->dx);
		else
			modexCopyPageRegion(mv[id].page, mv[0].page, x, 0, x, 0, mv[id].map->tiles->tileWidth, mv[id].map->tiles->tileHeight*17);
	//}
}


void near mapScrollUp(map_view_t *mv, player_t *player, word id, word plid)
{
	word x, y;  /* coordinate for drawing */

	/* increment the pixel position and update the page */
	mv[id].page->dy -= player[plid].speed;

	/* check to see if this changes the tile */
	if(mv[id].page->dy == 0 )
	{
	/* go down one tile */
	mv[id].ty--;
	/* Snap the origin downward */
	mv[id].page->data -= mv[id].page->width*4;
	mv[id].page->dy = mv[id].map->tiles->tileHeight;
	}

	/* draw the next row */
	y= 0;
	if(player[plid].q%3)
		if(id==0)
			mapDrawRow(&mv[0], mv[0].tx - 1, mv[0].ty-1, y, player, mv->page->dy);
		else
			modexCopyPageRegion(mv[id].page, mv[0].page, 0, y, 0, y, mv[id].map->tiles->tileWidth*22, mv[id].map->tiles->tileHeight);
	//}
}

void near mapScrollDown(map_view_t *mv, player_t *player, word id, word plid)
{
	word x, y;  /* coordinate for drawing */

	/* increment the pixel position and update the page */
	mv[id].page->dy += player[plid].speed;

	/* check to see if this changes the tile */
	if(mv[id].page->dy >= mv[id].dyThresh )
	{
	/* go down one tile */
	mv[id].ty++;
	/* Snap the origin downward */
	mv[id].page->data += mv[id].page->width*4;
	mv[id].page->dy = mv[id].map->tiles->tileHeight;
	}

	/* draw the next row */
	y= SCREEN_HEIGHT + mv[id].map->tiles->tileHeight;
	if(player[plid].q%3)
		if(id==0)
			mapDrawRow(&mv[0], mv[0].tx - 1, mv[0].ty+15, y, player, mv->page->dy);
		else
			modexCopyPageRegion(mv[id].page, mv[0].page, 0, y, 0, y, mv[id].map->tiles->tileWidth*22, mv[id].map->tiles->tileHeight);
	//}
}

sword chkmap(map_t *map, word q)
{
//	bitmap_t bp;
	static byte x[(MAPW*MAPH)+1] = /*{
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60
};*/
/*{
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 1, 2, 3, 4,
21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 5, 6, 7, 8,
41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 1, 2, 3, 4,
61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 5, 6, 7, 8,
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 1, 2, 3, 4,
21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 5, 6, 7, 8,
41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 1, 2, 3, 4,
61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 5, 6, 7, 8,
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 1, 2, 3, 4,
21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 5, 6, 7, 8,
41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 1, 2, 3, 4,
61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 5, 6, 7, 8,
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 1, 2, 3, 4,
21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 5, 6, 7, 8,
41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 1, 2, 3, 4,
61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 5, 6, 7, 8,
1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 1, 2, 3, 4,
21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 5, 6, 7, 8,
41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 1, 2, 3, 4
	};*/
{ 1, 2, 3, 4, 0, 3, 3, 3, 3, 3, 3, 3, 3, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 6, 7, 8, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 10, 11, 12, 4, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 13, 14, 15, 16, 0, 1, 1, 1, 5, 8, 1, 11, 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 4, 0, 0, 0, 0, 0, 8, 8, 1, 11, 11, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 8, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 1, 2, 3, 4, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 5, 6, 7, 8, 6, 6, 6, 6, 6, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 1, 1, 1, 1, 1, 9, 10, 11, 12, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 13, 14, 15, 16, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6, 6, 6, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 10, 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 10, 10 };
/*1,	2,	3,	4,	0,	3,	3,	3,	3,	3,	3,	3,	3,	4,	1,	1,	1,	1,	1,	1,	\
5,	6,	7,	8,	0,	1,	0,	1,	0,	0,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	\
9,	10,	11,	12,	4,	1,	0,	1,	0,	0,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	\
13,	14,	15,	16,	0,	1,	1,	1,	5,	8,	1,	11,	11,	1,	1,	1,	1,	1,	1,	1,	\
0,	0,	4,	0,	0,	0,	0,	0,	8,	8,	1,	11,	11,	3,	1,	1,	1,	1,	1,	1,	\
1,	1,	1,	1,	0,	0,	0,	0,	8,	8,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	\
1,	1,	1,	1,	0,	0,	0,	0,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	\
1,	1,	1,	1,	0,	0,	0,	0,	1,	1,	1,	1,	1,	1,	1,	1,	3,	1,	1,	1,	\
1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	\
1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	\
1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	3,	1,	1,	1,	\
1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	\
1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	3,	1,	1,	1,	\
1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	\
1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	3,	3,	3,	3,	1,	2,	3,	4 };*/
	//check for failed to load map
	if((map->width == map->height == 0) && (q>0))
	{
		//initiate a null map!
		map->width=MAPW;///2;
		map->height=MAPH;///2;
//		map->data = malloc(((map->width*map->height)+1)*sizeof(byte));
		map->data = &x;
		map->tiles = malloc(sizeof(tiles_t));
		//fix this to be far~
//		bp = bitmapLoadPcx("data/ed.pcx");
//		map->tiles->data = &bp;
		map->tiles->debug_data = map->data;
		map->tiles->tileHeight = 16;
		map->tiles->tileWidth = 16;
		map->tiles->rows = 1;
		map->tiles->cols = 1;
		map->tiles->debug_text = true;
	}
	else map->tiles->debug_text = false;
	return 0;
}

void mapGoTo(map_view_t *mv, int tx, int ty)
{
	int px, py;
	unsigned int i;

	/* set up the coordinates */
	mv->tx = tx;
	mv->ty = ty;
	mv->page->dx = mv->map->tiles->tileWidth;
	mv->page->dy = mv->map->tiles->tileHeight;

	/* set up the thresholds */
	mv->dxThresh = mv->map->tiles->tileWidth * 2;
	mv->dyThresh = mv->map->tiles->tileHeight * 2;

	/* draw the tiles */
	modexClearRegion(mv->page, 0, 0, mv->page->width, mv->page->height, 0);
	py=0;
	i=mv->ty * mv->map->width + mv->tx;
	for(ty=mv->ty-1; py < SCREEN_HEIGHT+mv->dyThresh && ty < mv->map->height; ty++, py+=mv->map->tiles->tileHeight) {
		mapDrawWRow(mv, tx-1, ty, py);
	i+=mv->map->width - tx;
	}
}


void near
mapDrawTile(tiles_t *t, word i, page_t *page, word x, word y)
{
	word rx;
	word ry;
	word textx=0;
	word texty=0;
	//if(i==0) i=2;
	if(i==0)
	{
		//wwww
		modexClearRegion(page, x, y, t->tileWidth, t->tileHeight, 0); //currently the over scan color!
	}
	else
	{
		rx = (((i-1) % ((t->data->width)/t->tileWidth)) * t->tileWidth);
		ry = (((i-1) / ((t->data->height)/t->tileHeight)) * t->tileHeight);
////0000printf("i=%d\n", i);
		switch(t->debug_text)
		{
			case 0:
#ifndef SPRITE
				modexClearRegion(page, x, y, t->tileWidth, t->tileHeight, ((t->debug_data[i])+1)*2);
				//cannot print number value du to it being slow as bakapee
#else
				modexDrawBmpRegion(page, x, y, rx, ry, t->tileWidth, t->tileHeight, (t->data));
#endif
			break;
			case 1:
				modexClearRegion(page, x, y, t->tileWidth, t->tileHeight, (t->debug_data[i])+1);
				//modexprintbig(page, x, y, 1, 15, 0, (t->debug_data));
				/*for(texty=0; texty<2; texty++)
				{
					for(textx=0; textx<2; textx++)
					{*/
//						modexprint(page, x+(textx*8), y+(texty*8), 1, (word)(t->debug_data), 0, (t->debug_data));
/*					}
				}*/
			break;
		}
	}
}

void near mapDrawRow(map_view_t *mv, int tx, int ty, word y, player_t *p, word poopoffset)
{
	word x;
	int i;
	poopoffset%=p[0].speed;
//printf("y: %d\n", poopoffset);
	/* the position within the map array */
	i=ty * mv->map->width + tx;
	for(x=poopoffset; x<(SCREEN_WIDTH+mv->dxThresh)/(poopoffset+1) && tx < mv->map->width; x+=mv->map->tiles->tileWidth, tx++) {
	if(i>=0) {
		/* we are in the map, so copy! */
		mapDrawTile(mv->map->tiles, mv->map->data[i], mv->page, x, y);
	}
	i++; /* next! */
	}
}

void near mapDrawCol(map_view_t *mv, int tx, int ty, word x, player_t *p, word poopoffset)
{
	int y;
	int i;
	poopoffset%=p[0].speed;
//printf("x: %d\n", poopoffset);
	/* location in the map array */
	i=ty * mv->map->width + tx;

	/* We'll copy all of the columns in the screen,
	   i + 1 row above and one below */
	for(y=poopoffset; y<(SCREEN_HEIGHT+mv->dyThresh)/(poopoffset+1) && ty < mv->map->height; y+=mv->map->tiles->tileHeight, ty++) {
	if(i>=0) {
		/* we are in the map, so copy away! */
		mapDrawTile(mv->map->tiles, mv->map->data[i], mv->page, x, y);
	}
	i += mv->map->width;
	}
}

void mapDrawWRow(map_view_t *mv, int tx, int ty, word y)
{
	word x;
	int i;

	/* the position within the map array */
	i=ty * mv->map->width + tx;
	for(x=0; x<SCREEN_WIDTH+mv->dxThresh && tx < mv->map->width; x+=mv->map->tiles->tileWidth, tx++) {
	if(i>=0) {
		/* we are in the map, so copy! */
		mapDrawTile(mv->map->tiles, mv->map->data[i], mv->page, x, y);
	}
	i++; /* next! */
	}
}

void mapDrawWCol(map_view_t *mv, int tx, int ty, word x)
{
	int y;
	int i;

	/* location in the map array */
	i=ty * mv->map->width + tx;

	/* We'll copy all of the columns in the screen,
	   i + 1 row above and one below */
	for(y=0; y<SCREEN_HEIGHT+mv->dyThresh && ty < mv->map->height; y+=mv->map->tiles->tileHeight, ty++) {
	if(i>=0) {
		/* we are in the map, so copy away! */
		mapDrawTile(mv->map->tiles, mv->map->data[i], mv->page, x, y);
	}
	i += mv->map->width;
	}
}

/*void qclean()
{
	modexLeave();
	//setkb(0);
}*/

//sync!
void shinku(page_t *page, global_game_variables_t *gv)
{
	if(((*clockdw)!=gv->clock_start)&&gv->tiku>0)
	{
	byte *pee;
	word x = (16);
	word y = (8+16);
	word col = 15;
	word bgcol = 0;
	word type = 1;
	float t;
	//gv->clock=*hwclockw;
		t=((*clockdw)-gv->clock_start)/18.2;
		//if(gv->tiku<=0) return;
		//if(elapsed_timer(gv) >= (1.0 / gv->frames_per_second))
		//{
		pee = _fmalloc(sizeof(byte)*64);
		sprintf(pee, "%lu", gv->tiku);
		modexprint(page, 16, 16, 1, 15, 0, pee);
		//sprintf(pee, "%f", t);
		//modexprint(page, x, y, type, col, bgcol, pee);
		//printf("%f time\n", t);
		/*printf("%lu gv->cs	", gv->clock_start);
		printf("%lu time	", (*clockdw));
		printf("%f fps	", gv->tiku/t);
		printf("%lu tiku\n", gv->tiku);*/
		gv->tiku=0;
	}
		//sprintf(pee, "%f", *gv->tiku/\*time_in_seconds(gv));
		//modexprint(page, x, y+8, type, col, bgcol, pee);
	//}
	(gv->clock_start)=*clockdw;
}

void near animatePlayer(map_view_t *pip, player_t *player, word playnum, sword scrollswitch)
{
	sword x = player[playnum].x;
	sword y = player[playnum].y;
#ifdef SPRITE
	word dire=32; //direction
#else
	word dire=8; //direction
#endif
	sword qq; //scroll offset
	word ls = player[playnum].persist_aniframe;

	if(scrollswitch==0) qq = 0;
	else qq = ((player[playnum].q)*(player[playnum].speed));
	x-=4;
	y-=TILEWH;
	switch (player[playnum].d)
	{
		case 0:
			//up
			dire*=player[playnum].d;
			y-=qq;
		break;
		case 3:
			// right
			dire*=(player[playnum].d-2);
			x+=qq;
		break;
		case 2:
		break;
		case 4:
			//down
			dire*=(player[playnum].d-2);
			y+=qq;
		break;
		case 1:
			//left
			dire*=(player[playnum].d+2);
			x-=qq;
		break;
	}

#ifdef SPRITE
#ifdef BMPTYPE
#define FRAME1 oldDrawBmp(VGA, x, y, &player[playnum].data, 1);
#define FRAME2 oldDrawBmp(VGA, x, y, &player[playnum].data, 1);
#define FRAME3 oldDrawBmp(VGA, x, y, &player[playnum].data, 1);
#define FRAME4 oldDrawBmp(VGA, x, y, &player[playnum].data, 1);
#else
#define FRAME1 modexDrawSpriteRegion(pip[1].page, x, y, 48, dire, 24, 32, &player[playnum].data);
#define FRAME2 modexDrawSpriteRegion(pip[1].page, x, y, 24, dire, 24, 32, &player[playnum].data);
#define FRAME3 modexDrawSpriteRegion(pip[1].page, x, y, 0, dire, 24, 32, &player[playnum].data);
#define FRAME4 modexDrawSpriteRegion(pip[1].page, x, y, 24, dire, 24, 32, &player[playnum].data);
#endif
#else
#define FRAME1 modexClearRegion(pip[1].page, x, y, 24, 32, 2+dire);
#define FRAME2 modexClearRegion(pip[1].page, x, y, 24, 32, 1+dire);
#define FRAME3 modexClearRegion(pip[1].page, x, y, 24, 32, dire);
#define FRAME4 modexClearRegion(pip[1].page, x, y, 24, 32, 1+dire);
#endif
	modexCopyPageRegion(pip[1].page, pip[0].page, x-4, y-4, x-4, y-4, 28, 40);
	if(2>ls && ls>=1) { FRAME1 }else
	if(3>ls && ls>=2) { FRAME2 }else
	if(4>ls && ls>=3) { FRAME3 }else
	if(5>ls && ls>=4) { FRAME4 }
	//TODO: mask copy //modexCopyPageRegion(dest->page, src->page, x-4, y-4, x-4, y-4, 28, 40);
	//modexClearRegion(top->page, 66, 66, 2, 40, 0);
	//modexCopyPageRegion(dest->page, top->page, 66, 66, 66, 66, 2, 40);
	//turn this off if XT
	if(detectcpu() > 0)
	modexWaitBorder();
}
