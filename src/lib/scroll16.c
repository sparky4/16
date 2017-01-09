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
/*
	scroll16 library~
*/
#include "src/lib/scroll16.h"

void walk(map_view_t *pip, player_t *player, word pn)
{
	#define INC_PER_FRAME if(player[pn].q&1) player[pn].persist_aniframe++; if(player[pn].persist_aniframe>4) player[pn].persist_aniframe = 1;
	//printf("player[%d].d=%d\n", pn, player[pn].d);
	switch(player[pn].d)
	{
		//no direction
		case 2:
			//0000pip[0].video->startclk = (*clockw);
		break;
		//right movement
		case 3:
			//printf("pip[0].page->tilesw=%d	", pip[0].page->tilesw); printf("pip[0].page->tw=%d\n", pip[0].page->tw);
			if(pip[0].tx >= 0 && pip[0].tx+pip[0].page->tw < pip[0].map->width && player[pn].tx == pip[0].tx+pip[0].page->tilemidposscreenx &&
			!(pip[0].map->data[(player[pn].tx)+(pip[0].map->width*(player[pn].ty-1))] == 0))//!(player[pn].tx+1 == TRIGGX && player[pn].ty == TRIGGY))	//collision detection!
			{
				if(player[pn].q<=player[pn].spt)
				{
					INC_PER_FRAME;
					animatePlayer(pip, player, pn, 1);
					ScrollRight(pip, player, 3, pn);
					ScrollRight(pip, player, 2, pn);
					mapScrollRight(pip, player, !(pip[0].video->p), pn);
					mapScrollRight(pip, player, (pip[0].video->p), pn);
					if(!pageflipflop) modexShowPage(pip[1].page);
					player[pn].q++;
					//0000pip[0].video->clk = ((*clockw)-pip[0].video->startclk)/18.2;
				} else { player[pn].q = 1; player[pn].d = 2; player[pn].tx++; }
			}
			else if(player[pn].tx < pip[0].map->width && !(pip[0].map->data[(player[pn].tx)+(pip[0].map->width*(player[pn].ty-1))] == 0))//!(player[pn].tx+1 == TRIGGX && player[pn].ty == TRIGGY))
			{
				if(player[pn].q<=player[pn].spt)
				{
					INC_PER_FRAME;
					player[pn].x+=(player[pn].speed);
					animatePlayer(pip, player, pn, 0);
					if(!pageflipflop) modexShowPage(pip[1].page);
					player[pn].q++;
				} else { player[pn].q = 1; player[pn].d = 2; player[pn].tx++; }
			}
			else
			{
				if(!pageflipflop) modexCopyPageRegion(pip[1].page, pip[0].page, player[pn].x, player[pn].y-TILEWH, player[pn].x, player[pn].y-TILEWH, 24, 32);
#ifdef SPRITE
				PBUFSFUN(pip[0].page, player[pn].x, player[pn].y-TILEWH, 24, 32, 24, 32, PLAYERBMPDATA);
#else
				modexClearRegion(pip[1].page, player[pn].x, player[pn].y-TILEWH, 24, 32, 14);
#endif
				if(!pageflipflop) modexShowPage(pip[1].page);
				player[pn].d = 2;
			}
			player[pn].triggerx = player[pn].tx+1;
			player[pn].triggery = player[pn].ty;
		break;

		//left movement
		case 1:
			if(pip[0].tx > 0 && pip[0].tx+pip[0].page->tw <= pip[0].map->width && player[pn].tx == pip[0].tx+pip[0].page->tilemidposscreenx &&
			!(pip[0].map->data[(player[pn].tx-2)+(pip[0].map->width*(player[pn].ty-1))] == 0))//!(player[pn].tx-1 == TRIGGX && player[pn].ty == TRIGGY))	//collision detection!
			{
				if(player[pn].q<=player[pn].spt)
				{
					INC_PER_FRAME;
					animatePlayer(pip, player, pn, 1);
					ScrollLeft(pip, player, 3, pn);
					ScrollLeft(pip, player, 2, pn);
					mapScrollLeft(pip, player, !(pip[0].video->p), pn);
					mapScrollLeft(pip, player, (pip[0].video->p), pn);
					if(!pageflipflop) modexShowPage(pip[1].page);
					player[pn].q++;
					//0000pip[0].video->clk = ((*clockw)-pip[0].video->startclk)/18.2;
				} else { player[pn].q = 1; player[pn].d = 2; player[pn].tx--; }
			}
			else if(player[pn].tx > 1 && !(pip[0].map->data[(player[pn].tx-2)+(pip[0].map->width*(player[pn].ty-1))] == 0))//!(player[pn].tx-1 == TRIGGX && player[pn].ty == TRIGGY))
			{
				if(player[pn].q<=player[pn].spt)
				{
					INC_PER_FRAME;
					player[pn].x-=(player[pn].speed);
					animatePlayer(pip, player, pn, 0);
					if(!pageflipflop) modexShowPage(pip[1].page);
					player[pn].q++;
				} else { player[pn].q = 1; player[pn].d = 2; player[pn].tx--; }
			}
			else
			{
				if(!pageflipflop) modexCopyPageRegion(pip[1].page, pip[0].page, player[pn].x, player[pn].y-TILEWH, player[pn].x, player[pn].y-TILEWH, 24, 32);
#ifdef SPRITE
				PBUFSFUN(pip[0].page, player[pn].x, player[pn].y-TILEWH, 24, 96, 24, 32, PLAYERBMPDATA);
#else
				modexClearRegion(pip[1].page, player[pn].x, player[pn].y-TILEWH, 24, 32, 10);
#endif
				if(!pageflipflop) modexShowPage(pip[1].page);
				player[pn].d = 2;
			}
			player[pn].triggerx = player[pn].tx-1;
			player[pn].triggery = player[pn].ty;
		break;

		//down movement
		case 4:
			if(pip[0].ty >= 0 && pip[0].ty+pip[0].page->th < pip[0].map->height && player[pn].ty == pip[0].ty+pip[0].page->tilemidposscreeny &&
			!(pip[0].map->data[(player[pn].tx-1)+(pip[0].map->width*(player[pn].ty))] == 0))//!(player[pn].tx == TRIGGX && player[pn].ty+1 == TRIGGY))	//collision detection!
			{
				if(player[pn].q<=player[pn].spt)
				{
					INC_PER_FRAME;
					animatePlayer(pip, player, pn, 1);
					ScrollDown(pip, player, 3, pn);
					ScrollDown(pip, player, 2, pn);
					mapScrollDown(pip, player, !(pip[0].video->p), pn);
					mapScrollDown(pip, player, (pip[0].video->p), pn);
					if(!pageflipflop) modexShowPage(pip[1].page);
					player[pn].q++;
					//0000pip[0].video->clk = ((*clockw)-pip[0].video->startclk)/18.2;
				} else { player[pn].q = 1; player[pn].d = 2; player[pn].ty++; }
			}
			else if(player[pn].ty < pip[0].map->height && !(pip[0].map->data[(player[pn].tx-1)+(pip[0].map->width*(player[pn].ty))] == 0))//!(player[pn].tx == TRIGGX && player[pn].ty+1 == TRIGGY))
			{
				if(player[pn].q<=player[pn].spt)
				{
					INC_PER_FRAME;
					player[pn].y+=(player[pn].speed);
					animatePlayer(pip, player, pn, 0);
					if(!pageflipflop) modexShowPage(pip[1].page);
					player[pn].q++;
				} else { player[pn].q = 1; player[pn].d = 2; player[pn].ty++; }
			}
			else
			{
				if(!pageflipflop) modexCopyPageRegion(pip[1].page, pip[0].page, player[pn].x, player[pn].y-TILEWH, player[pn].x, player[pn].y-TILEWH, 24, 32);
#ifdef SPRITE
				PBUFSFUN(pip[0].page, player[pn].x, player[pn].y-TILEWH, 24, 64, 24, 32, PLAYERBMPDATA);
#else
				modexClearRegion(pip[1].page, player[pn].x, player[pn].y-TILEWH, 24, 32, 9);
#endif
				if(!pageflipflop) modexShowPage(pip[1].page);
				player[pn].d = 2;
			}
			player[pn].triggerx = player[pn].tx;
			player[pn].triggery = player[pn].ty+1;
		break;

		//up movement
		case 0:
			if(pip[0].ty > 0 && pip[0].ty+pip[0].page->th <= pip[0].map->height && player[pn].ty == pip[0].ty+pip[0].page->tilemidposscreeny &&
			!(pip[0].map->data[(player[pn].tx-1)+(pip[0].map->width*(player[pn].ty-2))] == 0))//!(player[pn].tx == TRIGGX && player[pn].ty-1 == TRIGGY))	//collision detection!
			{
				if(player[pn].q<=player[pn].spt)
				{
					INC_PER_FRAME;
					animatePlayer(pip, player, pn, 1);
					ScrollUp(pip, player, 3, pn);
					ScrollUp(pip, player, 2, pn);
					mapScrollUp(pip, player, !(pip[0].video->p), pn);
					mapScrollUp(pip, player, (pip[0].video->p), pn);
					if(!pageflipflop) modexShowPage(pip[1].page);
					player[pn].q++;
					//0000pip[0].video->clk = ((*clockw)-pip[0].video->startclk)/18.2;
				} else { player[pn].q = 1; player[pn].d = 2; player[pn].ty--; }
			}
			else if(player[pn].ty > 1 && !(pip[0].map->data[(player[pn].tx-1)+(pip[0].map->width*(player[pn].ty-2))] == 0))//!(player[pn].tx == TRIGGX &&  player[pn].ty-1 == TRIGGY))
			{
				if(player[pn].q<=player[pn].spt)
				{
					INC_PER_FRAME;
					player[pn].y-=(player[pn].speed);
					animatePlayer(pip, player, 0, pn);
					if(!pageflipflop) modexShowPage(pip[1].page);
					player[pn].q++;
				} else { player[pn].q = 1; player[pn].d = 2; player[pn].ty--; }
			}
			else
			{
				if(!pageflipflop) modexCopyPageRegion(pip[1].page, pip[0].page, player[pn].x, player[pn].y-TILEWH, player[pn].x, player[pn].y-TILEWH, 24, 32);
#ifdef SPRITE
				PBUFSFUN(pip[0].page, player[pn].x, player[pn].y-TILEWH, 24, 0, 24, 32, PLAYERBMPDATA);
#else
				modexClearRegion(pip[1].page, player[pn].x, player[pn].y-TILEWH, 24, 32, 12);
#endif
				if(!pageflipflop) modexShowPage(pip[1].page);
				player[pn].d = 2;
			}
			player[pn].triggerx = player[pn].tx;
			player[pn].triggery = player[pn].ty-1;
		break;
	}
}

