/* Project 16 Source Code~
 * Copyright (C) 2012-2016 sparky4 & pngwen & andrius4669
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
#include "src/lib/bakapee.h"
#include "src/lib/modex16.h"
#include "src/lib/16_in.h"
#include "src/lib/bitmap.h"
#include "src/lib/planar.h"
#include "src/lib/mapread.h"
#include "src/lib/16_timer.h"
#include "src/lib/wcpu/wcpu.h"

//#define SPRITE
#define TILERENDER

#define PBUFSFUN		modexDrawSpritePBufRegion
#define PBUFBFUN		modexDrawBmpPBufRegion
#define PLAYERBMPDATA	&player[pn].data

typedef struct {
	map_t *map;
	page_t *page;
	int tx; //appears to be the top left tile position on the viewable screen map
	int ty; //appears to be the top left tile position on the viewable screen map
	word dxThresh; //????
	word dyThresh; //????
} map_view_t;

typedef struct
{
	map_view_t *mv;
} map_view_db_t;

//for null map!
#define MAPW	40
#define MAPH	30

//map_t allocMap(int w, int h);
//void initMap(map_t *map);
void walk(map_view_t *pip, player_t *player, word pn);
void near mapScrollRight(map_view_t *mv, player_t *player, word id, word plid);
void near mapScrollLeft(map_view_t *mv, player_t *player, word id, word plid);
void near mapScrollUp(map_view_t *mv, player_t *player, word id, word plid);
void near mapScrollDown(map_view_t *mv, player_t *player, word id, word plid);
sword chkmap(map_t *map, word q);
void mapGoTo(map_view_t *mv, int tx, int ty);
void near mapDrawTile(tiles_t *t, word i, page_t *page, word x, word y);
void near mapDrawRow(map_view_t *mv, int tx, int ty, word y, player_t *p, word poopoffset);
void near mapDrawCol(map_view_t *mv, int tx, int ty, word x, player_t *p, word poopoffset);
void mapDrawWRow(map_view_t *mv, int tx, int ty, word y);
void mapDrawWCol(map_view_t *mv, int tx, int ty, word x);
//void qclean();
void shinku(map_view_t *pip, global_game_variables_t *gv);
void near animatePlayer(map_view_t *pip, player_t *player, word playnum, sword scrollswitch);

#endif /*__SCROLL16_H_*/
