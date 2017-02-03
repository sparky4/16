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
#include "src/lib/bitmap.h"
#include "src/lib/16_map.h" //map is loaded here www
#include "src/lib/16render.h"
//#include "src/lib/16_map.h"	//new map stuff
#include "src/lib/16_timer.h"
#include "src/lib/wcpu/16_wcpu.h"
#include "src/lib/16_spri.h"

#include <hw/cpu/cpu.h>
#include <hw/dos/dos.h>
#include <hw/vga/vga.h>
#include <hw/vga/vrl.h>

#define SPRITE
//#define TILERENDER

#define PLAYERBMPDATAVAR	player[pn].data
#define PLAYERBMPDATA		*PLAYERBMPDATAVAR
#define PLAYERBMPDATAPTR	PLAYERBMPDATAVAR

#define PCXBMPVAR	player[0].data
#define PCXBMP		*PCXBMPVAR
#define PCXBMPPTR	PCXBMPVAR

//row and colum drawing routines
#define FULLRCREND
#define DRAWCOLNUM mv->page[id].dx
#define DRAWROWNUM mv->page[id].dy
//#define DRAWCOLNUM player[plid].enti.q
//#define DRAWROLNUM player[plid].enti.q

//for null map!
#define MAPW	40
#define MAPH	30

//===========================================================================//

	//if(player[0].enti.hp==0) printf("%d wwww\n", player[0].enti.y+8);
	//else printf("\nplayer[0].enti.y: %d\n", player[0].enti.y);
//printf("gvar.video.p=%u ", gvar.video.p);
//not used now	printf("temporary player sprite 0: http://www.pixiv.net/member_illust.php?mode=medium&illust_id=45556867\n");
//not used now	printf("temporary player sprite 1: http://www.pixiv.net/member_illust.php?mode=medium&illust_id=44606385\n");
//printf("mv[%u].tx: %d", gvar.video.panp, mv[gvar.video.panp].tx); printf("	mv[%u].ty: %d	", gvar.video.panp, mv[gvar.video.panp].ty);
//printf("gvar.kurokku:	"); printf("%.0f ", clock());	printf("tiku=%lu ", gvar.kurokku.tiku);	printf("t=%.0f ", gvar.kurokku.t);	printf("ticktock()=%f ", ticktock(&gvar));	printf("%.0f fps", (double)gvar.kurokku.tiku/ticktock(&gvar));
//printf("map.width=%d	map.height=%d	map.data[0]=%d\n", mv[1].map->width, mv[1].map->height, mv[1].map->data[0]);
//printf("pageflipflop=%u", pageflipflop);
//printf("&global_temp_status_text = %Fp\n", &global_temp_status_text);
//printf("middle tile position: %dx", gvar.video.page[0].tilemidposscreenx);	printf("%d\n", gvar.video.page[0].tilemidposscreeny);
//printf("	aniframe=%u", player[0].enti.persist_aniframe);
#define SCROLLEXITMESG 	printf("[0]txy: %dx%d	", mv[0].tx, mv[0].ty); printf("[1]txy: %dx%d", mv[1].tx, mv[1].ty);\
	printf("\n\n");\
	printf("player vars:\n");\
	printf("	xy: %dx%d", player[0].enti.x, player[0].enti.y); printf("	txy: %dx%d", player[0].enti.tx, player[0].enti.ty); printf("	triggxy: %dx%d", player[0].enti.triggerx, player[0].enti.triggery); printf("	value: %d\n", mv[1].map->data[(player[0].enti.triggerx-1)+(map.width*(player[0].enti.triggery-1))]);\
	printf("	hp: %d", (player[0].enti.hp));	printf("	q: %u", player[0].enti.q);	printf("	info.dir: %u", player[0].info.dir);	printf("	d: %u", player[0].enti.d);	printf("	dire: %u", player[0].enti.dire);\
		printf("	pdir: %u\n", player[0].pdir); printf("	delay=%u", player[0].enti.spri->delay);\
printf("\n\n");\
	VL_PrintmodexmemInfo(&gvar.video);\
\
	printf("gvar.video:\n");\
	printf(" r=%u ", gvar.video.dorender);\
	printf("bgps=%u ", gvar.video.bgps);\
printf("\n\n");

extern boolean pagenorendermap, pagedelayrendermap;

extern char global_temp_status_text[512];

//map_t allocMap(int w, int h);
void initMap(map_t *map);
void ZC_walk(map_view_t *pip, player_t *player, word pn);
void ZC_panPageManual(map_view_t *pip, player_t *player, word pn);
void ZC_MVSetup(map_view_t *pip, map_t *map, global_game_variables_t *gv);
void ZC_MVInit(map_view_t *pip, int tx, int ty);
void ZC_ShowMV(map_view_t *moo, boolean vsync, boolean sr);
void near mapScrollRight(map_view_t *mv, player_t *player, word id, word plid);
void near mapScrollLeft(map_view_t *mv, player_t *player, word id, word plid);
void near mapScrollUp(map_view_t *mv, player_t *player, word id, word plid);
void near mapScrollDown(map_view_t *mv, player_t *player, word id, word plid);
void ZC_mapScroll(map_view_t *mv, player_t *player, word pn);
void ZC_playerXYpos(int x, int y, player_t *player, map_view_t *pip, nibble pn, boolean defaultsw);
sword chkmap(map_t *map, word q);
void mapGoTo(map_view_t *mv, int tx, int ty);
void ZC_mapinitMV(map_view_t *mv, int tx, int ty);
void ZC_mapredraw(map_view_t *mv, int tx, int ty);
void near mapDrawTile(tiles_t *t, word i, page_t *page, word x, word y);
void near mapDrawRow(map_view_t *mv, int tx, int ty, word y, player_t *player, word poopoffset);
void near mapDrawCol(map_view_t *mv, int tx, int ty, word x, player_t *player, word poopoffset);
void mapDrawWRow(map_view_t *mv, int tx, int ty, word y);
void mapDrawWCol(map_view_t *mv, int tx, int ty, word x);
inline void near ScrollRight(map_view_t *mv, player_t *player, word id, word plid)
{
	/* increment the pixel position and update the page */
	mv[id].page->dx += player[plid].enti.speed;

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

inline void near ScrollLeft(map_view_t *mv, player_t *player, word id, word plid)
{
	/* decrement the pixel position and update the page */
	mv[id].page->dx -= player[plid].enti.speed;

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

inline void near ScrollUp(map_view_t *mv, player_t *player, word id, word plid)
{
	/* decrement the pixel position and update the page */
	mv[id].page->dy -= player[plid].enti.speed;

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

inline void near ScrollDown(map_view_t *mv, player_t *player, word id, word plid)
{
	/* increment the pixel position and update the page */
	mv[id].page->dy += player[plid].enti.speed;

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

// Move player around and call map scrolling if required/possible
void walk_player(player_t *player, map_view_t *map_v);

// Scroll map in one direction (assumed from player's movement)
void mapScroll(map_view_t *mv, player_t *player);

#endif /*__SCROLL16_H_*/
