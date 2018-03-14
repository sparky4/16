/* Project 16 Source Code~
 * Copyright (C) 2012-2018 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

//check map edge
boolean ZCL_mapEdgeChk(map_view_t *map_v, nibble dir, int tx, int ty, boolean pansw, boolean noscrollsw)
{
	int	w,h;

	switch (pansw)
	{
		case 0:
			w = map_v[0].map->width;
			h = map_v[0].map->height;
		break;
		case 1:
			w = map_v[0].page->ti.tilesw;
			h = map_v[0].page->ti.tilesh;
		break;
	}

	switch(dir)
	{
		default://no direction
		case 2:
			//0000pip[0].video->startclk = (*clockw);
			return false;
		break;
		//right movement
		case 3:
			if(noscrollsw) return tx < map_v[0].map->width;
			else return (map_v[0].tx >= 0 && map_v[0].tx+map_v[0].page->ti.tw < w);
		break;
		//left movement
		case 1:
			if(noscrollsw) return tx > 1;
			else return (map_v[0].tx > 0 && map_v[0].tx+map_v[0].page->ti.tw <= w);
		break;
		//down movement
		case 4:
			if(noscrollsw) return ty < map_v[0].map->height;
			else return (map_v[0].ty >= 0 && map_v[0].ty+map_v[0].page->ti.th < h);
		break;
		//up movement
		case 0:
			if(noscrollsw) return ty > 1;
			else return (map_v[0].ty > 0 && map_v[0].ty+map_v[0].page->ti.th <= h);
		break;
	}
	return false;
}

boolean ZCL_CollCheck(map_view_t *map_v, nibble dir, int tx, int ty)
{
	switch (dir)
	{
		default://no direction
		case 2:
		break;
		case 3://right
			return !(map_v[0].map->layerdata[0].data[(tx)+(map_v[0].map->width*(ty-1))] == 0);
		break;
		case 1://left
			return !(map_v[0].map->layerdata[0].data[(tx-2)+(map_v[0].map->width*(ty-1))] == 0);
		break;
		case 4://down
			return !(map_v[0].map->layerdata[0].data[(tx-1)+(map_v[0].map->width*(ty))] == 0);
		break;
		case 0://up
			return !(map_v[0].map->layerdata[0].data[(tx-1)+(map_v[0].map->width*(ty-2))] == 0);
		break;
	}
	return false;
}

boolean ZCL_ScreenMidPosChk(map_view_t *map_v, nibble dir, int tx, int ty)
{
	if(ZCL_mapEdgeChk(map_v, dir, 0, 0, 0, 0))
		switch(dir)
		{
			default://no direction
			case 2:
				//0000map_v[0].video->startclk = (*clockw);
				return false;
			break;
			//right movement
			case 3:
				return tx == map_v[0].tx+map_v[0].page->ti.tilemidposscreenx;
			break;
			//left movement
			case 1:
				return tx == map_v[0].tx+map_v[0].page->ti.tilemidposscreenx;
			break;
			//down movement
			case 4:
				return ty == map_v[0].ty+map_v[0].page->ti.tilemidposscreeny;
			break;
			//up movement
			case 0:
				return ty == map_v[0].ty+map_v[0].page->ti.tilemidposscreeny;
			break;
		}
	return false;
}

//#define INC_PER_FRAME if(gvar->player[pn].enti.q&1) gvar->player[pn].enti.persist_aniframe++; if(gvar->player[pn].enti.persist_aniframe>4) gvar->player[pn].enti.persist_aniframe = 1;
#ifdef OLDWALKSHOWPAGESTUFF
#define SHOWMVFUN__ VL_ShowPage(&gvar->video.page[0], 1, 0);//modexShowPage(&gvar->video.page);
//ZC_ShowMV(gvar->mv, 1, 0);
#endif

void ZC_walk(global_game_variables_t *gvar, word pn)
{
	switch(gvar->player[pn].enti.d)
	{
		//no direction
		case 2:
			//0000gvar->mv[0].video->startclk = (*clockw);
		break;
		//right movement
		case 3:
			if(ZCL_ScreenMidPosChk(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty) &&
			ZCL_CollCheck(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty))//!(gvar->player[pn].enti.tx+1 == TRIGGX && gvar->player[pn].enti.ty == TRIGGY))	//collision detection!
			{
				gvar->player[pn].walktype=2;
				if(gvar->player[pn].enti.q<=gvar->player[pn].enti.spt)
				{
					ZC_animatePlayer(gvar->mv, gvar->player, pn);
					ZC_mapScroll(gvar->mv, gvar->player, pn);
					gvar->player[pn].enti.q++;
					//0000gvar->mv[0].video->clk = ((*clockw)-gvar->mv[0].video->startclk)/18.2;
#ifdef OLDWALKSHOWPAGESTUFF
					SHOWMVFUN__
#endif
				} else { gvar->player[pn].enti.q = 1; gvar->player[pn].enti.d = 2; gvar->player[pn].enti.tx++; }
			}
			else if(ZCL_mapEdgeChk(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty, 0, 1) &&
			ZCL_CollCheck(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty))//!(gvar->player[pn].enti.tx+1 == TRIGGX && gvar->player[pn].enti.ty == TRIGGY))
			{
				gvar->player[pn].walktype=1;
				if(gvar->player[pn].enti.q<=gvar->player[pn].enti.spt)
				{
					gvar->player[pn].enti.x+=(gvar->player[pn].enti.spt);
					ZC_animatePlayer(gvar->mv, gvar->player, pn);
					gvar->player[pn].enti.q++;
#ifdef OLDWALKSHOWPAGESTUFF
					SHOWMVFUN__
#endif
				} else { gvar->player[pn].enti.q = 1; gvar->player[pn].enti.d = 2; gvar->player[pn].enti.tx++; }
			}
			else
			{
				gvar->player[pn].walktype=0;
				ZC_animatePlayer(gvar->mv, gvar->player, pn);
				gvar->player[pn].enti.d = 2;
#ifdef OLDWALKSHOWPAGESTUFF
				SHOWMVFUN__
#endif
			}
			gvar->player[pn].enti.triggerx = gvar->player[pn].enti.tx+1;
			gvar->player[pn].enti.triggery = gvar->player[pn].enti.ty;
		break;
		//left movement
		case 1:
			if(ZCL_ScreenMidPosChk(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty) &&
			ZCL_CollCheck(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty))//!(gvar->player[pn].enti.tx-1 == TRIGGX && gvar->player[pn].enti.ty == TRIGGY))	//collision detection!
			{
				gvar->player[pn].walktype=2;
				if(gvar->player[pn].enti.q<=gvar->player[pn].enti.spt)
				{
					ZC_animatePlayer(gvar->mv, gvar->player, pn);
					ZC_mapScroll(gvar->mv, gvar->player, pn);
					gvar->player[pn].enti.q++;
					//0000gvar->mv[0].video->clk = ((*clockw)-gvar->mv[0].video->startclk)/18.2;
#ifdef OLDWALKSHOWPAGESTUFF
					SHOWMVFUN__
#endif
				} else { gvar->player[pn].enti.q = 1; gvar->player[pn].enti.d = 2; gvar->player[pn].enti.tx--; }
			}
			else if(ZCL_mapEdgeChk(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty, 0, 1) &&
			ZCL_CollCheck(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty))//!(gvar->player[pn].enti.tx-1 == TRIGGX && gvar->player[pn].enti.ty == TRIGGY))
			{
				gvar->player[pn].walktype=1;
				if(gvar->player[pn].enti.q<=gvar->player[pn].enti.spt)
				{
					gvar->player[pn].enti.x-=(gvar->player[pn].enti.spt);
					ZC_animatePlayer(gvar->mv, gvar->player, pn);
					gvar->player[pn].enti.q++;
#ifdef OLDWALKSHOWPAGESTUFF
					SHOWMVFUN__
#endif
				} else { gvar->player[pn].enti.q = 1; gvar->player[pn].enti.d = 2; gvar->player[pn].enti.tx--; }
			}
			else
			{
				gvar->player[pn].walktype=0;
				ZC_animatePlayer(gvar->mv, gvar->player, pn);
				gvar->player[pn].enti.d = 2;
#ifdef OLDWALKSHOWPAGESTUFF
				SHOWMVFUN__
#endif
			}
			gvar->player[pn].enti.triggerx = gvar->player[pn].enti.tx-1;
			gvar->player[pn].enti.triggery = gvar->player[pn].enti.ty;
		break;
		//down movement
		case 4:
			if(ZCL_ScreenMidPosChk(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty) &&
			ZCL_CollCheck(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty))//!(gvar->player[pn].enti.tx == TRIGGX && gvar->player[pn].enti.ty+1 == TRIGGY))	//collision detection!
			{
				gvar->player[pn].walktype=2;
				if(gvar->player[pn].enti.q<=gvar->player[pn].enti.spt)
				{
					ZC_animatePlayer(gvar->mv, gvar->player, pn);
					ZC_mapScroll(gvar->mv, gvar->player, pn);
					gvar->player[pn].enti.q++;
					//0000gvar->mv[0].video->clk = ((*clockw)-gvar->mv[0].video->startclk)/18.2;
#ifdef OLDWALKSHOWPAGESTUFF
					SHOWMVFUN__
#endif
				} else { gvar->player[pn].enti.q = 1; gvar->player[pn].enti.d = 2; gvar->player[pn].enti.ty++; }
			}
			else if(ZCL_mapEdgeChk(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty, 0, 1) &&
			ZCL_CollCheck(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty))//!(gvar->player[pn].enti.tx == TRIGGX && gvar->player[pn].enti.ty+1 == TRIGGY))
			{
				gvar->player[pn].walktype=1;
				if(gvar->player[pn].enti.q<=gvar->player[pn].enti.spt)
				{
					gvar->player[pn].enti.y+=(gvar->player[pn].enti.spt);
					ZC_animatePlayer(gvar->mv, gvar->player, pn);
					gvar->player[pn].enti.q++;
#ifdef OLDWALKSHOWPAGESTUFF
					SHOWMVFUN__
#endif
				} else { gvar->player[pn].enti.q = 1; gvar->player[pn].enti.d = 2; gvar->player[pn].enti.ty++; }
			}
			else
			{
				gvar->player[pn].walktype=0;
				ZC_animatePlayer(gvar->mv, gvar->player, pn);
				gvar->player[pn].enti.d = 2;
#ifdef OLDWALKSHOWPAGESTUFF
				SHOWMVFUN__
#endif
			}
			gvar->player[pn].enti.triggerx = gvar->player[pn].enti.tx;
			gvar->player[pn].enti.triggery = gvar->player[pn].enti.ty+1;
		break;
		//up movement
		case 0:
			if(ZCL_ScreenMidPosChk(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty) &&
			ZCL_CollCheck(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty))//!(gvar->player[pn].enti.tx == TRIGGX && gvar->player[pn].enti.ty-1 == TRIGGY))	//collision detection!
			{
				gvar->player[pn].walktype=2;
				if(gvar->player[pn].enti.q<=gvar->player[pn].enti.spt)
				{
					ZC_animatePlayer(gvar->mv, gvar->player, pn);
					ZC_mapScroll(gvar->mv, gvar->player, pn);
					gvar->player[pn].enti.q++;
					//0000gvar->mv[0].video->clk = ((*clockw)-gvar->mv[0].video->startclk)/18.2;
#ifdef OLDWALKSHOWPAGESTUFF
					SHOWMVFUN__
#endif
				} else { gvar->player[pn].enti.q = 1; gvar->player[pn].enti.d = 2; gvar->player[pn].enti.ty--; }
			}
			else if(ZCL_mapEdgeChk(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty, 0, 1) &&
			ZCL_CollCheck(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty))//!(gvar->player[pn].enti.tx == TRIGGX &&  gvar->player[pn].enti.ty-1 == TRIGGY))
			{
				gvar->player[pn].walktype=1;
				if(gvar->player[pn].enti.q<=gvar->player[pn].enti.spt)
				{
					gvar->player[pn].enti.y-=(gvar->player[pn].enti.spt);
					ZC_animatePlayer(gvar->mv, gvar->player, pn);
					gvar->player[pn].enti.q++;
#ifdef OLDWALKSHOWPAGESTUFF
					SHOWMVFUN__
#endif
				} else { gvar->player[pn].enti.q = 1; gvar->player[pn].enti.d = 2; gvar->player[pn].enti.ty--; }
			}
			else
			{
				gvar->player[pn].walktype=0;
				ZC_animatePlayer(gvar->mv, gvar->player, pn);
				gvar->player[pn].enti.d = 2;
#ifdef OLDWALKSHOWPAGESTUFF
				SHOWMVFUN__
#endif
			}
			gvar->player[pn].enti.triggerx = gvar->player[pn].enti.tx;
			gvar->player[pn].enti.triggery = gvar->player[pn].enti.ty-1;
		break;
	}
}


//panning page
void ZC_panPageManual(map_view_t *pip, player_t *player, word pn)
{
#define SHOWMVFUN_ VL_ShowPage(pip[0].page, 0, 0);//ZC_ShowMV(pip, 0, 0);
	switch(player[pn].enti.d)
	{
		//right movement
		case 3:
			if(ZCL_mapEdgeChk(pip, player[pn].enti.d, player[pn].enti.tx, player[pn].enti.ty, 1, 0))
			{
				if(player[pn].enti.q<=player[pn].enti.spt)
				{
					pip[0].page->dx+=4;
					SHOWMVFUN_;
					player[pn].enti.q++;
				} else { player[pn].enti.q = 1; player[pn].enti.d = 2; pip[0].tx++; }
			}
		break;

		//left movement
		case 1:
			if(ZCL_mapEdgeChk(pip, player[pn].enti.d, player[pn].enti.tx, player[pn].enti.ty, 1, 0))
			{
				if(player[pn].enti.q<=player[pn].enti.spt)
				{
					pip[0].page->dx-=4;
					SHOWMVFUN_;
					player[pn].enti.q++;
				} else { player[pn].enti.q = 1; player[pn].enti.d = 2; pip[0].tx--; }
			}
		break;

		//down movement
		case 4:
			if(ZCL_mapEdgeChk(pip, player[pn].enti.d, player[pn].enti.tx, player[pn].enti.ty, 1, 0))
			{
				if(player[pn].enti.q<=player[pn].enti.spt)
				{
					pip[0].page->dy+=4;
					SHOWMVFUN_;
					player[pn].enti.q++;
				} else { player[pn].enti.q = 1; player[pn].enti.d = 2; pip[0].ty++; }
			}
		break;

		//up movement
		case 0:
			if(ZCL_mapEdgeChk(pip, player[pn].enti.d, player[pn].enti.tx, player[pn].enti.ty, 1, 0))
			{
				if(player[pn].enti.q<=player[pn].enti.spt)
				{
					pip[0].page->dy-=4;
					SHOWMVFUN_;
					player[pn].enti.q++;
				} else { player[pn].enti.q = 1; player[pn].enti.d = 2; pip[0].ty--; }
			}
		break;
	}
}

/*
 * modex mv setup
*/
void ZC_MVSetup(map_view_t *pip, map_t *map, global_game_variables_t *gv)
{
#define ZC_MVI 1
	nibble i;
	// 1st page
	pip[0].page		= &gv->video.page[0];
	pip[0].map		= map;
	pip[0].video		= &gv->video;
	pip[0].kurokku	= &gv->kurokku;
	pip[0].p		= &gv->video.p;
	pip[0].sp		= &gv->video.sp;
	ZC_MVInit(pip, 1, 1);

	for(i=ZC_MVI;i<gv->video.num_of_pages;i++)
	{
		pip[i].page	=	&gv->video.page[i];
		pip[i].map	=	pip[0].map;
		pip[i].video	=	pip[0].video;
		pip[i].p	=	pip[0].p;
		pip[i].sp	=	pip[0].sp;
//		if(i>ZC_MVI) {
		pip[i].tx =	1;
		pip[i].ty =	1;
//		}
	}
}

