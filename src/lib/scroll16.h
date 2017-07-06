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

#ifndef __SCROLL16_H_
#define __SCROLL16_H_

#include "src/lib/16_head.h"
#include "src/lib/16_tail.h"
#include "src/lib/bakapee.h"
#include "src/lib/16_vl.h"
#include "src/lib/16_in.h"
#include "src/lib/16_map.h" //map is loaded here www
//#include "src/lib/16render.h"
#include "src/lib/16_timer.h"
#include "src/lib/wcpu/16_wcpu.h"
#include "src/lib/16_spri.h"

#include <hw/cpu/cpu.h>
#include <hw/dos/dos.h>
#include <hw/vga/vga.h>
#include <hw/vga/vrl.h>

#define SPRITE
//#define TILERENDER

//0000
#define SHOWFPS
//#define OLDWALKSHOWPAGESTUFF


#define PLAYERBMPDATAVAR	gvar.player[pn].data
#define PLAYERBMPDATA		*PLAYERBMPDATAVAR
#define PLAYERBMPDATAPTR	PLAYERBMPDATAVAR

#define PCXBMPVAR	gvar.player[0].data
#define PCXBMP		*PCXBMPVAR
#define PCXBMPPTR	PCXBMPVAR

//row and colum drawing routines
#define FULLRCREND
#define DRAWCOLNUM mv->page[id].dx
#define DRAWROWNUM mv->page[id].dy
//#define DRAWCOLNUM pl[plid].enti.q
//#define DRAWROLNUM pl[plid].enti.q

//for null map!
#define MAPW	40
#define MAPH	30

//===========================================================================//

	//if(gvar.player[0].enti.hp==0) printf("%d wwww\n", gvar.player[0].enti.y+8);
	//else printf("\ngvar.player[0].enti.y: %d\n", gvar.player[0].enti.y);
//printf("gvar.video.p=%u ", gvar.video.p);
//not used now	printf("temporary gvar.player sprite 0: http://www.pixiv.net/member_illust.php?mode=medium&illust_id=45556867\n");
//not used now	printf("temporary gvar.player sprite 1: http://www.pixiv.net/member_illust.php?mode=medium&illust_id=44606385\n");
//printf("mv[%u].tx: %d", gvar.video.panp, mv[gvar.video.panp].tx); printf("	mv[%u].ty: %d	", gvar.video.panp, mv[gvar.video.panp].ty);
//printf("gvar.kurokku:	"); printf("%.0f ", clock());	printf("tiku=%lu ", gvar.kurokku.tiku);	printf("t=%.0f ", gvar.kurokku.t);	printf("ticktock()=%f ", ticktock(&gvar));	printf("%.0f fps", (double)gvar.kurokku.tiku/ticktock(&gvar));
//printf("map.width=%d	map.height=%d	map.data[0]=%d\n", mv[1].map->width, mv[1].map->height, mv[1].map->layerdata[0][0]);
//printf("pageflipflop=%u", pageflipflop);
//printf("&global_temp_status_text = %Fp\n", &global_temp_status_text);
//printf("middle tile position: %dx", gvar.video.page[0].tilemidposscreenx);	printf("%d\n", gvar.video.page[0].tilemidposscreeny);
//printf("	aniframe=%u", gvar.player[0].enti.persist_aniframe);
//printf("mv[0]txy: %dx%d	", mv[0].tx, mv[0].ty); printf("mv[1]txy: %dx%d", mv[1].tx, mv[1].ty);
//	printf("page[0]dxy: %dx%d", gvar.video.page[0].dx, gvar.video.page[0].dy);
#define SCROLLEXITMESG \
	printf("size of mmblocktype: %u	", sizeof(mmblocktype));\
	printf("\n\n");\
	printf("player vars:\n");\
	printf("	xy: %dx%d", gvar.player[0].enti.x, gvar.player[0].enti.y); printf("	txy: %dx%d", gvar.player[0].enti.tx, gvar.player[0].enti.ty); printf("	triggxy: %dx%d", gvar.player[0].enti.triggerx, gvar.player[0].enti.triggery); printf("	value: %d\n", gvar.mv[1].map->layerdata[0].data[(gvar.player[0].enti.triggerx-1)+(gvar.map.width*(gvar.player[0].enti.triggery-1))]);\
	printf("	hp: %d", (gvar.player[0].enti.hp));	printf("	q: %u", gvar.player[0].enti.q);	printf("	info.dir: %u", gvar.player[0].info.dir);	printf("	d: %u", gvar.player[0].enti.d);	printf("	dire: %u", gvar.player[0].enti.dire);\
		printf("	pdir: %u\n", gvar.player[0].pdir); printf("	delay=%u", gvar.player[0].enti.spri.delay);\
printf("\n\n");\
	VL_PrintmodexmemInfo(&gvar.video);\
\
	printf("gvar.video:\n");\
	printf(" r=%u ", gvar.video.dorender);\
	printf("bgps=%u ", gvar.video.vga_state.bgps);\
printf("\n\n");

