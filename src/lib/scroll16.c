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
/*
	scroll16 library~
*/
#include "src/lib/scroll16.h"

//#define INC_PER_FRAME if(player[pn].enti.q&1) player[pn].enti.persist_aniframe++; if(player[pn].enti.persist_aniframe>4) player[pn].enti.persist_aniframe = 1;

void ZC_walk(map_view_t *pip, player_t *player, word pn)
{
	switch(player[pn].enti.d)
	{
		//no direction
		case 2:
			//0000pip[0].video->startclk = (*clockw);
		break;
		//right movement
		case 3:
			if(pip[0].tx >= 0 && pip[0].tx+pip[0].page->ti.tw < pip[0].map->width && player[pn].enti.tx == pip[0].tx+pip[0].page->ti.tilemidposscreenx &&
			!(pip[0].map->layerdata[0].data[(player[pn].enti.tx)+(pip[0].map->width*(player[pn].enti.ty-1))] == 0))//!(player[pn].enti.tx+1 == TRIGGX && player[pn].enti.ty == TRIGGY))	//collision detection!
			{
				player[pn].walktype=2;
				if(player[pn].enti.q<=player[pn].enti.spt)
				{
					ZC_animatePlayer(pip, player, pn);
					ZC_mapScroll(pip, player, pn);
					player[pn].enti.q++;
					//0000pip[0].video->clk = ((*clockw)-pip[0].video->startclk)/18.2;
				} else { player[pn].enti.q = 1; player[pn].enti.d = 2; player[pn].enti.tx++; }
			}
			else if(player[pn].enti.tx < pip[0].map->width && !(pip[0].map->layerdata[0].data[(player[pn].enti.tx)+(pip[0].map->width*(player[pn].enti.ty-1))] == 0))//!(player[pn].enti.tx+1 == TRIGGX && player[pn].enti.ty == TRIGGY))
			{
				player[pn].walktype=1;
				if(player[pn].enti.q<=player[pn].enti.spt)
				{
					player[pn].enti.x+=(player[pn].enti.speed);
					ZC_animatePlayer(pip, player, pn);
					player[pn].enti.q++;
				} else { player[pn].enti.q = 1; player[pn].enti.d = 2; player[pn].enti.tx++; }
			}
			else
			{
				player[pn].walktype=0;
				ZC_animatePlayer(pip, player, pn);
				player[pn].enti.d = 2;
			}
			player[pn].enti.triggerx = player[pn].enti.tx+1;
			player[pn].enti.triggery = player[pn].enti.ty;
		break;
		//left movement
		case 1:
			if(pip[0].tx > 0 && pip[0].tx+pip[0].page->ti.tw <= pip[0].map->width && player[pn].enti.tx == pip[0].tx+pip[0].page->ti.tilemidposscreenx &&
			!(pip[0].map->layerdata[0].data[(player[pn].enti.tx-2)+(pip[0].map->width*(player[pn].enti.ty-1))] == 0))//!(player[pn].enti.tx-1 == TRIGGX && player[pn].enti.ty == TRIGGY))	//collision detection!
			{
				player[pn].walktype=2;
				if(player[pn].enti.q<=player[pn].enti.spt)
				{
					ZC_animatePlayer(pip, player, pn);
					ZC_mapScroll(pip, player, pn);
					player[pn].enti.q++;
					//0000pip[0].video->clk = ((*clockw)-pip[0].video->startclk)/18.2;
				} else { player[pn].enti.q = 1; player[pn].enti.d = 2; player[pn].enti.tx--; }
			}
			else if(player[pn].enti.tx > 1 && !(pip[0].map->layerdata[0].data[(player[pn].enti.tx-2)+(pip[0].map->width*(player[pn].enti.ty-1))] == 0))//!(player[pn].enti.tx-1 == TRIGGX && player[pn].enti.ty == TRIGGY))
			{
				player[pn].walktype=1;
				if(player[pn].enti.q<=player[pn].enti.spt)
				{
					player[pn].enti.x-=(player[pn].enti.speed);
					ZC_animatePlayer(pip, player, pn);
					player[pn].enti.q++;
				} else { player[pn].enti.q = 1; player[pn].enti.d = 2; player[pn].enti.tx--; }
			}
			else
			{
				player[pn].walktype=0;
				ZC_animatePlayer(pip, player, pn);
				player[pn].enti.d = 2;
			}
			player[pn].enti.triggerx = player[pn].enti.tx-1;
			player[pn].enti.triggery = player[pn].enti.ty;
		break;
		//down movement
		case 4:
			if(pip[0].ty >= 0 && pip[0].ty+pip[0].page->ti.th < pip[0].map->height && player[pn].enti.ty == pip[0].ty+pip[0].page->ti.tilemidposscreeny &&
			!(pip[0].map->layerdata[0].data[(player[pn].enti.tx-1)+(pip[0].map->width*(player[pn].enti.ty))] == 0))//!(player[pn].enti.tx == TRIGGX && player[pn].enti.ty+1 == TRIGGY))	//collision detection!
			{
				player[pn].walktype=2;
				if(player[pn].enti.q<=player[pn].enti.spt)
				{
					ZC_animatePlayer(pip, player, pn);
					ZC_mapScroll(pip, player, pn);
					player[pn].enti.q++;
					//0000pip[0].video->clk = ((*clockw)-pip[0].video->startclk)/18.2;
				} else { player[pn].enti.q = 1; player[pn].enti.d = 2; player[pn].enti.ty++; }
			}
			else if(player[pn].enti.ty < pip[0].map->height && !(pip[0].map->layerdata[0].data[(player[pn].enti.tx-1)+(pip[0].map->width*(player[pn].enti.ty))] == 0))//!(player[pn].enti.tx == TRIGGX && player[pn].enti.ty+1 == TRIGGY))
			{
				player[pn].walktype=1;
				if(player[pn].enti.q<=player[pn].enti.spt)
				{
					player[pn].enti.y+=(player[pn].enti.speed);
					ZC_animatePlayer(pip, player, pn);
					player[pn].enti.q++;
				} else { player[pn].enti.q = 1; player[pn].enti.d = 2; player[pn].enti.ty++; }
			}
			else
			{
				player[pn].walktype=0;
				ZC_animatePlayer(pip, player, pn);
				player[pn].enti.d = 2;
			}
			player[pn].enti.triggerx = player[pn].enti.tx;
			player[pn].enti.triggery = player[pn].enti.ty+1;
		break;
		//up movement
		case 0:
			if(pip[0].ty > 0 && pip[0].ty+pip[0].page->ti.th <= pip[0].map->height && player[pn].enti.ty == pip[0].ty+pip[0].page->ti.tilemidposscreeny &&
			!(pip[0].map->layerdata[0].data[(player[pn].enti.tx-1)+(pip[0].map->width*(player[pn].enti.ty-2))] == 0))//!(player[pn].enti.tx == TRIGGX && player[pn].enti.ty-1 == TRIGGY))	//collision detection!
			{
				player[pn].walktype=2;
				if(player[pn].enti.q<=player[pn].enti.spt)
				{
					ZC_animatePlayer(pip, player, pn);
					ZC_mapScroll(pip, player, pn);
					player[pn].enti.q++;
					//0000pip[0].video->clk = ((*clockw)-pip[0].video->startclk)/18.2;
				} else { player[pn].enti.q = 1; player[pn].enti.d = 2; player[pn].enti.ty--; }
			}
			else if(player[pn].enti.ty > 1 && !(pip[0].map->layerdata[0].data[(player[pn].enti.tx-1)+(pip[0].map->width*(player[pn].enti.ty-2))] == 0))//!(player[pn].enti.tx == TRIGGX &&  player[pn].enti.ty-1 == TRIGGY))
			{
				player[pn].walktype=1;
				if(player[pn].enti.q<=player[pn].enti.spt)
				{
					player[pn].enti.y-=(player[pn].enti.speed);
					ZC_animatePlayer(pip, player, pn);
					player[pn].enti.q++;
				} else { player[pn].enti.q = 1; player[pn].enti.d = 2; player[pn].enti.ty--; }
			}
			else
			{
				player[pn].walktype=0;
				ZC_animatePlayer(pip, player, pn);
				player[pn].enti.d = 2;
			}
			player[pn].enti.triggerx = player[pn].enti.tx;
			player[pn].enti.triggery = player[pn].enti.ty-1;
		break;
	}
}