void ZC_MVInit(map_view_t *pip, int tx, int ty)
{
	pip[0].tx = tx;
	pip[0].ty = ty;
	//pip[0].tx = pip[1].tx = tx;
	//pip[0].ty = pip[1].ty = ty;
}

//#if 0
//for backwards compatibility with code i am lazy ww
void ZC_ShowMV(map_view_t *moo, boolean vsync, boolean sr)
{
	VL_ShowPage(moo[0].page, vsync, sr);
}

#if 0
void
initMap(map_t *map) {
	// just a place holder to fill out an alternating pattern
	int x, y;
	int i;
	int tile = 1;

	i=0;
	for(y=0; y<map->height; y++) {
		for(x=0; x<map->width; x++) {
			map->layerdata[0].data[i]=255;
//			printf("[%d]", map->layerdata[0].data[i]);
			tile = tile ? 0 : 1;
			i++;
		}
		tile = tile ? 0 : 1;
	}
}

void
initMap(map_t *map) {
	/* just a place holder to fill out an alternating pattern */
	int x, y;
	int i;
	int tile = 1;
	map->tiles = malloc(sizeof(tiles_t));
	map->width=20;
	map->height=15;

	/* create the tile set */
	map->tiles->spri = malloc(sizeof(sprite_t));
	map->tiles->spri->sprite_vrl_cont.vrl_header->width = (TILEWH*2);
	map->tiles->spri->sprite_vrl_cont.vrl_header->height= TILEWH;
	map->tiles->spri->sprite_vrl_cont.buffer = malloc((TILEWH*2)*TILEWH);
	map->tiles->tileHeight = TILEWH;
	map->tiles->tileWidth =TILEWH*2;
	map->tiles->rows = 1;
	map->tiles->cols = 2;
//#if 0
	i=0;
	for(y=0; y<TILEWH; y++) {
	for(x=0; x<(TILEWH*2); x++) {
		if(x<TILEWH)
			map->tiles->spri->sprite_vrl_cont.buffer[i] = 28;//0x24;
		else
			map->tiles->spri->sprite_vrl_cont.buffer[i] = 0;//0x34;
		i++;
	}
	}
//#endif
	i=0;
	printf("pee\n");
	for(y=0; y<map->height; y++) {
		for(x=0; x<map->width; x++) {
			map->layerdata[0].data[i] = tile;
//			printf("%u ", map->layerdata[0].data[i]);
			tile = tile ? 0 : 1;
			i++;
		}
//		printf("\n");
		tile = tile ? 0 : 1;
	}
}
#endif
//===========================================================================//