//panning page
void panPageManual(map_view_t *pip, player_t *player, word pn)
{
	switch(player[pn].d)
	{
		//right movement
		case 3:
			if(pip[pip[0].pan->pn].tx >= 0 && pip[pip[0].pan->pn].tx+pip[pip[0].pan->pn].page->tw < pip[pip[0].pan->pn].page->tilesw)
			{
				if(player[pn].q<=player[pn].spt)
				{
					pip[pip[0].pan->pn].page->dx+=4;
					modexShowPage(pip[pip[0].pan->pn].page);
					player[pn].q++;
				} else { player[pn].q = 1; player[pn].d = 2; pip[pip[0].pan->pn].tx++; }
			}
		break;

		//left movement
		case 1:
			if(pip[pip[0].pan->pn].tx > 0 && pip[pip[0].pan->pn].tx+pip[pip[0].pan->pn].page->tw <= pip[pip[0].pan->pn].page->tilesw)
			{
				if(player[pn].q<=player[pn].spt)
				{
					pip[pip[0].pan->pn].page->dx-=4;
					modexShowPage(pip[pip[0].pan->pn].page);
					player[pn].q++;
				} else { player[pn].q = 1; player[pn].d = 2; pip[pip[0].pan->pn].tx--; }
			}
		break;

		//down movement
		case 4:
			if(pip[pip[0].pan->pn].ty >= 0 && pip[pip[0].pan->pn].ty+pip[pip[0].pan->pn].page->th < pip[pip[0].pan->pn].page->tilesh)
			{
				if(player[pn].q<=player[pn].spt)
				{
					pip[pip[0].pan->pn].page->dy+=4;
					modexShowPage(pip[pip[0].pan->pn].page);
					player[pn].q++;
				} else { player[pn].q = 1; player[pn].d = 2; pip[pip[0].pan->pn].ty++; }
			}
		break;

		//up movement
		case 0:
			if(pip[pip[0].pan->pn].ty > 0 && pip[pip[0].pan->pn].ty+pip[pip[0].pan->pn].page->th <= pip[pip[0].pan->pn].page->tilesh)
			{
				if(player[pn].q<=player[pn].spt)
				{
					pip[pip[0].pan->pn].page->dy-=4;
					modexShowPage(pip[pip[0].pan->pn].page);
					player[pn].q++;
				} else { player[pn].q = 1; player[pn].d = 2; pip[pip[0].pan->pn].ty--; }
			}
			break;
	}
	//if (player[pn].d!=2) printf("player[%u].d=%u player[%u].q=%u\n", pn, player[pn].d, pn, player[pn].q);
}

