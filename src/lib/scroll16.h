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
#include "src/lib/16_sprit.h"

#include <hw/cpu/cpu.h>
#include <hw/dos/dos.h>
#include <hw/vga/vga.h>
#include <hw/vga/vrl.h>

//#define SPRITE
#define SPRITO
//#define TILERENDER

//extern void modexDrawSpriteRegion(page_t *page, int x, int y, int rx, int ry, int rw, int rh, bitmap_t *bmp);

//modexDrawSpritePBufRegion
//modexDrawBmpPBufRegion
//#define PBUFSFUN		modexDrawSpriteRegion
//#define PBUFBFUN		modexDrawBmpRegion

#define PLAYERBMPDATAVAR	player[pn].data
#define PLAYERBMPDATA		*PLAYERBMPDATAVAR
#define PLAYERBMPDATAPTR	PLAYERBMPDATAVAR

#define PCXBMPVAR	player[0].data
#define PCXBMP		*PCXBMPVAR
#define PCXBMPPTR	PCXBMPVAR

typedef struct {
	map_t *map;
	page_t *page;
	int tx; //appears to be the top left tile position on the viewable screen map
	int ty; //appears to be the top left tile position on the viewable screen map
	word dxThresh; //Threshold for physical tile switch
	word dyThresh; //Threshold for physical tile switch
	video_t *video;	//pointer to game variables of the video
	nibble *panp;	// pointer to video's pan page num
//newer vars!
	int dx, dy, delta, d;
} map_view_t;
/* Map is presumed to:
 * 1. Have all the required layers and tilesets within itself
 * 2. Have a 'fence' around accessible blocks to simplify boundary logic
 * 3. Have a persistent map and tile size among the layers
 * Map view is presumed to:
 * 1. Calculate, store and update a panning info, which includes, but not limited to:
 * 	combined layer information, actual map representation (reflecting real state of the game),
 * 	pixel shift for smooth tile scrolling.
 * 2. Provide ways to draw a visible part of map. For simplicity with smooth scrolling,
 * 	additional row/column is always drawn at the each side of the map. This implies that 'fence'
 * 	should have a sprite too. Map is drawn left-to-right, top-to-bottom.
 */

//for null map!
#define MAPW	40
#define MAPH	30

	//if(player[0].enti.hp==0) printf("%d wwww\n", player[0].enti.y+8);
	//else printf("\nplayer[0].enti.y: %d\n", player[0].enti.y);
//	printf("palette offset:	%d\n", paloffset/3);
//++++	printf("Total used: %zu\n", oldfreemem-GetFreeSize());
//++++	printf("Total free: %zu\n", GetFreeSize());
//not used now	printf("temporary player sprite 0: http://www.pixiv.net/member_illust.php?mode=medium&illust_id=45556867\n");
//not used now	printf("temporary player sprite 1: http://www.pixiv.net/member_illust.php?mode=medium&illust_id=44606385\n");
//printf("mv[%u].tx: %d", gvar.video.panp, mv[gvar.video.panp].tx); printf("	mv[%u].ty: %d	", gvar.video.panp, mv[gvar.video.panp].ty);
//printf("gvar.kurokku:	"); printf("%.0f ", clock());	printf("tiku=%lu ", gvar.kurokku.tiku);	printf("t=%.0f ", gvar.kurokku.t);	printf("ticktock()=%f ", ticktock(&gvar));	printf("%.0f fps", (double)gvar.kurokku.tiku/ticktock(&gvar));
//printf("map.width=%d	map.height=%d	map.data[0]=%d\n", mv[0].map->width, mv[0].map->height, mv[0].map->data[0]);
//printf("&global_temp_status_text = %Fp\n", &global_temp_status_text);
#define SCROLLEXITMESG 	printf("tx: %d	", mv[0].tx);\
	printf("ty: %d\n", mv[0].ty);\
	printf("\n");\
	printf("player vars:\n");\
	printf("	x: %d", player[0].enti.x); printf("	y: %d\n", player[0].enti.y);\
	\
	printf("	tx: %d", player[0].enti.tx); printf("	ty: %d\n", player[0].enti.ty);\
	printf("	triggx: %d", player[0].enti.triggerx); printf("	triggy: %d\n", player[0].enti.triggery);\
	printf("	hp: %d", (player[0].enti.hp));	printf("	q: %d", player[0].enti.q);	printf("	player.info.dir: %d", player[0].info.dir);	printf("	player.d: %d ", player[0].enti.d);\
		printf("	pdir=%d\n", player[0].pdir); printf("	aniframe=%u", player[0].enti.persist_aniframe);\
	printf("	tile data value at player trigger position: %d\n\n", mv[0].map->data[(player[0].enti.triggerx-1)+(map.width*(player[0].enti.triggery-1))]);\