void near mapScrollRight(map_view_t *mv, player_t *player, word id, word plid)
{
	word x;//, y;  /* coordinate for drawing */

	ScrollRight(mv, player, id, plid);

	/* draw the next column */
	x= mv[0].page->sw + mv[0].map->tiles->tileWidth;
	if(ZCL_mapEdgeChk(mv, player[plid].enti.d, player[plid].enti.tx, player[plid].enti.ty, 0, 0))
#ifndef FULLRCREND
	if(player[plid].enti.q%4)
#else
	if(player[plid].enti.q==4)
#endif
//		if(id==0)
			mapDrawCol(&mv[id], mv[0].tx + mv[0].page->ti.tw, mv[0].ty-1, x, player);
//		else
//			if(mv[0].video->vga_state.bgps)
//				modexCopyPageRegion(mv->page, mv[0].page, x, 0, x, 0, mv[0].map->tiles->tileWidth, mv[0].page->height);
	//mv[0].video->dorender=1;//ScrollRight
}


void near mapScrollLeft(map_view_t *mv, player_t *player, word id, word plid)
{
	word x;//,y;  /* coordinate for drawing */

	ScrollLeft(mv, player, id, plid);

	/* draw the next column */
	x= 0;
	if(ZCL_mapEdgeChk(mv, player[plid].enti.d, player[plid].enti.tx, player[plid].enti.ty, 0, 0))
#ifndef FULLRCREND
	if(player[plid].enti.q%4)
#else
	if(player[plid].enti.q==4)
#endif
//		if(id==0)
			mapDrawCol(&mv[id], mv[0].tx - 1, mv[0].ty-1, x, player);
//		else
//			if(mv[0].video->vga_state.bgps)
//				modexCopyPageRegion(mv->page, mv[0].page, x, 0, x, 0, mv[0].map->tiles->tileWidth, mv[0].page->height);
	//mv[0].video->dorender=1;//ScrollLeft
}