//panning vmem
void panVmemManual(map_view_t *pip, player_t *player, word pn)
{
	switch(player[pn].d)
	{
		//right movement
		case 3:
			if(pip[pip[0].pan->pn].tx >= 0 && pip[pip[0].pan->pn].tx+pip[pip[0].pan->pn].page->tw < pip[pip[0].pan->pn].page->tilesw)
			{
				if(player[pn].q<=player[pn].spt)
				{
					pip[pip[0].pan->pn].page->dx+=4;
					modexShowPageVsync(pip[pip[0].pan->pn].page);
					player[pn].q++;
				} else { player[pn].q = 1; player[pn].d = 2; pip[pip[0].pan->pn].tx++; }
			}
		break;

		//left movement
		case 1:
			if(pip[pip[0].pan->pn].tx > 0 && pip[pip[0].pan->pn].tx+pip[pip[0].pan->pn].page->tw <= pip[pip[0].pan->pn].page->tilesw)
			{
				if(player[pn].q<=player[pn].spt)
				{
					pip[pip[0].pan->pn].page->dx-=4;
					modexShowPageVsync(pip[pip[0].pan->pn].page);
					player[pn].q++;
				} else { player[pn].q = 1; player[pn].d = 2; pip[pip[0].pan->pn].tx--; }
			}
		break;

		//down movement
		case 4:
			if(pip[pip[0].pan->pn].ty >= 0 && pip[pip[0].pan->pn].ty+pip[pip[0].pan->pn].page->th < pip[pip[0].pan->pn].page->tilesh+pip[pip[1].pan->pn].page->tilesh)//+pip[pip[2].pan->pn].page->tilesh+pip[pip[3].pan->pn].page->tilesh)
			{
				if(player[pn].q<=player[pn].spt)
				{
					pip[pip[0].pan->pn].page->dy+=4;
					modexShowPageVsync(pip[pip[0].pan->pn].page);
					player[pn].q++;
				} else { player[pn].q = 1; player[pn].d = 2; pip[pip[0].pan->pn].ty++; }
			}
		break;

		//up movement
		case 0:
			if(pip[pip[0].pan->pn].ty > 0 && pip[pip[0].pan->pn].ty+pip[pip[0].pan->pn].page->th <= pip[pip[0].pan->pn].page->tilesh+pip[pip[1].pan->pn].page->tilesh)//+pip[pip[2].pan->pn].page->tilesh+pip[pip[3].pan->pn].page->tilesh)
			{
				if(player[pn].q<=player[pn].spt)
				{
					pip[pip[0].pan->pn].page->dy-=4;
					modexShowPageVsync(pip[pip[0].pan->pn].page);
					player[pn].q++;
				} else { player[pn].q = 1; player[pn].d = 2; pip[pip[0].pan->pn].ty--; }
			}
			break;
	}
	//if (player[pn].d!=2) printf("player[%u].d=%u player[%u].q=%u\n", pn, player[pn].d, pn, player[pn].q);
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
	word x;//, y;  /* coordinate for drawing */

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
	x= mv[0].page->sw + mv[id].map->tiles->tileWidth;
	if(player[plid].q%4)
		if(id==0)
			mapDrawCol(&mv[0], mv[0].tx + mv[0].page->tw, mv[0].ty-1, x, player, mv->page->dx);
		else
			if(!pageflipflop && !pageploop)
				modexCopyPageRegion(mv[id].page, mv[0].page, x, 0, x, 0, mv[id].map->tiles->tileWidth, mv[id].map->tiles->tileHeight*(mv[0].page->th+2));
	mv[0].video->r=1;
}


