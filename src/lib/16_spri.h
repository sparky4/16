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
#ifndef __16_SPRI__
#define __16_SPRI__

#include "src/lib/16_vrs.h"
#include "src/lib/scroll16.h"

/* Retrive current animation name of sprite
* In:
* + struct sprite *spri - sprite to retrive current animation sequence name from
* Out:
* + char* - animation sequence name
*/
char* get_curr_anim_name(struct sprite *spri);

/* Change sprite's current animation to the one given by id
 * In:
 * struct sprite *spri - sprite to manipulate on
 * int id - id of a new animation sequence of th sprite
 * Out:
 * int - 0 on success, -1 on error
 */
int set_anim_by_id(struct sprite *spri, int anim_id);

/* Animate sprite, triggering any events and changing indices if necessary
 * NB: if you want to change animation sequence after a specific sprite is shown, you should call animate_spri first
 * In:
 * + struct sprite *spri - sprite to animate
 */
void animate_spri(entity_t *enti, video_t *video);

void print_anim_ids(struct sprite *spri);

#endif