void near mapScrollUp(map_view_t *mv, player_t *player, word id, word plid)
{
	word y;//x,  /* coordinate for drawing */

	ScrollUp(mv, player, id, plid);

	/* draw the next row */
	y= 0;
	if(ZCL_mapEdgeChk(mv, player[plid].enti.d, player[plid].enti.tx, player[plid].enti.ty, 0, 0))
#ifndef FULLRCREND
	if(player[plid].enti.q%3)
#else
	if(player[plid].enti.q==4)
#endif
//		if(id==0)
			mapDrawRow(&mv[id], mv[0].tx - 1, mv[0].ty-1, y, player);
//		else
//			if(mv[0].video->vga_state.bgps)
//				modexCopyPageRegion(mv->page, mv[0].page, 0, y, 0, y, mv[0].page->width, mv[0].map->tiles->tileHeight);
	//mv[0].video->dorender=1;//ScrollUp
}


void near mapScrollDown(map_view_t *mv, player_t *player, word id, word plid)
{
	word y;//x,  /* coordinate for drawing */

	ScrollDown(mv, player, id, plid);

	/* draw the next row */
	y= mv[0].page->sh + mv[0].map->tiles->tileHeight;
	if(ZCL_mapEdgeChk(mv, player[plid].enti.d, player[plid].enti.tx, player[plid].enti.ty, 0, 0))
#ifndef FULLRCREND
	if(player[plid].enti.q%3)
#else
	if(player[plid].enti.q==4)
#endif
//		if(id==0)
			mapDrawRow(&mv[id], mv[0].tx - 1, mv[0].ty+mv[0].page->ti.th, y, player);
//		else
//			if(mv[0].video->vga_state.bgps)
//				modexCopyPageRegion(mv->page, mv[0].page, 0, y, 0, y, mv[0].page->width, mv[0].map->tiles->tileHeight);
	//mv[0].video->dorender=1;//ScrollDown
}