void near mapScrollLeft(map_view_t *mv, player_t *player, word id, word plid)
{
	word x;//,y;  /* coordinate for drawing */

	/* decrement the pixel position and update the page */
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
			if(!pageflipflop && !pageploop)
				modexCopyPageRegion(mv[id].page, mv[0].page, x, 0, x, 0, mv[id].map->tiles->tileWidth, mv[id].map->tiles->tileHeight*(mv[0].page->th+2));
	mv[0].video->r=1;
}


void near mapScrollUp(map_view_t *mv, player_t *player, word id, word plid)
{
	word y;//x,  /* coordinate for drawing */

	/* decrement the pixel position and update the page */
	mv[id].page->dy -= player[plid].speed;

	/* check to see if this changes the tile */
	if(mv[id].page->dy == 0 )
	{
	/* go down one tile */
	mv[id].ty--;
	/* Snap the origin downward */
	mv[id].page->data -= mv[id].page->pi;

	mv[id].page->dy = mv[id].map->tiles->tileHeight;
	}

	/* draw the next row */
	y= 0;
	if(player[plid].q%3)
		if(id==0)
			mapDrawRow(&mv[0], mv[0].tx - 1, mv[0].ty-1, y, player, mv->page->dy);
		else
			if(!pageflipflop && !pageploop)
				modexCopyPageRegion(mv[id].page, mv[0].page, 0, y, 0, y, mv[id].map->tiles->tileWidth*(mv[0].page->tw+2), mv[id].map->tiles->tileHeight);
	mv[0].video->r=1;
}

