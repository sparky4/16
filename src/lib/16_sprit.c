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

#include "src/lib/16_sprit.h"

char* get_curr_anim_name(struct sprite *spri)
{
	// Retrive animation name list
	struct vrs_header far *vrs = spri->spritesheet->vrs_hdr;
	uint32_t far *anim_names_offsets = 	(uint32_t far *)
						((byte far *)vrs +
						 vrs->offset_table[VRS_HEADER_OFFSET_ANIMATION_NAME_LIST]);

	return (char *)(vrs + anim_names_offsets[spri->curr_anim]);
}

void init_anim(struct sprite *spri, int anim_index)
{
	struct vrs_header far *vrs = spri->spritesheet->vrs_hdr;
	uint32_t far *anim_lists_offsets = 	(uint32_t far *)
						((byte far *)vrs +
						 vrs->offset_table[VRS_HEADER_OFFSET_ANIMATION_LIST]);
	struct vrs_animation_list_entry_t far *anim_list =	(struct vrs_animation_list_entry_t far *)
								((byte far *)vrs +
								 anim_lists_offsets[anim_index]);

	// Upon new animation, start from the first sprite in it
	spri->curr_anim = anim_index;
	spri->curr_anim_spri = 0;
	spri->curr_spri_id = anim_list[0].sprite_id;
	spri->delay = anim_list[0].delay;

	spri->curr_anim_list = anim_list;
}


int set_anim_by_id(struct sprite *spri, int anim_id)
{
	int new_anim_index = 0;
	int iter_id;
	struct vrs_header far *vrs = spri->spritesheet->vrs_hdr;
	// Retruve animation ids list
        uint16_t far *anim_ids =	(uint16_t far *)
					((byte far *)vrs +
					 vrs->offset_table[VRS_HEADER_OFFSET_ANIMATION_ID_LIST]);

	// Loop through animation id untill match or end of list
	while(iter_id = anim_ids[new_anim_index])
	{
		// Return on successful match
		if (iter_id == anim_id)
		{
			init_anim(spri, new_anim_index);
			return 0;
		}
		new_anim_index++;
	}
	return -1;
}

void print_anim_ids(struct sprite *spri)
{
	int new_anim_index = 0;
	int iter_id;
	struct vrs_header far *vrs = spri->spritesheet->vrs_hdr;
	// Retruve animation ids list
        uint16_t far *anim_ids =	(uint16_t far *)
					((byte far *)vrs +
					 vrs->offset_table[VRS_HEADER_OFFSET_ANIMATION_ID_LIST]);

	if(!anim_ids[new_anim_index])
		return;
	// Loop through animation id untill match or end of list
	while(iter_id = anim_ids[new_anim_index])
	{
		printf("s%d ", iter_id);
		new_anim_index++;
	}
}