//	printf("dbg_delayanimation=%u ", dbg_delayanimation);
/*
	if(IN_KeyDown(sc_F11)){ pagedelayrendermap=!pagedelayrendermap;		IN_UserInput(1, &gvar); }
	if(IN_KeyDown(sc_F10)){ gvar.kurokku.fpscap=!gvar.kurokku.fpscap;		IN_UserInput(1, &gvar); }
	if(IN_KeyDown(sc_F9)){ pagenorendermap=!pagenorendermap;				IN_UserInput(1, &gvar); }
	if(IN_KeyDown(sc_F8)){ gvar.video.vga_state.bgps=!gvar.video.vga_state.bgps;					IN_UserInput(1, &gvar); }
	if(IN_KeyDown(sc_T)){ gvar.video.vga_state.rss=!gvar.video.vga_state.rss;						IN_UserInput(1, &gvar); }
*/

extern char global_temp_status_text[512];
extern char global_temp_status_text2[512];

void initMap(map_t *map);
void ZC_walk(global_game_variables_t *gvar, word pn);
void ZC_panPageManual(map_view_t *pip, player_t *player, word pn);
void ZC_MVSetup(map_view_t *pip, map_t *map, global_game_variables_t *gv);
void ZC_MVInit(map_view_t *pip, int tx, int ty);
//void ZC_ShowMV(map_view_t *moo, boolean vsync, boolean sr);
void near mapScrollRight(map_view_t *mv, player_t *player, word id, word plid);
void near mapScrollLeft(map_view_t *mv, player_t *player, word id, word plid);
void near mapScrollUp(map_view_t *mv, player_t *player, word id, word plid);
void near mapScrollDown(map_view_t *mv, player_t *player, word id, word plid);
void ZC_mapScroll(map_view_t *mv, player_t *player, word pn);
void ZC_PlayerXYpos(int x, int y, player_t *player, map_view_t *pip, boolean defaultsw);
sword chkmap(map_t *map, word q);
void mapGoTo(map_view_t *mv, int tx, int ty);
void ZC_mapinitMV(map_view_t *mv, int tx, int ty);
void ZC_mapredraw(map_view_t *mv, int tx, int ty);
void near mapDrawTile(tiles_t *t, word i, page_t *page, word x, word y);
void near mapDrawRow(map_view_t *mv, int tx, int ty, word y, player_t *player, word poopoffset);
void near mapDrawCol(map_view_t *mv, int tx, int ty, word x, player_t *player, word poopoffset);
void mapDrawWRow(map_view_t *mv, int tx, int ty, word y);
void mapDrawWCol(map_view_t *mv, int tx, int ty, word x);
inline void near ScrollRight(map_view_t *mv, player_t *pl, word id, word plid)
{
	/* increment the pixel position and update the page */
	mv[id].page->dx += pl[plid].enti.spt;

	/* check to see if this changes the tile */
	if(mv[id].page->dx >= mv[id].dxThresh )
	{
		/* go forward one tile */
		if(id==0) mv[id].tx++;
		/* Snap the origin forward */
		mv[id].page->data += 4;

		mv[id].page->dx = mv[id].map->tiles->tileWidth;
	}
}

inline void near ScrollLeft(map_view_t *mv, player_t *pl, word id, word plid)
{
	/* decrement the pixel position and update the page */
	mv[id].page->dx -= pl[plid].enti.spt;

	/* check to see if this changes the tile */
	if(mv[id].page->dx == 0)
	{
		/* go backward one tile */
		if(id==0) mv[id].tx--;
		/* Snap the origin backward */
		mv[id].page->data -= 4;

		mv[id].page->dx = mv[id].map->tiles->tileWidth;
	}
}

inline void near ScrollUp(map_view_t *mv, player_t *pl, word id, word plid)
{
	/* decrement the pixel position and update the page */
	mv[id].page->dy -= pl[plid].enti.spt;

	/* check to see if this changes the tile */
	if(mv[id].page->dy == 0 )
	{
		/* go up one tile */
		if(id==0) mv[id].ty--;
		/* Snap the origin upward */
		mv[id].page->data -= mv[id].page->pi;

		mv[id].page->dy = mv[id].map->tiles->tileHeight;
	}
}

inline void near ScrollDown(map_view_t *mv, player_t *pl, word id, word plid)
{
	/* increment the pixel position and update the page */
	mv[id].page->dy += pl[plid].enti.spt;

	/* check to see if this changes the tile */
	if(mv[id].page->dy >= mv[id].dyThresh )
	{
		/* go down one tile */
		if(id==0) mv[id].ty++;
		/* Snap the origin downward */
		mv[id].page->data += mv[id].page->pi;

		mv[id].page->dy = mv[id].map->tiles->tileHeight;
	}
}

//void qclean();
//sync
void shinku(global_game_variables_t *gv);
//animate the sprite
void near ZC_animatePlayer(map_view_t *pip, player_t *player, word pn);

// Move an entity around. Should actually be in 16_entity
boolean ZC_walk2(entity_t *ent, map_view_t *map_v);

// Move gvar.player around and call map scrolling if required/possible
void walk_player(player_t *player, map_view_t *map_v);

// Scroll map in one direction (assumed from gvar.player's movement)
void mapScroll(map_view_t *mv, player_t *player);

void ZC_GirdChk (global_game_variables_t *gvar, player_t *player);

#endif /*__SCROLL16_H_*/
