/* Project 16 Source Code~
 * Copyright (C) 2012-2023 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

#include "src/lib/16_enti.h"

void EN_initentity(entity_t *enti, video_t *video)
{
	unsigned int i,o,o2,w,h;
	//tx and ty inited by map
	enti->x = enti->tx*TILEWH;
	enti->y = enti->ty*TILEWH;
	enti->triggerx = enti->tx;
	enti->triggery = enti->ty+1;	//south

	enti->q =		1;//loop var
	enti->d =		2;//dir var
	enti->pred =	4;//previous dir
	enti->hp =		4;//hit points
	enti->spt =	4;//walkspeed
	enti->persist_aniframe = enti->q;
	enti->spt=(TILEWH/(enti->spt));	//speed per tile wwww
	enti->overdraww =	0;
	enti->overdrawh =	4;

	VL_Initofs(video);

// 	modexClearRegion(&video->page[0], enti->x, enti->y, 16, 16, 2);
// 	modexClearRegion(&video->page[2], 0, 0, video->page[2].sw, video->page[2].sh, 45);
// 	modexClearRegion(&video->page[3], 0, 0, video->page[3].sw, video->page[3].sh, 47);
	if(!video->vga_state.bgps){
		//video->ofs.pattern_ofs=(uint16_t)video->page[0].data;
		//video->ofs.offscreen_ofs=(uint16_t)video->page[3].data;
		w = (enti->x + 24) & (~3);
		h = enti->y + 32;

		// block copy pattern to where we will draw the sprite
		vga_setup_wm1_block_copy();
		o2 = video->ofs.offscreen_ofs;										//dest
		o = video->ofs.pattern_ofs + (enti->y * video->page[0].stridew) + (enti->x >> 2);	// source
		for (i=0;i < h;i++,o += video->page[0].stridew,o2 += (w >> 2)) vga_wm1_mem_block_copy(o2,o,w >> 2);
		// must restore Write Mode 0/Read Mode 0 for this code to continue drawing normally
		vga_restore_rm0wm0();
	}//else{
		//w=h=o=i=o2=0;
		//modexCopyPageRegion(&video->page[2], &video->page[0], enti->x, enti->y, 0, 0, 24, 32);
	//}
}

//init player!
void EN_initPlayer(player_t *player, video_t *video)
{
	EN_initentity(&player->enti, video);
	player->enti.spri.x = player[0].enti.x-4;
	player->enti.spri.y = player[0].enti.y-16;
	player->walktype=0;
	player->enti.dire=30;
	vga_state.vga_draw_stride_limit = vga_state.vga_draw_stride = video->page[0].stridew;//skewing bug

	video->sprifilei =	set_anim_by_id(&player->enti.spri, player->enti.dire);
	animate_spri(&player->enti, video);

	//print_anim_ids(player[0].enti.spri);
/*	player[0].info.x = player[0].tx;
	player[0].info.xaxis = player[0].tx*TILEWH;
	player[0].info.y = player[0].ty;
	player[0].info.yaxis = player[0].ty*TILEWH;*/
}