/*
void animate_spri(struct sprite *spri, global_game_variables_t *gv)
{
	int i;
	// Events go here


	// Draw sprite
	i = get_vrl_by_id(spri->spritesheet, spri->curr_spri_id, spri->sprite_vrl_cont);
	if(i < 0)
	{
		Quit (gv, "Error retriving required sprite");
	}

	// replace VGA stride with our own and mem ptr. then sprite rendering at this stage is just (0,0)
	vga_state.vga_draw_stride_limit = (gv->video.page[0].width + 3 - spri->x) >> 2;

	draw_vrl1_vgax_modex(
		spri->x-gv->video.page[0].dx,
		spri->y-gv->video.page[0].dy,
		spri->sprite_vrl_cont->vrl_header,
		spri->sprite_vrl_cont->line_offsets,
		spri->sprite_vrl_cont->buffer + sizeof(struct vrl1_vgax_header),
		spri->sprite_vrl_cont->data_size
	);

	// restore stride
	vga_state.vga_draw_stride_limit = vga_state.vga_draw_stride = gv->video.page[0].stridew;

	// Depending on delay, update indices
	switch(spri->delay){
		// Delay = 0 means that sprite should loop. Nothing to change here
		case 0:
			break;

		// Delay = 1 means that on next time unit sprite should be changed
		case 1:
			spri->curr_anim_spri++;

			// If we hit the end of an animation sequence, restart it
			if(!(spri->curr_spri_id = spri->curr_anim_list[spri->curr_anim_spri].sprite_id)){
				spri->curr_anim_spri = 0;
				spri->curr_spri_id = spri->curr_anim_list[spri->curr_anim_spri].sprite_id;
			}
			spri->delay = spri->curr_anim_list[spri->curr_anim_spri].delay;

		// Delay > 1 means that we should not change sprite yet. Decrease delay
		default:
			spri->delay--;
			break;
	}
}
*/
void animate_spri(struct sprite *spri, global_game_variables_t *gv)
{
#define VMEMPAGESIZE2	gv->video.page[0].pagesize+gv->video.page[1].pagesize
#define VMEMPAGEDATA2	gv->video.page[2].data
	unsigned int i,o,o2; int j;
	int x,y,rx,ry,w,h;
	int overdraw = 1;	// how many pixels to "overdraw" so that moving sprites with edge pixels don't leave streaks.
						// if the sprite's edge pixels are clear anyway, you can set this to 0.
	VGA_RAM_PTR omemptr;

	// Events go here


	omemptr = vga_state.vga_graphics_ram; // save original mem ptr
	x=spri->x-4;
	y=spri->y;

	// Draw sprite
	j = get_vrl_by_id(spri->spritesheet, spri->curr_spri_id, spri->sprite_vrl_cont);
	if(j < 0)
	{
		Quit (gv, "Error retriving required sprite");
	}

	// render box bounds. y does not need modification, but x and width must be multiple of 4
	if (x >= overdraw) rx = (x - overdraw) & (~3);
		else rx = -(gv->video.page[0].dx);
	if (y >= overdraw) ry = (y - overdraw);
		else ry = -(gv->video.page[0].dy);
	h = spri->sprite_vrl_cont->vrl_header->height + overdraw + y - ry;
	w = (x + spri->sprite_vrl_cont->vrl_header->width + (overdraw*2) + 3 - rx) & (~3);//round up
	if ((rx+w) > gv->video.page[0].width) w = gv->video.page[0].width-rx;
	if ((ry+h) > gv->video.page[0].height) h = gv->video.page[0].height-ry;

	// block copy pattern to where we will draw the sprite
	vga_setup_wm1_block_copy();
	o2 = VMEMPAGESIZE2;
	o = (0x10000UL - (uint16_t)VMEMPAGEDATA2) + (ry * gv->video.page[0].stridew) + (rx >> 2); // source offscreen
	for (i=0;i < h;i++,o += gv->video.page[0].stridew,o2 += (w >> 2)) vga_wm1_mem_block_copy(o2,o,w >> 2);

	// must restore Write Mode 0/Read Mode 0 for this code to continue drawing normally
	vga_restore_rm0wm0();

	// replace VGA stride with our own and mem ptr. then sprite rendering at this stage is just (0,0)
	vga_state.vga_draw_stride_limit = (gv->video.page[0].width + 3 - x) >> 2;//round up
	vga_state.vga_draw_stride = w >> 2;
	vga_state.vga_graphics_ram = omemptr + VMEMPAGESIZE2;

	// then the sprite. note modding ram ptr means we just draw to (x&3,0)
	draw_vrl1_vgax_modex(
		x-rx,
		y-ry,
		spri->sprite_vrl_cont->vrl_header,
		spri->sprite_vrl_cont->line_offsets,
		spri->sprite_vrl_cont->buffer + sizeof(struct vrl1_vgax_header),
		spri->sprite_vrl_cont->data_size
	);

	// restore ptr
	vga_state.vga_graphics_ram = omemptr;

	// block copy to visible RAM from offscreen
	vga_setup_wm1_block_copy();
	o = VMEMPAGESIZE2; // source offscreen
	o2 = (ry * gv->video.page[0].stridew) + (rx >> 2); // dest visible (original stride)
	for (i=0;i < h;i++,o += vga_state.vga_draw_stride,o2 += gv->video.page[0].stridew) vga_wm1_mem_block_copy(o2,o,w >> 2);
	// must restore Write Mode 0/Read Mode 0 for this code to continue drawing normally
	vga_restore_rm0wm0();

	// restore stride
	vga_state.vga_draw_stride_limit = vga_state.vga_draw_stride = gv->video.page[0].stridew;

	// Depending on delay, update indices
	switch(spri->delay){
		// Delay = 0 means that sprite should loop. Nothing to change here
		case 0:
			break;

		// Delay = 1 means that on next time unit sprite should be changed
		case 1:
			spri->curr_anim_spri++;

			// If we hit the end of an animation sequence, restart it
			if(!(spri->curr_spri_id = spri->curr_anim_list[spri->curr_anim_spri].sprite_id)){
				spri->curr_anim_spri = 0;
				spri->curr_spri_id = spri->curr_anim_list[spri->curr_anim_spri].sprite_id;
			}
			spri->delay = spri->curr_anim_list[spri->curr_anim_spri].delay;

		// Delay > 1 means that we should not change sprite yet. Decrease delay
		default:
			spri->delay--;
			break;
	}
}
