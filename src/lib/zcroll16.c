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
#include "src/lib/zcroll16.h"

boolean boundary_check(int x, int y, int dx, int dy, int h, int w){
	return (dx > 0 && (x + dx) < w) || (dx < 0 && (x + dx) >= 0) || (dy > 0 && (y + dy) < h) || (dy < 0 && (y + dy) >= 0) || (dx == dy && dx == 0);
}

boolean coll_check(int x, int y, int dx, int dy, map_view_t *map_v){
	// Assume everything crosses at most 1 tile at once
	return dx && 1;//crossable_tile(x + dx, map_v) || dy && crossable_tile(y + dy, map_v);
}

boolean walk(entity_t *ent, map_view_t *map_v){
	return 1;
	int dx = 1;
	int dy = 1;
	switch(ent->d){
		case STOP:
			return 0;
		case LEFT:
			dx = -dx;
		case RIGHT:
			dy = 0;
			break;
		case UP:
			dy = -dy;
		case DOWN:
			dx = 0;
			break;
	}
	if(coll_check(ent->x, ent->y, dx, dy,  map_v)){
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
	int dx, dy;
	if(walk(player->ent, map_v) && boundary_check(map_v->tx, map_v->ty, dx, dy, map_v->map->width - 2*map_v->page->tilesw, map_v->map->height - 2*map_v->page->tilesh)){
		mapScroll(map_v, player);
		// (Un)load stuff?
	}
}

void mapGoTo(map_view_t *mv, int tx, int ty)
{
	return;
}

void near mapScroll(map_view_t *mv, player_t *player){
	word x, y;  /* coordinate for drawing */
	int c = 1;
	int delta;
	mv->delta += player->dx | player->dy;
	delta = mv->delta;
	mv->d = (player->dx) ? (player->dx > 0) ? RIGHT : LEFT : (player->dy) ? (player->dy > 0) ? DOWN : UP : STOP;
	switch(mv->d){
		case DOWN:
			c = -1;
			delta = -delta;
		case UP:
			if(!(delta + mv->dxThresh)){
				mv->delta = 0;
				mv->ty += c;
			}
			break;
		case RIGHT:
			c = -1;
			delta = -delta;
		case LEFT:
			if(!(delta + mv->dyThresh)){
				mv->delta = 0;
				mv->tx += c;
			}
			break;
		default:
			break;
	}

	mv->video->r=1;
}

sword chkmap(map_t *map, word q)
{
	/*
//	bitmap_t bp;
	static byte x[(MAPW*MAPH)+1] =
{ 1, 2, 3, 4, 0, 3, 3, 3, 3, 3, 3, 3, 3, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 6, 7, 8, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 10, 11, 12, 4, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 13, 14, 15, 16, 0, 1, 1, 1, 5, 8, 1, 11, 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 4, 0, 0, 0, 0, 0, 8, 8, 1, 11, 11, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 8, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 1, 2, 3, 4, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 5, 6, 7, 8, 6, 6, 6, 6, 6, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 1, 1, 1, 1, 1, 9, 10, 11, 12, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 13, 14, 15, 16, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6, 6, 6, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 10, 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 10, 10 };
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
	else map->tiles->debug_text = false;*/
	return 0;
}

//TODO: player position here

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

unsigned char shinku_fps_indicator_page = 2;
boolean pageflipflop = 1;
boolean pageploop = 1;

/*	sync	*/
void shinku(global_game_variables_t *gv)
{
	word x = (0) + gv->video.page[/*!*/(gv->video.p)].dx; // follow the screen
	word y = (0) + gv->video.page[/*!*/(gv->video.p)].dy; // follow the screen
	word w = 64, h = 8, col = 7, bgcol = 0, type = 1;
	byte o,o2,i;
	if(elapsed_timer(gv) >= (1.0 / gv->kurokku.frames_per_second))
	{
		sprintf(gv->pee, "%.0f fps", (double)gv->kurokku.tiku/ticktock(gv));
		//modexClearRegion(&(gv->video.page[shinku_fps_indicator_page]), x, y, w, h, 45);
		modexprint(&(gv->video.page[/*!*/(gv->video.p)]), x, y, type, col, bgcol, gv->pee);
		gv->kurokku.tiku=0;
	}else //copy dat sheet
	gv->kurokku.tiku++;

	switch(gv->kurokku.fpscap)
	{
		case 0:
			gv->kurokku.frames_per_second=1;
		break;
		case 1:
			//turn this off if XT
			vga_wait_for_vsync();
			gv->kurokku.frames_per_second=60;
		break;
	}
	/*
	if(pageflipflop){
		if(gv->video.r){
			if(!pageploop) modexCopyPageRegion(&(gv->video.page[(gv->video.p)]), &(gv->video.page[(!gv->video.p)]), 0, 0, 0, 0, gv->video.page[gv->video.p].width, gv->video.page[!gv->video.p].height);
			modexShowPage(&(gv->video.page[gv->video.p]));
			if(!pageploop) gv->video.p=!gv->video.p;
			gv->video.r=!gv->video.r;
		}
	}*/
}

void near animatePlayer(map_view_t *pip, player_t *player,  sword scrollswitch)
{
	sword x = player->x;
	sword y = player->y;
	sword bx = x+16;	//buffer's x
	sword by = y+16;	//buffer's y
	word dire=32; //direction
	sword qq; //scroll offset
	word ls = player->persist_aniframe;

	switch(scrollswitch)
	{
		case 0:
			qq = 0;
		break;
		default:
			qq = ((player->q)*(player->speed));
		break;
	}
	//x-=4;
	//y-=pip->map->tiles->tileHeight;
	switch (player->d)
	{
		case 0:
			//up
			dire*=player->d;
			y-=qq;
			by-=4;
		break;
		case 3:
			// right
			dire*=(player->d-2);
			x+=qq;
			bx+=4;
		break;
		case 2:
		break;
		case 4:
			//down
			dire*=(player->d-2);
			y+=qq;
			by+=4;
		break;
		case 1:
			//left
			dire*=(player->d+2);
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
		modexCopyPageRegion(pip->page, pip->page, x-4, y-4, x-4, y-4, 28, 36);
	else{
		//copy old bg to page0
		//modexCopyPageRegion(pip[3].page, pip[0].page, bx, by,	0, 0,	20, 36);
		//update buffer
		//modexCopyPageRegion(pip[0].page, pip[3].page, 0, 0,	x, y,	20, 36);
	}
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
	pip->video->r=1;
}