void ZC_mapScroll(map_view_t *mv, player_t *player, word pn)
{
	word b = mv[0].video->vga_state.bgps;
	switch(player[pn].enti.d)
	{
		case 2://none
		break;
		case 3://right
				ScrollRight(mv, player, 3, pn);
				ScrollRight(mv, player, 2, pn);
				mapScrollRight(mv, player, (0), pn);
			if(b)	mapScrollRight(mv, player, (1), pn);
		break;
		case 1://left
				ScrollLeft(mv, player, 3, pn);
				ScrollLeft(mv, player, 2, pn);
				mapScrollLeft(mv, player, (0), pn);
			if(b)	mapScrollLeft(mv, player, (1), pn);
		break;
		case 4://down
				ScrollDown(mv, player, 3, pn);
				ScrollDown(mv, player, 2, pn);
				mapScrollDown(mv, player, (0), pn);
			if(b)	mapScrollDown(mv, player, (1), pn);
		break;
		case 0://up
				ScrollUp(mv, player, 3, pn);
				ScrollUp(mv, player, 2, pn);
				mapScrollUp(mv, player, (0), pn);
			if(b)	mapScrollUp(mv, player, (1), pn);
		break;
	}
	//mv[0].video->dorender=1;//ZC_mapScroll
}

//===========================================================================
//TODO: put player in starting position of assigned spot on map
//default player position on the viewable map
void ZC_PlayerXYpos(int x, int y, player_t *player, map_view_t *pip, boolean defaultsw)
{
	player->enti.tx = x + pip[0].tx;
	player->enti.ty = y + pip[0].ty;

	switch(defaultsw)
	{
		case 1:
			player->enti.tx += pip[0].page->ti.tilemidposscreenx;
			player->enti.ty += pip[0].page->ti.tilemidposscreeny;
		break;
		case 0:
		break;
	}
}
//===========================================================================

void ZC_checkardmap(map_t *map)
{
	// just a place holder to fill out an alternating pattern
	int x, y;
	int i;
	static byte z[(MAPW*MAPH)];//map->width*map->height];
	byte tile = 1;

	i=0;
	for(y=0; y<map->height; y++) {
		for(x=0; x<map->width; x++) {
			z[i]=tile+1;
			tile = tile ? 0 : 1;
			i++;
		}
		tile = tile ? 0 : 1;
	}
	map->layerdata[0].data = &z;
}

sword chkmap(map_t *map, word q)
{
//	bitmap_t bp;
//	static byte x[(MAPW*MAPH)+1] =
//{ 1, 2, 3, 4, 0, 3, 3, 3, 3, 3, 3, 3, 3, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 5, 6, 7, 8, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 10, 11, 12, 4, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 13, 14, 15, 16, 0, 1, 1, 1, 5, 8, 1, 11, 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 4, 0, 0, 0, 0, 0, 8, 8, 1, 11, 11, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 8, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 1, 2, 3, 4, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 5, 6, 7, 8, 6, 6, 6, 6, 6, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 1, 1, 1, 1, 1, 9, 10, 11, 12, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 13, 14, 15, 16, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 6, 6, 6, 6, 6, 6, 6, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 3, 3, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 3, 3, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 10, 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 10, 10 };
//1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1 };
//1,	2,	3,	4,	0,	3,	3,	3,	3,	3,	3,	3,	3,	4,	1,	1,	1,	1,	1,	1,	5,	6,	7,	8,	0,	1,	0,	1,	0,	0,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	9,	10,	11,	12,	4,	1,	0,	1,	0,	0,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	13,	14,	15,	16,	0,	1,	1,	1,	5,	8,	1,	11,	11,	1,	1,	1,	1,	1,	1,	1,	0,	0,	4,	0,	0,	0,	0,	0,	8,	8,	1,	11,	11,	3,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	0,	0,	0,	0,	8,	8,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	0,	0,	0,	0,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	0,	0,	0,	0,	1,	1,	1,	1,	1,	1,	1,	1,	3,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	3,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	3,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	3,	3,	3,	3,	1,	2,	3,	4 };
	//check for failed to load map
	if((!map->width || !map->height) && (q>0))
	{
		//initiate a null map!
		map->width=MAPW;
		map->height=MAPH;
//		map->layerdata[0].data = fmfaflflfofcf(((map->width*map->height)+1)*sizeof(byte));
		ZC_checkardmap(map);//map->layerdata[0].data = &x;
		//fix this to be far~
//		bp = bitmapLoadPcx("data/ed.pcx");
//		map->tiles->data = &bp;
		map->tiles->tileHeight = 16;
		map->tiles->tileWidth = 16;
		map->tiles->rows = 1;
		map->tiles->cols = 1;
	}
	return 0;
}