\
	printf("Virtual Screen: %dx", gvar.video.page[0].width);	printf("%d	", gvar.video.page[0].height);\
	printf("Screen: %dx", gvar.video.page[0].sw);	printf("%d\n", gvar.video.page[0].sh);\
	printf("virtual tile resolution: %dx", gvar.video.page[0].tilesw);	printf("%d	", gvar.video.page[0].tilesh);\
	printf("tile resolution: %dx", gvar.video.page[0].tw);	printf("%d\n", gvar.video.page[0].th);\
	printf("middle tile position: %dx", gvar.video.page[0].tilemidposscreenx);	printf("%d\n", gvar.video.page[0].tilemidposscreeny);\
	modexprintmeminfo(&gvar.video);\
\
	printf("gvar.video.p=%u ", gvar.video.p); printf("gvar.video.r=%u ", gvar.video.r);\
	printf("pageflipflop=%u\n", pageflipflop);\
	printf("\n");

#define SHOWMVFUN ZC_ShowMV(&mv, 0, 0);
#define PANKEYFUN \
			ZC_panPageManual(&mv, &player, 0); \
			if(IN_KeyDown(1+1) || IN_KeyDown(sc_Z)){ gvar.video.panp=0; SHOWMVFUN; } \
			if(IN_KeyDown(2+1) || IN_KeyDown(sc_X)){ gvar.video.panp=1; SHOWMVFUN; } \
			if(IN_KeyDown(3+1) || IN_KeyDown(sc_C)){ gvar.video.panp=2; SHOWMVFUN; if(IN_KeyDown(sc_C)) modexClearRegion(&gvar.video.page[2], 0, 0, gvar.video.page[2].sw, gvar.video.page[2].sh, 47); } \
			if(IN_KeyDown(4+1) || IN_KeyDown(sc_V)){ gvar.video.panp=3; SHOWMVFUN; if(IN_KeyDown(sc_V)) modexClearRegion(&gvar.video.page[3], 0, 0, gvar.video.page[3].sw, gvar.video.page[3].sh, 45); } \
			if(IN_KeyDown(25)){ modexpdump(mv[1].page); modexShowPage(&(gvar.video.page[1])); IN_UserInput(1,1); }

extern boolean pageflipflop, pageploop;

extern char global_temp_status_text[512];

//map_t allocMap(int w, int h);
//void initMap(map_t *map);
void ZC_walk(map_view_t *pip, player_t *player, word pn);
void oldwalk(map_view_t *pip, player_t *player, word pn);
void ZC_panPageManual(map_view_t *pip, player_t *player, word pn);
void ZC_MVSetup(map_view_t *pip, map_t *map, global_game_variables_t *gv);
void ZC_MVInit(map_view_t *pip, int tx, int ty);
void ZC_ShowMV(map_view_t *moo, boolean vsync, boolean sr);
void near mapScrollRight(map_view_t *mv, player_t *player, word id, word plid);
void near mapScrollLeft(map_view_t *mv, player_t *player, word id, word plid);
void near mapScrollUp(map_view_t *mv, player_t *player, word id, word plid);
void near mapScrollDown(map_view_t *mv, player_t *player, word id, word plid);
void near ScrollRight(map_view_t *mv, player_t *player, word id, word plid);
void near ScrollLeft(map_view_t *mv, player_t *player, word id, word plid);
void near ScrollUp(map_view_t *mv, player_t *player, word id, word plid);
void near ScrollDown(map_view_t *mv, player_t *player, word id, word plid);
void playerXYpos(int x, int y, player_t *player, map_view_t *pip, nibble pn);
sword chkmap(map_t *map, word q);
void mapGoTo(map_view_t *mv, int tx, int ty);
void mapinitmapview(map_view_t *mv, int tx, int ty);
void near mapDrawTile(tiles_t *t, word i, page_t *page, word x, word y);
void near mapDrawRow(map_view_t *mv, int tx, int ty, word y, player_t *player, word poopoffset);
void near mapDrawCol(map_view_t *mv, int tx, int ty, word x, player_t *player, word poopoffset);
void mapDrawWRow(map_view_t *mv, int tx, int ty, word y);
void mapDrawWCol(map_view_t *mv, int tx, int ty, word x);
//void qclean();
void shinku(global_game_variables_t *gv);
void near animatePlayer(map_view_t *pip, player_t *player, word pn, sword scrollswitch);
void near ZC_animatePlayer(map_view_t *pip, player_t *player, word pn, sword scrollswitch);

// Move an entity around. Should actually be in 16_entity
boolean ZC_walk2(entity_t *ent, map_view_t *map_v);

// Move player around and call map scrolling if required/possible
void walk_player(player_t *player, map_view_t *map_v);

// Scroll map in one direction (assumed from player's movement)
void near mapScroll(map_view_t *mv, player_t *player);

#endif /*__SCROLL16_H_*/