void near mapScrollDown(map_view_t *mv, player_t *player, word id, word plid)
{
	word y;//x,  /* coordinate for drawing */

	/* increment the pixel position and update the page */
	mv[id].page->dy += player[plid].speed;

	/* check to see if this changes the tile */
	if(mv[id].page->dy >= mv[id].dyThresh )
	{
	/* go down one tile */
	mv[id].ty++;
	/* Snap the origin downward */
	mv[id].page->data += mv[id].page->pi;

	mv[id].page->dy = mv[id].map->tiles->tileHeight;
	}

	/* draw the next row */
	y= mv[0].page->sh + mv[id].map->tiles->tileHeight;
	if(player[plid].q%3)
		if(id==0)
			mapDrawRow(&mv[0], mv[0].tx - 1, mv[0].ty+mv[0].page->th, y, player, mv->page->dy);
		else
			if(!pageflipflop && !pageploop)
				modexCopyPageRegion(mv[id].page, mv[0].page, 0, y, 0, y, mv[id].map->tiles->tileWidth*(mv[0].page->tw+2), mv[id].map->tiles->tileHeight);
	mv[0].video->r=1;
}


//TODO finish this wwww
void near ScrollRight(map_view_t *mv, player_t *player, word id, word plid)
{
	/* increment the pixel position and update the page */
	mv[id].page->dx += player[plid].speed;

	/* check to see if this changes the tile */
	if(mv[id].page->dx >= mv[0].dxThresh )
	{
// 		vga_setup_wm1_block_copy();
// 		_fmemmove(mv[id].page->data+4, mv[id].page->data, mv[id].page->pagesize);
// 		vga_restore_rm0wm0();
		/* Snap the origin forward */
		mv[id].page->data += 4;
		mv[id].page->dx = mv[0].map->tiles->tileWidth;
	}
}

void near ScrollLeft(map_view_t *mv, player_t *player, word id, word plid)
{
	/* decrement the pixel position and update the page */
	mv[id].page->dx -= player[plid].speed;

	/* check to see if this changes the tile */
	if(mv[id].page->dx == 0)
	{
// 		vga_setup_wm1_block_copy();
// 		_fmemmove(mv[id].page->data-4, mv[id].page->data, mv[id].page->pagesize);
// 		vga_restore_rm0wm0();
		/* Snap the origin backward */
		mv[id].page->data -= 4;
		mv[id].page->dx = mv[0].map->tiles->tileWidth;
	}
}

void near ScrollUp(map_view_t *mv, player_t *player, word id, word plid)
{
	/* decrement the pixel position and update the page */
	mv[id].page->dy -= player[plid].speed;

	/* check to see if this changes the tile */
	if(mv[id].page->dy == 0)
	{
// 		vga_setup_wm1_block_copy();
// 		_fmemmove(mv[id].page->data-mv[id].page->pi, mv[id].page->data, mv[id].page->pagesize);
// 		vga_restore_rm0wm0();
		/* Snap the origin backward */
		mv[id].page->data -= mv[id].page->pi;
		mv[id].page->dy = mv[0].map->tiles->tileWidth;
	}
}

void near ScrollDown(map_view_t *mv, player_t *player, word id, word plid)
{
	/* increment the pixel position and update the page */
	mv[id].page->dy += player[plid].speed;

	/* check to see if this changes the tile */
	if(mv[id].page->dy >= mv[0].dxThresh )
	{
// 		vga_setup_wm1_block_copy();
// 		_fmemmove(mv[id].page->data+mv[id].page->pi, mv[id].page->data, mv[id].page->pagesize);
// 		vga_restore_rm0wm0();
		/* Snap the origin forward */
		mv[id].page->data += mv[id].page->pi;
		mv[id].page->dy = mv[0].map->tiles->tileWidth;
	}
}