//TODO: player position here
void mapGoTo(map_view_t *mv, int tx, int ty)
{
	ZC_mapinitMV(mv, tx, ty);

	/* draw the tiles */
	modexClearRegion(mv[0].page, 0, 0, mv[0].page->width, mv[0].page->height, 0);
	ZC_mapredraw(mv, tx, ty);

	//if(mv[0].video->vga_state.bgps) modexCopyPageRegion(mv[1].page, mv[0].page, 0, 0, 0, 0, mv[0].page->width, mv[0].page->height);
}

void ZC_mapinitMV(map_view_t *mv, int tx, int ty)
{
	/* set up the coordinates */
	mv[0].tx = mv[1].tx = tx;
	mv[0].ty = mv[1].ty = ty;
	mv[0].page->dx = mv[1].page->dx = mv[2].page->dx = mv[3].page->dx = mv->map->tiles->tileWidth;
	mv[0].page->dy = mv[1].page->dy = mv[2].page->dy = mv[3].page->dy = mv->map->tiles->tileHeight;

	/* set up the thresholds */
	mv[0].dxThresh = mv[1].dxThresh = mv[2].dxThresh = mv[3].dxThresh = mv->map->tiles->tileWidth * 2;
	mv[0].dyThresh = mv[1].dyThresh = mv[2].dyThresh = mv[3].dyThresh = mv->map->tiles->tileHeight * 2;
}

void ZC_mapredraw(map_view_t *mv, int tx, int ty)
{
	int py;//px,
	unsigned int i;

	py=0;
	i=mv[0].ty * mv[0].map->width + mv[0].tx;
	for(ty=mv[0].ty-1; py < mv[0].page->sh+mv->dyThresh && ty < mv[0].map->height; ty++, py+=mv[0].map->tiles->tileHeight) {
		mapDrawWRow(&mv[0], tx-1, ty, py);
		mapDrawWRow(&mv[1], tx-1, ty, py);
		i+=mv->map->width - tx;
	}
}

