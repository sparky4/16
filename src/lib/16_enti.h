/* Project 16 Source Code~
 * Copyright (C) 2012-2022 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

#ifndef __16_NPC__
#define __16_NPC__

#include "src/lib/16_head.h"
#include "src/lib/16_spri.h"
#include "src/lib/16_in.h"

//moved to 16_tdef.h
// typedef	struct
// {
// 	int x; // exact position on the viewable map
// 	int y; // exact position on the viewable map
// 	int tx; // tile position on the viewable map
// 	int ty; // tile position on the viewable map
// 	int triggerx; // trigger box tile position on the viewable map
// 	int triggery; // trigger box tile position on the viewable map
// 	byte d;		// direction the NPC faces
// 	struct sprite *spri; // sprite used by NPC
// 	sword hp; // hitpoints of the NPC
// 	byte near pdir;	// previous direction~
// 	word speed;		// NPC's speed
// 	word spt;		// speed per tile
// } entity_t;

void EN_initentity(entity_t *enti, video_t *video);
void EN_initPlayer(player_t *player, video_t *video);

#endif