//panning page
void ZC_panPageManual(map_view_t *pip, player_t *player, word pn)
{
#define SHOWMVFUN_ ZC_ShowMV(pip, 0, 0);
	switch(player[pn].enti.d)
	{
		//right movement
		case 3:
			if(pip[0].tx >= 0 && pip[0].tx+pip[0].page->ti.tw < pip[0].page->ti.tilesw)
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
			if(pip[0].tx > 0 && pip[0].tx+pip[0].page->ti.tw <= pip[0].page->ti.tilesw)
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
			if(pip[0].ty >= 0 && pip[0].ty+pip[0].page->ti.th < pip[0].page->ti.tilesh)
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
			if(pip[0].ty > 0 && pip[0].ty+pip[0].page->ti.th <= pip[0].page->ti.tilesh)
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
	pip[0].page = &gv->video.page[0];
	pip[0].map = map;
	pip[0].video = &gv->video;
	pip[0].p	= &gv->video.p;
	pip[0].sp	= &gv->video.sp;
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

void ZC_ShowMV(map_view_t *moo, boolean vsync, boolean sr)
{
	word high_address, low_address, offset;
	byte crtcOffset;

	// calculate offset
	offset = (word) moo[moo[0].video->sp].page->data;
	offset += moo[0].page->dy * (moo[0].page->width >> 2 );
	offset += moo[0].page->dx >> 2;

	// calculate crtcOffset according to virtual width
	switch(sr)
	{
		case 1:
			crtcOffset = moo[moo[0].video->sp].page->sw >> 3;
		break;
		default:
		case 0:
			crtcOffset = moo[0].page->width >> 3;
		break;
	}

	high_address = HIGH_ADDRESS | (offset & 0xff00);
	low_address  = LOW_ADDRESS  | (offset << 8);

	// wait for appropriate timing and then program CRTC
	if(vsync) while ((inp(INPUT_STATUS_1) & DISPLAY_ENABLE));
	outpw(CRTC_INDEX, high_address);
	outpw(CRTC_INDEX, low_address);
	outp(CRTC_INDEX, 0x13);
	outp(CRTC_DATA, crtcOffset);

	// wait for one retrace
	if(vsync) while (!(inp(INPUT_STATUS_1) & VRETRACE));

	// do PEL panning here
	outp(AC_INDEX, 0x33);
	outp(AC_INDEX, (moo[0].page->dx & 0x03) << 1);
	vga_state.vga_draw_stride_limit = vga_state.vga_draw_stride = moo[0].page->stridew;
}

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

//===========================================================================//


void near mapScrollRight(map_view_t *mv, player_t *player, word id, word plid)
{
	word x;//, y;  /* coordinate for drawing */

	ScrollRight(mv, player, id, plid);

	/* draw the next column */
	x= mv[0].page->sw + mv[0].map->MAPTILESPTR->tileWidth;
#ifndef FULLRCREND
	if(player[plid].enti.q%4)
#else
	if(player[plid].enti.q==4)
#endif
//		if(id==0)
			mapDrawCol(mv, id, mv[0].tx + mv[0].page->ti.tw, mv[0].ty-1, x, player, DRAWCOLNUM);
//		else
//			if(mv[0].video->bgps)
//				modexCopyPageRegion(mv[id].page, mv[0].page, x, 0, x, 0, mv[0].map->MAPTILESPTR->tileWidth, mv[0].page->height);
	//mv[0].video->dorender=1;//ScrollRight
}


void near mapScrollLeft(map_view_t *mv, player_t *player, word id, word plid)
{
	word x;//,y;  /* coordinate for drawing */

	ScrollLeft(mv, player, id, plid);

	/* draw the next column */
	x= 0;
#ifndef FULLRCREND
	if(player[plid].enti.q%4)
#else
	if(player[plid].enti.q==4)
#endif
//		if(id==0)
			mapDrawCol(mv, id, mv[0].tx - 1, mv[0].ty-1, x, player, DRAWCOLNUM);
//		else
//			if(mv[0].video->bgps)
//				modexCopyPageRegion(mv[id].page, mv[0].page, x, 0, x, 0, mv[0].map->MAPTILESPTR->tileWidth, mv[0].page->height);
	//mv[0].video->dorender=1;//ScrollLeft
}


void near mapScrollUp(map_view_t *mv, player_t *player, word id, word plid)
{
	word y;//x,  /* coordinate for drawing */

	ScrollUp(mv, player, id, plid);

	/* draw the next row */
	y= 0;
#ifndef FULLRCREND
	if(player[plid].enti.q%3)
#else
	if(player[plid].enti.q==4)
#endif
//		if(id==0)
			mapDrawRow(mv, id, mv[0].tx - 1, mv[0].ty-1, y, player, DRAWROWNUM);
//		else
//			if(mv[0].video->bgps)
//				modexCopyPageRegion(mv[id].page, mv[0].page, 0, y, 0, y, mv[0].page->width, mv[0].map->MAPTILESPTR->tileHeight);
	//mv[0].video->dorender=1;//ScrollUp
}


void near mapScrollDown(map_view_t *mv, player_t *player, word id, word plid)
{
	word y;//x,  /* coordinate for drawing */

	ScrollDown(mv, player, id, plid);

	/* draw the next row */
	y= mv[0].page->sh + mv[0].map->MAPTILESPTR->tileHeight;
#ifndef FULLRCREND
	if(player[plid].enti.q%3)
#else
	if(player[plid].enti.q==4)
#endif
//		if(id==0)
			mapDrawRow(mv, id, mv[0].tx - 1, mv[0].ty+mv[0].page->ti.th, y, player, DRAWROWNUM);
//		else
//			if(mv[0].video->bgps)
//				modexCopyPageRegion(mv[id].page, mv[0].page, 0, y, 0, y, mv[0].page->width, mv[0].map->MAPTILESPTR->tileHeight);
	//mv[0].video->dorender=1;//ScrollDown
}

void ZC_mapScroll(map_view_t *mv, player_t *player, word pn)
{
	word b = mv[0].video->bgps;
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
void ZC_playerXYpos(int x, int y, player_t *player, map_view_t *pip, nibble pn, boolean defaultsw)
{
	player[pn].enti.tx = x + pip[0].tx;
	player[pn].enti.ty = y + pip[0].ty;

	switch(defaultsw)
	{
		case 1:
			player[pn].enti.tx += pip[0].page->ti.tilemidposscreenx;
			player[pn].enti.ty += pip[0].page->ti.tilemidposscreeny;
		break;
		case 0:
		break;
	}
}
//===========================================================================

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
//		map->layerdata[0].data = malloc(((map->width*map->height)+1)*sizeof(byte));
		map->layerdata[0].data = &x;
		map->MAPTILESPTR = malloc(sizeof(tiles_t));
		//fix this to be far~
//		bp = bitmapLoadPcx("data/ed.pcx");
//		map->MAPTILESPTR->data = &bp;
		map->MAPTILESPTR->tileHeight = 16;
		map->MAPTILESPTR->tileWidth = 16;
		map->MAPTILESPTR->rows = 1;
		map->MAPTILESPTR->cols = 1;
#ifdef __DEBUG_MAP__
		dbg_maptext = true;
#endif
	}
#ifdef __DEBUG_MAP__
	else
	{
		dbg_maptext = false;
	}
#endif
	return 0;
}

//TODO: player position here
void mapGoTo(map_view_t *mv, int tx, int ty)
{
	ZC_mapinitMV(mv, tx, ty);

	/* draw the tiles */
	modexClearRegion(mv[0].page, 0, 0, mv[0].page->width, mv[0].page->height, 0);
	ZC_mapredraw(mv, tx, ty);

	//if(mv[0].video->bgps) modexCopyPageRegion(mv[1].page, mv[0].page, 0, 0, 0, 0, mv[0].page->width, mv[0].page->height);
}

void ZC_mapinitMV(map_view_t *mv, int tx, int ty)
{
	/* set up the coordinates */
	mv[0].tx = mv[1].tx = tx;
	mv[0].ty = mv[1].ty = ty;
	mv[0].page->dx = mv[1].page->dx = mv[2].page->dx = mv[3].page->dx = mv->map->MAPTILESPTR->tileWidth;
	mv[0].page->dy = mv[1].page->dy = mv[2].page->dy = mv[3].page->dy = mv->map->MAPTILESPTR->tileHeight;

	/* set up the thresholds */
	mv[0].dxThresh = mv[1].dxThresh = mv[2].dxThresh = mv[3].dxThresh = mv->map->MAPTILESPTR->tileWidth * 2;
	mv[0].dyThresh = mv[1].dyThresh = mv[2].dyThresh = mv[3].dyThresh = mv->map->MAPTILESPTR->tileHeight * 2;
}

void ZC_mapredraw(map_view_t *mv, int tx, int ty)
{
	int py;//px,
	unsigned int i;

	py=0;
	i=mv[0].ty * mv[0].map->width + mv[0].tx;
	for(ty=mv[0].ty-1; py < mv[0].page->sh+mv->dyThresh && ty < mv[0].map->height; ty++, py+=mv[0].map->MAPTILESPTR->tileHeight) {
		mapDrawWRow(&mv[0], tx-1, ty, py);
		mapDrawWRow(&mv[1], tx-1, ty, py);
		i+=mv->map->width - tx;
	}
}

void near
mapDrawTile(tiles_t *t, word i, page_t *page, word x, word y)
{
	word rx, ry;
	//word textx=0, texty=0;
	//if(i==0) i=2;
//	printf("%02d ", i); if(x >= page->width - t->tileWidth) printf("\n");
	switch(i)
	{
		case 0:
			//wwww
			modexClearRegion(page, x, y, t->tileWidth, t->tileHeight, 1); //currently the over scan color!
		break;
		default:
			rx = (((i-1) % ((t->pcximg->width)/t->tileWidth)) * t->tileWidth);
			ry = (((i-1) / ((t->pcximg->height)/t->tileHeight)) * t->tileHeight);
#ifndef TILERENDER
			if(!pagenorendermap) modexClearRegion(page, x, y, t->tileWidth, t->tileHeight, i+1);
#else
			modexDrawBmpRegion		(page, x, y, rx, ry, t->tileWidth, t->tileHeight, i);
			//draw_vrl1_vgax_modex(x-rx,y-ry,vrl_header,vrl_lineoffs,buffer+sizeof(*vrl_header),bufsz-sizeof(*vrl_header));
			//modexDrawBmpRegion	(page, x, y, rx, ry, t->tileWidth, t->tileHeight, (t->data));
#endif
#ifdef __DEBUG_MAP__
			if(dbg_maptext){ sprintf(global_temp_status_text2, "%u", i); modexprint(page, x, y, 1, 0, 1, 2, global_temp_status_text2); }
#endif
		break;
	}
}
#define PALMAPDRAWW 13
void near mapDrawRow(map_view_t *mv, nibble id, int tx, int ty, word y, player_t *player, word poopoffset)
{
	int i; nibble z;
if(pagedelayrendermap)		if(!y)	y+=TILEWH;	else	y-=TILEWH;
	poopoffset%=player[0].enti.speed;
//printf("y: %d\n", poopoffset);
if(pagedelayrendermap){ sprintf(global_temp_status_text, "%-3u", mv[id].dx); modexprint(mv[0].page, player[0].enti.x, player[0].enti.y-28-(poopoffset*8) , 1, 1, 2, 1, global_temp_status_text); }
	/* the position within the map array */
	i=ty * mv[id].map->width + tx;
	for(	mv[id].dx=poopoffset;	mv[id].dx<(mv[id].page->sw+mv[id].dxThresh)/(poopoffset+1) && tx < mv[id].map->width;	mv[id].dx+=mv[id].map->MAPTILESPTR->tileWidth, tx++) {
		if(i>=0){
			mapDrawTile(mv[id].map->MAPTILESPTR, mv[id].map->layerdata[0].data[i], mv[id].page, mv[id].dx, y);
			for(z=1;z<=2;z++){
				if(mv[id].map->layerdata[z].data[i]){
					sprintf(global_temp_status_text2, "%u", mv[id].map->layerdata[z].data[i]); modexprint(mv[id].page, mv[id].dx, (y+((z-1)*4))>>2, 1, 0, PALMAPDRAWW, z+2, global_temp_status_text2);
					//mapDrawTile(mv[id].map->MAPTILESPTR, mv[id].map->layerdata[z].data[i], mv[id].page, mv[id].dx, y);
				}
			}
		}
		i++; /* next! */
	}
if(pagedelayrendermap) delay(200);
}

void near mapDrawCol(map_view_t *mv, nibble id, int tx, int ty, word x, player_t *player, word poopoffset)
{
	int i; nibble z;
if(pagedelayrendermap)		if(!x)	x+=TILEWH;		else	x-=TILEWH;
	poopoffset%=player[0].enti.speed;
//printf("x: %d\n", poopoffset);
if(pagedelayrendermap){ sprintf(global_temp_status_text, "%-3u", mv[id].dy); modexprint(mv[0].page, player[0].enti.x, player[0].enti.y-28-(poopoffset*8) , 1, 1, 2, 1, global_temp_status_text); }
	/* location in the map array */
	i=ty * mv[id].map->width + tx;
	/* We'll copy all of the columns in the screen,
	   i + 1 row above and one below */
	for(	mv[id].dy=poopoffset;	mv[id].dy<(mv[id].page->sh+mv[id].dyThresh)/(poopoffset+1) && ty < mv[id].map->height;	mv[id].dy+=mv[id].map->MAPTILESPTR->tileHeight, ty++) {
		if(i>=0){
			mapDrawTile(mv[id].map->MAPTILESPTR, mv[id].map->layerdata[0].data[i], mv[id].page, x, mv[id].dy);
			for(z=1;z<=2;z++){
				if(mv[id].map->layerdata[z].data[i]){
					sprintf(global_temp_status_text2, "%u", mv[id].map->layerdata[z].data[i]); modexprint(mv[id].page, (x+((z-1)*4))>>2, mv[id].dy, 1, 0, PALMAPDRAWW, z+2, global_temp_status_text2);
					//mapDrawTile(mv[id].map->MAPTILESPTR, mv[id].map->layerdata[z].data[i], mv[id].page, x, mv[id].dy);
				}
			}
		}
		i += mv[id].map->width;
	}
if(pagedelayrendermap) delay(200);
}

void mapDrawWRow(map_view_t *mv, int tx, int ty, word y)
{
	int i; nibble z;

	/* the position within the map array */
	i=ty * mv->map->width + tx;
	for(mv->dx=0; mv->dx<mv->page->sw+mv->dxThresh && tx < mv->map->width; mv->dx+=mv->map->MAPTILESPTR->tileWidth, tx++) {
		if(i>=0){	/* we are in the map, so copy! */
			mapDrawTile(mv->map->MAPTILESPTR, mv->map->layerdata[0].data[i], mv->page, mv->dx, y);
			for(z=1;z<=2;z++){
				if(mv->map->layerdata[z].data[i]){
					sprintf(global_temp_status_text2, "%u", mv->map->layerdata[z].data[i]); modexprint(mv->page, mv->dx, y, 1, 0, PALMAPDRAWW, z+2, global_temp_status_text2);
					//mapDrawTile(mv->map->MAPTILESPTR, mv->map->layerdata[z].data[i], mv->page, mv->dx, y);
				}
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
	for(mv->dy=0; mv->dy<mv->page->sh+mv->dyThresh && ty < mv->map->height; mv->dy+=mv->map->MAPTILESPTR->tileHeight, ty++) {
		if(i>=0){	/* we are in the map, so copy away! */
			mapDrawTile(mv->map->MAPTILESPTR, mv->map->layerdata[0].data[i], mv->page, x, mv->dy);
			for(z=1;z<=2;z++){
				if(mv->map->layerdata[z].data[i]){
					sprintf(global_temp_status_text2, "%u", mv->map->layerdata[z].data[i]); modexprint(mv->page, x, mv->dy, 1, 0, PALMAPDRAWW, z+2, global_temp_status_text2);
					//mapDrawTile(mv->map->MAPTILESPTR, mv->map->layerdata[z].data[i], mv->page, x, mv->dy);
				}
			}
		}
		i += mv->map->width;
	}
}

boolean pagenorendermap = 0;
boolean pagedelayrendermap = 0;

/*	sync	*/
void shinku(global_game_variables_t *gv)
{
	word x = gv->video.page[/*!*/(gv->video.p)].dx;	//(gv->video.page[(gv->video.p)].tlx) - // follow the screen
	word y = gv->video.page[/*!*/(gv->video.p)].dy;	//(gv->video.page[(gv->video.p)].tly) - // follow the screen
	word col = 2, bgcol = 1, type = 1;//w = 64, h = 8,
	if(elapsed_timer(gv) >= (1.0 / gv->kurokku.frames_per_second))
	{
        // NTS: For some bizarre reason, gv->pee is not initialized, but the pointer is not NULL even
        //      though it should be. Instead it's NULL as a neAr pointer but contains a non-null
        //      segment value, so testing against NULL doesn't work. It is initialized properly if
        //      you call start_timer() though which uses neAr malloc. Rather than fight with that,
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
			modexprint(&(gv->video.page[/*!*/(gv->video.p)]), x, y, type, 1, col, bgcol, global_temp_status_text);
//0000printf("dx=%u	dy=%u\n", gv->video.page[/*!*/(gv->video.p)].dx, gv->video.page[/*!*/(gv->video.p)].dy);
		}
		gv->kurokku.tiku=0;
	}else //copy dat sheet
	gv->kurokku.tiku++;

	switch(gv->kurokku.fpscap)
	{
		case 0: //turn this off if XT
			//modexprint(&(gv->video.page[0]), x, y+8, type, 1, col, bgcol, "sanic!");
			gv->kurokku.frames_per_second=1;
		break;
		case 1:
			//modexWaitBorder();
			//modexWaitBorder_start();
			//vga_wait_for_vsync();
			vga_wait_for_vsync_end();
			gv->kurokku.frames_per_second=60;
		break;
	}
	//render!!
	if(gv->video.dorender )
	{//r=1
		/*if(video->bgp s)
		{
			modexCopyPageRegion(&(gv->video.page[ ]), &(gv->video.page[ ]), 0, 0, 0, 0, gv->video.page[ ].width, gv->video.page[ ].height);
		}else{
			//modexCopyPageRegion(&video->page[0], &video->page[1], enti->x, enti->y, 0, 0, 24, 32);
		}*/
		VL_ShowPage(&(gv->video.page[gv->video.sp]), gv->kurokku.fpscap, 0);
		gv->video.dorender =!gv->video.dorender ;
		//0000gv->video.tickclk = ((*clockw)-gv->video.startclk)/18.2;
	}
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
			qq = player[pn].enti.q*player[pn].enti.speed;
		break;
	}
	x-=4;
	y-=pip[0].map->MAPTILESPTR->tileHeight;
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
	player[pn].enti.spri->x = x;
	player[pn].enti.spri->y = y;

	if((player[pn].enti.q==1 && player[pn].enti.pred != player[pn].enti.d) || !dd)//when dir changed OR when player change face direction
	{
		//0000printf("	q=%u	pred=%u	d=%u	dd=%u\n", player[pn].enti.q, player[pn].enti.pred, player[pn].enti.d, dd);
		set_anim_by_id(player[pn].enti.spri, player[pn].enti.dire); //pip->video->sprifilei = set_anim_by_id(player[pn].enti.spri, player[pn].enti.dire);	if(pip->video->sprifilei == -1){ printf("ERROR!	%u\n", player[pn].enti.dire); return; }
		player[pn].enti.pred = player[pn].enti.d;
		if(!dd)//changed direction while NOT moving!
		{
			modexCopyPageRegion(pip[0].page, pip[1].page, x-4, y-4, x-4, y-4, 28, 40);
			animate_spri(&player[pn].enti, pip[0].video);
		}
	}

	if(pip[0].video->bgps && dd)//if moving wwww
	{
		modexCopyPageRegion(pip[0].page, pip[1].page, x-4, y-4, x-4, y-4, 28, 40);

		//draw sprite
		animate_spri(&player[pn].enti, pip[0].video);
	}
//0000if(player[pn].enti.q<4) delay(200);
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