void near
mapDrawTile(tiles_t *t, word i, page_t *page, word x, word y)
{
	word rx, ry;
//	printf("%02d ", i); if(x >= page->width - t->tileWidth) printf("\n");
	switch(i)
	{
		case 0:
			//wwww
			modexClearRegion(page, x, y, t->tileWidth, t->tileHeight, 1); //currently the over scan color!
		break;
		default:
			rx = (((i-1) % ((t->spri->sprite_vrl_cont.vrl_header->width)/t->tileWidth)) * t->tileWidth);
			ry = (((i-1) / ((t->spri->sprite_vrl_cont.vrl_header->height)/t->tileHeight)) * t->tileHeight);
#ifndef TILERENDER
#ifdef __DEBUG_RF__
			if(!dbg_pagenorendermap)
#endif
				modexClearRegion(page, x, y, t->tileWidth, t->tileHeight, i+1);
#else
			modexDrawBmpRegion		(page, x, y, rx, ry, t->tileWidth, t->tileHeight, i);
			//draw_vrl1_vgax_modex(x-rx,y-ry,vrl_header,vrl_lineoffs,buffer+sizeof(*vrl_header),bufsz-sizeof(*vrl_header));
			//modexDrawBmpRegion	(page, x, y, rx, ry, t->tileWidth, t->tileHeight, (t->data));
#endif
#ifdef __DEBUG_MAP__
			if(dbg_maptext){ sprintf(global_temp_status_text2, "%u", i); modexprint(page, x, y, 1, 0, 1, 2, 1, global_temp_status_text2); }
#endif
		break;
	}
}
#define PALMAPDRAWW 13
void near mapDrawRow(map_view_t *mv, int tx, int ty, word y, player_t *player)
{
	int i;
#if defined(FULLRCREND) || defined(__DEBUG_RF__)
	word drawtileoffset=0;
//printf("y: %d	", drawtileoffset);
	drawtileoffset%=player[0].enti.spt;
//printf("%d\n", drawtileoffset);
#endif
#ifdef __DEBUG_RF__
if(dbg_pagedelayrendermap){		if(!y)	y+=TILEWH;	else	y-=TILEWH;
	sprintf(global_temp_status_text, "%-3u", drawtileoffset); modexprint(mv->page, player[0].enti.x, player[0].enti.y-28-(drawtileoffset*8) , 1, 0, PALMAPDRAWW, 1, 1, global_temp_status_text); }
#endif
	/* the position within the map array */
	i=ty * mv->map->width + tx;
	/*
	*/
#ifndef FULLRCREND
	for(	mv->dx=drawtileoffset;	mv->dx<(mv->page->sw+mv->dxThresh)/(drawtileoffset+1) && tx < mv->map->width;	mv->dx+=mv->map->tiles->tileWidth, tx++) {
#else
	for(mv->dx=0;	mv->dx<mv->page->sw+mv->dxThresh && tx < mv->map->width;	mv->dx+=mv->map->tiles->tileWidth, tx++) {
#endif
		if(i>=0)
			mapDrawTile(mv->map->tiles, mv->map->layerdata[0].data[i], mv->page, mv->dx, y);
		i++; /* next! */
	}
#ifdef __DEBUG_RF__
if(dbg_pagedelayrendermap) DBG_RFDELAYMETHOD;
#endif
}








void near mapDrawCol(map_view_t *mv, int tx, int ty, word x, player_t *player)
{
	int i;
#if defined(FULLRCREND) || defined(__DEBUG_RF__)
	word drawtileoffset=0;
//printf("x: %d	", drawtileoffset);
	drawtileoffset%=player[0].enti.spt;
//printf("%d\n", drawtileoffset);
#endif
#ifdef __DEBUG_RF__
if(dbg_pagedelayrendermap){		if(!x)	x+=TILEWH;	else	x-=TILEWH;
	sprintf(global_temp_status_text, "%-3u", drawtileoffset); modexprint(mv->page, player[0].enti.x, player[0].enti.y-28-(drawtileoffset*8) , 1, 0, PALMAPDRAWW, 1, 1, global_temp_status_text); }
#endif
	/* location in the map array */
	i=ty * mv->map->width + tx;
	/* We'll copy all of the columns in the screen,
	   i + 1 row above and one below */
#ifndef FULLRCREND
	for(	mv->dy=drawtileoffset;	mv->dy<(mv->page->sh+mv->dyThresh)/(drawtileoffset+1) && ty < mv->map->height;	mv->dy+=mv->map->tiles->tileHeight, ty++) {
#else
	for(mv->dy=0;	mv->dy<mv->page->sh+mv->dyThresh && ty < mv->map->height;	mv->dy+=mv->map->tiles->tileHeight, ty++) {
#endif
		if(i>=0)
			mapDrawTile(mv->map->tiles, mv->map->layerdata[0].data[i], mv->page, x, mv->dy);
		i += mv->map->width;
	}
#ifdef __DEBUG_RF__
if(dbg_pagedelayrendermap) DBG_RFDELAYMETHOD;
#endif
}

void mapDrawWRow(map_view_t *mv, int tx, int ty, word y)
{
	int i; nibble z;

	/* the position within the map array */
	i=ty * mv->map->width + tx;
	for(mv->dx=0; mv->dx<mv->page->sw+mv->dxThresh && tx < mv->map->width; mv->dx+=mv->map->tiles->tileWidth, tx++) {
		if(i>=0){	/* we are in the map, so copy! */
			mapDrawTile(mv->map->tiles, mv->map->layerdata[0].data[i], mv->page, mv->dx, y);
			for(z=1;z<=2;z++)
				if(mv->map->layerdata[z].data[i]){
					sprintf(global_temp_status_text2, "%u", mv->map->layerdata[z].data[i]); modexprint(mv->page, mv->dx, y, 1, 0, PALMAPDRAWW, z+2, 1, global_temp_status_text2);
					//mapDrawTile(mv->map->tiles, mv->map->layerdata[z].data[i], mv->page, mv->dx, y);
				}
		}
		i++; /* next! */
	}
}

void mapDrawWCol(map_view_t *mv, int tx, int ty, word x)
{
	int i; nibble z;

	/* location in the map array */
	i=ty * mv->map->width + tx;

	/* We'll copy all of the columns in the screen,
	   i + 1 row above and one below */
	for(mv->dy=0; mv->dy<mv->page->sh+mv->dyThresh && ty < mv->map->height; mv->dy+=mv->map->tiles->tileHeight, ty++) {
		if(i>=0){	/* we are in the map, so copy away! */
			mapDrawTile(mv->map->tiles, mv->map->layerdata[0].data[i], mv->page, x, mv->dy);
			for(z=1;z<=2;z++)
				if(mv->map->layerdata[z].data[i]){
					sprintf(global_temp_status_text2, "%u", mv->map->layerdata[z].data[i]); modexprint(mv->page, x, mv->dy, 1, 0, PALMAPDRAWW, z+2, 1, global_temp_status_text2);
					//mapDrawTile(mv->map->tiles, mv->map->layerdata[z].data[i], mv->page, x, mv->dy);
				}
		}
		i += mv->map->width;
	}
}


/*	sync	*/
void shinku(global_game_variables_t *gv)
{
#ifdef SHOWFPS
	word x = gv->video.page[/*!*/(gv->video.p)].dx;	//(gv->video.page[(gv->video.p)].tlx) - // follow the screen
	word y = gv->video.page[/*!*/(gv->video.p)].dy;	//(gv->video.page[(gv->video.p)].tly) - // follow the screen
	word col = 2, bgcol = 1, type = 1;//w = 64, h = 8,
#endif
	if(elapsed_timer(gv) >= (1.0 / gv->kurokku.frames_per_second))
	{
#ifdef SHOWFPS
        // NTS: For some bizarre reason, gv->pee is not initialized, but the pointer is not NULL even
        //      though it should be. Instead it's NULL as a neAr pointer but contains a non-null
        //      segment value, so testing against NULL doesn't work. It is initialized properly if
        //      you call start_timer() though which uses neAr mAlloc. Rather than fight with that,
        //      I decided it would be better to declare a temp buffer statically and sprintf to that.
        //
        //      This fixes *** Null pointer assignment detected error message in ZCROLL.EXE on exit.
		sprintf(global_temp_status_text, "%.0f fps", (double)gv->kurokku.tiku/ticktock(gv));
		//modexprint(&(gv->video.page[/*!*/(gv->video.p)]), x, y, type, 1, col, bgcol, global_temp_status_text);
		if(gv->kurokku.wcpu)
		{
			modexCopyPageRegion(&gv->video.page[0], &gv->video.page[1],
				gv->video.page[/*!*/(gv->video.p)].dx,
				gv->video.page[/*!*/(gv->video.p)].dy,
				gv->video.page[/*!*/(gv->video.p)].dx,
				gv->video.page[/*!*/(gv->video.p)].dy,
				96, 16);
			modexprint(&(gv->video.page[/*!*/(gv->video.p)]), x, y, type, 1, col, bgcol, 1, global_temp_status_text);
//0000printf("dx=%u	dy=%u\n", gv->video.page[/*!*/(gv->video.p)].dx, gv->video.page[/*!*/(gv->video.p)].dy);
		}
#endif
		gv->kurokku.tiku=0;
	}else //copy dat sheet
	gv->kurokku.tiku++;

	//render!!
	if(gv->video.dorender)
	{//r=1
		/*if(video->bgp s)
		{
			modexCopyPageRegion(&(gv->video.page[ ]), &(gv->video.page[ ]), 0, 0, 0, 0, gv->video.page[ ].width, gv->video.page[ ].height);
		}else{
			//modexCopyPageRegion(&video->page[0], &video->page[1], enti->x, enti->y, 0, 0, 24, 32);
		}*/
#ifndef OLDWALKSHOWPAGESTUFF
		VL_ShowPage(&(gv->video.page[gv->video.sp]), gv->kurokku.fpscap, 0);
#endif
		gv->video.dorender =!gv->video.dorender;
		//0000gv->video.tickclk = ((*clockw)-gv->video.startclk)/18.2;
	}

	switch(gv->kurokku.fpscap)
	{
		case 0: //turn this off if XT
			//modexprint(&(gv->video.page[0]), x, y+8, type, 1, col, bgcol, "sanic!");
			gv->kurokku.frames_per_second=1;
		break;
		case 1:
//#ifndef OLDWALKSHOWPAGESTUFF
			//vga_wait_for_vsync();
			vga_wait_for_vsync_end();
//#else
			//modexWaitBorder();
//#endif
			gv->kurokku.frames_per_second=FPSVALUE;
		break;
	}
	PM_NextFrame(gv);
}

void near ZC_animatePlayer(map_view_t *pip, player_t *player, word pn)
{
	sword x = player[pn].enti.x;
	sword y = player[pn].enti.y;
	sword qq,dd; //scroll offset
	player[pn].enti.dire=10; //direction

	switch(player[pn].walktype)
	{
		case 0:
			dd = 0;
			qq = 0;
		break;
		case 1:
			dd = 1;
			qq = 0;
		break;
		case 2:
			dd = 1;
			qq = player[pn].enti.q*player[pn].enti.spt;
		break;
	}
	x-=4;
	y-=pip[0].map->tiles->tileHeight;
	switch (player[pn].enti.d)
	{
		case 0:
			//up
			player[pn].enti.dire*=player[pn].enti.d+1;
			y-=qq;
		break;
		case 3:
			// right
			player[pn].enti.dire*=(player[pn].enti.d-1);
			x+=qq;
		break;
		case 2:
		break;
		case 4:
			//down
			player[pn].enti.dire*=(player[pn].enti.d-1);
			y+=qq;
		break;
		case 1:
			//left
			player[pn].enti.dire*=(player[pn].enti.d+3);
			x-=qq;
		break;
	}
	player[pn].enti.dire+=dd;
	//setting xy position
	player[pn].enti.spri.x = x;
	player[pn].enti.spri.y = y;

	if((player[pn].enti.q==1 && player[pn].enti.pred != player[pn].enti.d) || !dd)//when dir changed OR when player change face direction
	{
		//0000printf("	q=%u	pred=%u	d=%u	dd=%u\n", player[pn].enti.q, player[pn].enti.pred, player[pn].enti.d, dd);
		set_anim_by_id(&player[pn].enti.spri, player[pn].enti.dire); //pip->video->sprifilei = set_anim_by_id(player[pn].enti.spri, player[pn].enti.dire);	if(pip->video->sprifilei == -1){ printf("ERROR!	%u\n", player[pn].enti.dire); return; }
		player[pn].enti.pred = player[pn].enti.d;
		if(!dd)//changed direction while NOT moving!
		{
			modexCopyPageRegion(pip[0].page, pip[1].page, x-4, y-4, x-4, y-4, 28, 40);
			animate_spri(&player[pn].enti, pip[0].video);
		}
	}

	if(pip[0].video->vga_state.bgps && dd)//if moving wwww
	{
		modexCopyPageRegion(pip[0].page, pip[1].page, x-4, y-4, x-4, y-4, 28, 40);

		//draw sprite
		animate_spri(&player[pn].enti, pip[0].video);
	}
//0000if(player[pn].enti.q<4) delay(200);
#ifdef OLDWALKSHOWPAGESTUFF
	modexWaitBorder();
#endif
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

boolean ZC_walk2(entity_t *enti, map_view_t *map_v)
{
	//return 1;
	int dx = 1;
	int dy = 1;
	switch(enti->d)
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
	if(coll_check(enti->x, enti->y, dx, dy,  map_v))
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
	if(ZC_walk2(&(player->enti), map_v) && boundary_check(map_v->tx, map_v->ty, dx, dy, map_v->map->width - 2*map_v->page->ti.tilesw, map_v->map->height - 2*map_v->page->ti.tilesh))
	{
		mapScroll(map_v, player);
		// (Un)load stuff?
	}
}

/*
 *	end
 */


void mapScroll(map_view_t *mv, player_t *player)
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
}

void ZC_GirdChk (global_game_variables_t *gvar, player_t *player)
{
	if((player->enti.q==1) &&
		!(player->enti.x%gvar->mv[0].map->tiles->tileWidth==0 && player->enti.y%gvar->mv[0].map->tiles->tileHeight==0))
		ZC_PlayerXYpos(0, 0, player, gvar->mv, 1);
		//TODO: MAKE SURE TO RESYNC PLAYER IN WALKABLE SPOT
}