sword chkmap(map_t *map, word q)
{
//	bitmap_t bp;
	static byte x[(MAPW*MAPH)+1] =
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

//TODO: player position here
void mapGoTo(map_view_t *mv, int tx, int ty)
{
	int py;//px,
	unsigned int i;

	mapinitmapview(mv, tx, ty);

	/* draw the tiles */
	modexClearRegion(mv[0].page, 0, 0, mv[0].page->width, mv[0].page->height, 0);
	py=0;
	i=mv[0].ty * mv[0].map->width + mv[0].tx;
	for(ty=mv[0].ty-1; py < mv[0].page->sh+mv->dyThresh && ty < mv[0].map->height; ty++, py+=mv[0].map->tiles->tileHeight) {
		mapDrawWRow(&mv[0], tx-1, ty, py);
	i+=mv->map->width - tx;
	}
	if(!pageploop) modexCopyPageRegion(mv[1].page, mv[0].page, 0, 0, 0, 0, mv[0].page->width, mv[0].page->height);
// 	{
// 		unsigned int k,j,o;
// 		/* fill screen with a distinctive pattern */
// 		for (k=0;k < vga_state.vga_width;k++) {
// 			o = k >> 2;
// 			vga_write_sequencer(0x02/*map mask*/,1 << (k&3));
// 				for (j=0;j < (mv[0].page->height)+(mv[1].page->height)+(mv[2].page->height)+(mv[3].page->height);j++,o += vga_state.vga_stride)
// 					vga_state.vga_graphics_ram[o] = (k^j)&15; // VRL samples put all colors in first 15!
// 		}
// 	}
	modexCopyPageRegion(mv[3].page, mv[0].page, 0/**/, 0/**/, 0, 0, 24, 32);
}

void mapinitmapview(map_view_t *mv, int tx, int ty)
{
	/* set up the coordinates */
	mv[0].tx = mv[1].tx = tx;
	mv[0].ty = mv[1].ty = ty;
	mv[0].page->dx = mv[1].page->dx = mv[2].page->dx = mv[3].page->dx = mv->map->tiles->tileWidth;
	mv[0].page->dy = mv[1].page->dy = mv[2].page->dy = mv[3].page->dy = mv->map->tiles->tileHeight;

	/* set up the thresholds */
	mv[0].dxThresh = mv[1].dxThresh = mv[2].dxThresh = mv[3].dxThresh = mv->map->tiles->tileWidth * 2;
	mv[0].dyThresh = mv[1].dyThresh = mv[2].dxThresh = mv[3].dxThresh = mv->map->tiles->tileHeight * 2;
}

void near
mapDrawTile(tiles_t *t, word i, page_t *page, word x, word y)
{
	word rx;
	word ry;
	//word textx=0, texty=0;
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
#ifndef TILERENDER
				modexClearRegion(page, x, y, t->tileWidth, t->tileHeight, ((t->debug_data[i])+1));
				//modexprint(page, x, y, 1, 15, 0, (char const *)(t->debug_data[i]));
#else
				PBUFBFUN		(page, x, y, rx, ry, t->tileWidth, t->tileHeight, (t->data));
				/* then the sprite. note modding ram ptr means we just draw to (x&3,0) */
				//draw_vrl1_vgax_modex(x-rx,y-ry,vrl_header,vrl_lineoffs,buffer+sizeof(*vrl_header),bufsz-sizeof(*vrl_header));
				//modexDrawBmpRegion	(page, x, y, rx, ry, t->tileWidth, t->tileHeight, (t->data));
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
	for(x=poopoffset; x<(mv->page->sw+mv->dxThresh)/(poopoffset+1) && tx < mv->map->width; x+=mv->map->tiles->tileWidth, tx++) {
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
	for(y=poopoffset; y<(mv->page->sh+mv->dyThresh)/(poopoffset+1) && ty < mv->map->height; y+=mv->map->tiles->tileHeight, ty++) {
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
	for(x=0; x<mv->page->sw+mv->dxThresh && tx < mv->map->width; x+=mv->map->tiles->tileWidth, tx++) {
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
	for(y=0; y<mv->page->sh+mv->dyThresh && ty < mv->map->height; y+=mv->map->tiles->tileHeight, ty++) {
	if(i>=0) {
		/* we are in the map, so copy away! */
		mapDrawTile(mv->map->tiles, mv->map->data[i], mv->page, x, y);
	}
	i += mv->map->width;
	}
}

/*void qclean()
{
	//setkb(0);
}*/

unsigned char shinku_fps_indicator_page = 2;
boolean pageflipflop = 1;
boolean pageploop = 1;

/*	sync	*/
void shinku(global_game_variables_t *gv)
{
	word x = (0) + gv->video.page[/*!*/(gv->video.p)].dx; // follow the screen
	word y = (0) + gv->video.page[/*!*/(gv->video.p)].dy; // follow the screen
	word col = 7, bgcol = 0, type = 1;//w = 64, h = 8,
	//byte o,o2,i;
	//modexCopyPageRegion(pip[1].page, pip[2].page, 16, 16, 16, 16, (14*8)+4, 8+4);
	/* block copy to visible RAM from offscreen */
//	vga_setup_wm1_block_copy();
//	modexCopyPageRegion(&(gv->video.page[shinku_fps_indicator_page]), &(gv->video.page[!shinku_fps_indicator_page]), x, y, x+w, 0, w, h);
// 	o =	*(gv->video.page[2].data); // source offscreen
// 	o2 =	*(gv->video.page[shinku_fps_indicator_page].data)+(y * vga_state.vga_stride) + (x >> 2); // dest visible (original stride)
// 	for (i=0;i < h;i++,o += vga_state.vga_draw_stride,o2 += vga_state.vga_stride) vga_wm1_mem_block_copy(o2,o,w >> 2);
	/* must restore Write Mode 0/Read Mode 0 for this code to continue drawing normally */
//	vga_restore_rm0wm0();
	if(elapsed_timer(gv) >= (1.0 / gv->kurokku.frames_per_second))
	{
        // NTS: For some bizarre reason, gv->pee is not initialized, but the pointer is not NULL even
        //      though it should be. Instead it's NULL as a near pointer but contains a non-null
        //      segment value, so testing against NULL doesn't work. It is initialized properly if
        //      you call start_timer() though which uses near malloc. Rather than fight with that,
        //      I decided it would be better to declare a temp buffer statically and sprintf to that.
        //
        //      This fixes *** Null pointer assignment detected error message in ZCROLL.EXE on exit.
		sprintf(global_temp_status_text, "%.0f fps", (double)gv->kurokku.tiku/ticktock(gv));
		//modexClearRegion(&(gv->video.page[shinku_fps_indicator_page]), x, y, w, h, 45);
		modexprint(&(gv->video.page[/*!*/(gv->video.p)]), x, y, type, col, bgcol, global_temp_status_text);
		gv->kurokku.tiku=0;
		/* block copy to visible RAM from offscreen */
//		vga_setup_wm1_block_copy();
// 		o =	*(gv->video.page[shinku_fps_indicator_page].data); // source offscreen
// 		o2 =	*(gv->video.page[2].data)+(y * vga_state.vga_stride) + (x >> 2); // dest visible (original stride)
// 		for (i=0;i < h;i++,o += vga_state.vga_draw_stride,o2 += vga_state.vga_stride) vga_wm1_mem_block_copy(o2,o,w >> 2);
//		modexCopyPageRegion(&(gv->video.page[shinku_fps_indicator_page]), &(gv->video.page[!shinku_fps_indicator_page]), x, y, x, 0, w, h);
		/* must restore Write Mode 0/Read Mode 0 for this code to continue drawing normally */
//		vga_restore_rm0wm0();
	}else //copy dat sheet
	gv->kurokku.tiku++;

	switch(gv->kurokku.fpscap)
	{
		case 0:
			//modexprint(&(gv->video.page[shinku_fps_indicator_page]), x, y+8, type, col, bgcol, "sanic!");
			gv->kurokku.frames_per_second=1;
		break;
		case 1:
			//turn this off if XT
			//modexWaitBorder();
			vga_wait_for_vsync();
			gv->kurokku.frames_per_second=60;
		break;
	}
	if(pageflipflop){
	if(gv->video.r){
		//vga_setup_wm1_block_copy();
		//_fmemcpy((gv->video.page[(gv->video.p)]).data, (gv->video.page[(!gv->video.p)]).data, gv->video.page[(!gv->video.p)].pagesize);
		//vga_restore_rm0wm0();
		if(!pageploop) modexCopyPageRegion(&(gv->video.page[(gv->video.p)]), &(gv->video.page[(!gv->video.p)]), 0, 0, 0, 0, gv->video.page[gv->video.p].width, gv->video.page[!gv->video.p].height);
		modexShowPage(&(gv->video.page[gv->video.p]));
		if(!pageploop) gv->video.p=!gv->video.p;
		gv->video.r=!gv->video.r;
		//0000gv->video.tickclk = ((*clockw)-gv->video.startclk)/18.2;
	}
	}
}

void near animatePlayer(map_view_t *pip, player_t *player, word pn, sword scrollswitch)
{
	sword x = player[pn].x;
	sword y = player[pn].y;
	sword bx = x+16;	//buffer's x
	sword by = y+16;	//buffer's y
	word dire=32; //direction
	sword qq; //scroll offset
	word ls = player[pn].persist_aniframe;

	switch(scrollswitch)
	{
		case 0:
			qq = 0;
		break;
		default:
			qq = ((player[pn].q)*(player[pn].speed));
		break;
	}
	//x-=4;
	y-=pip[0].map->tiles->tileHeight;
	switch (player[pn].d)
	{
		case 0:
			//up
			dire*=player[pn].d;
			y-=qq;
			by-=4;
		break;
		case 3:
			// right
			dire*=(player[pn].d-2);
			x+=qq;
			bx+=4;
		break;
		case 2:
		break;
		case 4:
			//down
			dire*=(player[pn].d-2);
			y+=qq;
			by+=4;
		break;
		case 1:
			//left
			dire*=(player[pn].d+2);
			x-=qq;
			bx-=4;
		break;
	}

#ifdef SPRITE
#define FRAME1 PBUFSFUN(pip[/*!*/(pip->video->p)].page, x, y, 48, dire, 24, 32,	PLAYERBMPDATA);
#define FRAME2 PBUFSFUN(pip[/*!*/(pip->video->p)].page, x, y, 24, dire, 24, 32,	PLAYERBMPDATA);
#define FRAME3 PBUFSFUN(pip[/*!*/(pip->video->p)].page, x, y, 0, dire, 24, 32,	PLAYERBMPDATA);
#define FRAME4 PBUFSFUN(pip[/*!*/(pip->video->p)].page, x, y, 24, dire, 24, 32,	PLAYERBMPDATA);
#else
#define FRAME1 modexClearRegion(pip[/*!*/(pip->video->p)].page, x, y, 24, 32, 2+dire);
#define FRAME2 modexClearRegion(pip[/*!*/(pip->video->p)].page, x, y, 24, 32, 1+dire);
#define FRAME3 modexClearRegion(pip[/*!*/(pip->video->p)].page, x, y, 24, 32, dire);
#define FRAME4 modexClearRegion(pip[/*!*/(pip->video->p)].page, x, y, 24, 32, 1+dire);
#endif
	if(!pageflipflop)
		modexCopyPageRegion(pip[1].page, pip[0].page, x-4, y-4, x-4, y-4, 28, 36);
	else{
		//copy old bg to page0
		//modexCopyPageRegion(pip[3].page, pip[0].page, bx, by,	0, 0,	20, 36);
		//update buffer
		//modexCopyPageRegion(pip[0].page, pip[3].page, 0, 0,	x, y,	20, 36);
	}
//modexCopyPageRegion(page_t *dest, page_t *src, word sx, word sy, word dx, word dy, word width, word height);
	//modexCopyPageRegion(pip[3].page, pip[!(pip->video->p)].page, x-4, y-4, 0, 128, 28, 36);
	/*modexCopyPageRegion(pip[pip->video->p].page,
 pip[!(pip->video->p)].page, x-4, y-4, x-4, y-4, 28, 36);*/
//	else modexCopyPageRegion(pip[1].page, pip[0].page, x-4, y-4, x-4, y-4, 28, 40);
	switch(ls)
	{
		case 1:
			FRAME1
		break;
		case 2:
			FRAME2
		break;
		case 3:
			FRAME3
		break;
		case 4:
			FRAME4
		break;
	}
//	if(2>ls && ls>=1) { FRAME1 }else
//	if(3>ls && ls>=2) { FRAME2 }else
//	if(4>ls && ls>=3) { FRAME3 }else
//	if(5>ls && ls>=4) { FRAME4 }
	//modexCopyPageRegion(pip[0].page, pip[3].page, 0, 0, x, y, 24, 32);
	//printf("x=%d	y=%d	bx=%d		by=%d\n", x, y, bx, by);
	pip->video->r=1;
}

/*
 * from zcroll16.c
*/

boolean boundary_check(int x, int y, int dx, int dy, int h, int w)
{
	return (dx > 0 && (x + dx) < w) || (dx < 0 && (x + dx) >= 0) || (dy > 0 && (y + dy) < h) || (dy < 0 && (y + dy) >= 0) || (dx == dy && dx == 0);
}

boolean coll_check(int x, int y, int dx, int dy, map_view_t *map_v)
{
	// Assume everything crosses at most 1 tile at once
	return dx && 1;//crossable_tile(x + dx, map_v) || dy && crossable_tile(y + dy, map_v);
}

boolean ZC_walk(entity_t *ent, map_view_t *map_v)
{
	//return 1;
	int dx = 1;
	int dy = 1;
	switch(ent->d)
	{
		case 2:
			return 0;
		case 1:
			dx = -dx;
		case 3:
			dy = 0;
			break;
		case 0:
			dy = -dy;
		case 4:
			dx = 0;
			break;
	}
	if(coll_check(ent->x, ent->y, dx, dy,  map_v))
	{
		// Allow movement
		// Set speed
		// Start animation
		// Mark next tile as occupied
		// Mark this tile as vacant
		return 1;
	}
	return 0;
}

void player_walk(player_t *player, map_view_t *map_v){
	int dx=16, dy=16;
	if(ZC_walk(player->ent, map_v) && boundary_check(map_v->tx, map_v->ty, dx, dy, map_v->map->width - 2*map_v->page->tilesw, map_v->map->height - 2*map_v->page->tilesh))
	{
		mapScroll(map_v, player);
		// (Un)load stuff?
	}
}

void near mapScroll(map_view_t *mv, player_t *player)
{
	//word x, y;  /* coordinate for drawing */
	int c = 1;
	int delta;
	mv->delta += player->dx | player->dy;
	delta = mv->delta;
	mv->d = (player->dx) ? (player->dx > 0) ? 3 : 1 : (player->dy) ? (player->dy > 0) ? 4 : 0 : 2;
	switch(mv->d){
		case 4:
			c = -1;
			delta = -delta;
		case 0:
			if(!(delta + mv->dxThresh))
			{
				mv->delta = 0;
				mv->ty += c;
			}
			break;
		case 3:
			c = -1;
			delta = -delta;
		case 1:
			if(!(delta + mv->dyThresh))
			{
				mv->delta = 0;
				mv->tx += c;
			}
			break;
		default:
			break;
	}

	mv->video->r=1;
}
