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

#include "src/lib/16_spri.h"
#include <hw/vga/vrl1xdrc.h>

#define VRS_USECAMMPM

void VRS_ReadVRS(char *filename, entity_t *enti, global_game_variables_t *gvar){	VRS_OpenVRS(filename, enti, 1, gvar);	}
void VRS_LoadVRS(char *filename, entity_t *enti, global_game_variables_t *gvar){	VRS_OpenVRS(filename, enti, 0, gvar);	}
void VRS_OpenVRS(char *filename, entity_t *enti, boolean rlsw, global_game_variables_t *gvar)
{
#ifndef VRS_USECAMMPM
	vrl1_vgax_offset_t **vrl_line_offsets;
#endif
	uint16_t far *vrl_id_iter;
	uint32_t far *vrl_headers_offsets;
	struct vrl1_vgax_header far *curr_vrl;
	uint16_t vrl_size;
	int num_of_vrl,i;

	switch(rlsw)
	{
		case 1:
//TODO ++++			CA_ReadFile(filename, &gvar->ca.spribuff, gvar);
			CA_ReadFile(filename, MEMPTRCONV enti->spri.spritesheet.spribuff, gvar);
		break;
		case 0:
//TODO ++++			CA_LoadFile(filename, &gvar->ca.spribuff, gvar);
			CA_LoadFile(filename, MEMPTRCONV enti->spri.spritesheet.spribuff, gvar);
		break;
	}

	// Insert sanity cheks later
//TODO ++++	enti->spri.spritesheet.buffer = gvar->ca.spribuff;
	enti->spri.spritesheet.buffer = enti->spri.spritesheet.spribuff;	//TODO: merge these 2 vars into 1
	enti->spri.spritesheet.data_size = sizeof(enti->spri.spritesheet.buffer) - sizeof(struct vrl1_vgax_header);
	num_of_vrl = 0;
	vrl_id_iter = (uint16_t far *)(enti->spri.spritesheet.buffer + enti->spri.spritesheet.vrs_hdr->offset_table[VRS_HEADER_OFFSET_SPRITE_ID_LIST]);
	while(vrl_id_iter[num_of_vrl]){
		num_of_vrl++;
	}

	// Allocate memory for vrl line offsets table
#ifndef VRS_USECAMMPM
	vrl_line_offsets = malloc(sizeof(vrl1_vgax_offset_t *)*num_of_vrl);//TODO: USE MM_ CA_ AND PM_
#else
	switch(rlsw)
	{
		case 0:
#ifdef __DEBUG_MM__
			dbg_debugmm=0;
#endif
//			MM_GetPtr(MEMPTRCONV gvar->ca.grsegs, sizeof(vrl1_vgax_offset_t *)*num_of_vrl, gvar);
//			enti->spri.spritesheet.vrl_line_offsets = (vrl1_vgax_offset_t **)gvar->ca.grsegs;
			MM_GetPtr(gvar->ca.spribuff, sizeof(vrl1_vgax_offset_t *)*num_of_vrl, gvar);
			enti->spri.spritesheet.vrl_line_offsets = (vrl1_vgax_offset_t **)(gvar->ca.spribuff);
//			MM_GetPtr(spribuff, sizeof(vrl1_vgax_offset_t *)*num_of_vrl, gvar);
//			enti->spri.spritesheet.vrl_line_offsets = (vrl1_vgax_offset_t **)spribuff;
		break;
	}
#endif

	vrl_headers_offsets = (uint32_t far *)(enti->spri.spritesheet.buffer + enti->spri.spritesheet.vrs_hdr->offset_table[VRS_HEADER_OFFSET_VRS_LIST]);
	// Calculate line offsets for each vrl
	for(i = 0; i < num_of_vrl; i++){
		curr_vrl = (struct vrl1_vgax_header far *)(enti->spri.spritesheet.buffer + vrl_headers_offsets[i]);

		// Calc. vrl size as (next_offset - curr_offset)
		if (i != num_of_vrl - 1){
			vrl_size = vrl_headers_offsets[i+1] - vrl_headers_offsets[i] - sizeof(struct vrl1_vgax_header);
		}
		// If it's the last vrl, size is (next_vrs_struct_offset - curr_offset)
		else{
			vrl_size = enti->spri.spritesheet.vrs_hdr->offset_table[VRS_HEADER_OFFSET_SPRITE_ID_LIST] - vrl_headers_offsets[i] - sizeof(struct vrl1_vgax_header);
		}
#ifndef VRS_USECAMMPM
		vrl_line_offsets[i] = vrl1_vgax_genlineoffsets(curr_vrl, (byte *)curr_vrl + sizeof(struct vrl1_vgax_header), vrl_size);
#else
		enti->spri.spritesheet.vrl_line_offsets[i] = vrl1_vgax_genlineoffsets((struct vrl1_vgax_header *)curr_vrl, (byte *)curr_vrl + sizeof(struct vrl1_vgax_header), vrl_size);
#endif
	}
#ifndef VRS_USECAMMPM
	enti->spri.spritesheet.vrl_line_offsets = vrl_line_offsets;
#endif
//	printf("VRS_OpenVRS\n");
//	printf("	vrl_size=%lu\n\n",vrl_size);
}

#if 0
//
void VRS_ReadVRL(char *filename, entity_t *enti, global_game_variables_t *gvar){	VRS_OpenVRL(filename, enti, 1, gvar);	}
void VRS_LoadVRL(char *filename, entity_t *enti, global_game_variables_t *gvar){	VRS_OpenVRL(filename, enti, 0, gvar);	}
void VRS_OpenVRL(char *filename, entity_t *enti, boolean rlsw, global_game_variables_t *gvar)
{
#ifndef VRL_USECAMMPM
	vrl1_vgax_offset_t *line_offsets;
#endif
//	uint32_t far *vrl_headers_offsets;
	struct vrl1_vgax_header far *curr_vrl;

	switch(rlsw)
	{
		case 1:
			CA_ReadFile(filename, &enti->spri.spritesheet.spribuff, gvar);
		break;
		case 0:
			CA_LoadFile(filename, &enti->spri.spritesheet.spribuff, gvar);
		break;
	}

	// Insert sanity cheks later
	enti->spri.sprite_vrl_cont.buffer = enti->spri.spritesheet.spribuff;
	enti->spri.sprite_vrl_cont.data_size = sizeof(enti->spri.spritesheet.buffer) - sizeof(struct vrl1_vgax_header);

	// Allocate memory for vrl line offsets table
#ifndef VRL_USECAMMPM
	line_offsets = malloc(sizeof(vrl1_vgax_offset_t *));//TODO: USE MM_ CA_ AND PM_
#else
	MM_GetPtr(MEMPTRCONV gvar->ca.grsegs[0], sizeof(vrl1_vgax_offset_t *), gvar);
	enti->spri.sprite_vrl_cont.line_offsets = (vrl1_vgax_offset_t *)gvar->ca.grsegs[0];
#endif

	//vrl_headers_offsets = (uint32_t far *)(enti->spri.sprite_vrl_cont.buffer + enti->spri.sprite_vrl_cont.vrs_hdr->offset_table[VRL_HEADER_OFFSET_VRL_LIST]);
	// Calculate line offsets for each vrl
		curr_vrl = (struct vrl1_vgax_header far *)(enti->spri.sprite_vrl_cont.buffer);// + vrl_headers_offsets);

#ifndef VRL_USECAMMPM
		line_offsets = vrl1_vgax_genlineoffsets(curr_vrl, (byte *)curr_vrl + sizeof(struct vrl1_vgax_header), sizeof(enti->spri.spritesheet.buffer));
#else
		enti->spri.sprite_vrl_cont.line_offsets = vrl1_vgax_genlineoffsets(curr_vrl, (byte *)curr_vrl + sizeof(struct vrl1_vgax_header), sizeof(enti->spri.spritesheet.buffer));
#endif

#ifndef VRL_USECAMMPM
	enti->spri.sprite_vrl_cont.line_offsets = line_offsets;
#endif
	printf("VRS_OpenVRL\n");
	printf("	vrl_size=%lu\n\n",sizeof(enti->spri.spritesheet.buffer));
}
//
#endif

// Seek and return a specified .vrl blob from .vrs blob in far memory
int get_vrl_by_id(struct vrs_container far *vrs_cont, uint16_t id, struct vrl_container *vrl_cont){
	uint16_t far *ids;
	uint32_t far *vrl_offs_list;
	int counter = 0;

	// If id is invalid, return -1
	if(id == 0){
		// Probably add an error message?
		return -1;
	}

	// Get id list from .vrs blob (base + offset)
	ids = (uint16_t far*)(vrs_cont->buffer +
		vrs_cont->vrs_hdr->offset_table[VRS_HEADER_OFFSET_SPRITE_ID_LIST]);

	// Loop through the id list until we found the right one or hit the end of the list
	// Counter is keeping track of the offset(in ids/vrl blobs)
	while(ids[counter] != id && ids[counter]){
		counter++;
	}
	// Return -2 if we couldn't find the requested id
	if(!ids[counter]){
		// Error message?
		return -2;
	}

	// Get vrl offsets list from .vrs blob (base + offset)
	vrl_offs_list = (uint32_t far *)(vrs_cont->buffer +
					vrs_cont->vrs_hdr->offset_table[VRS_HEADER_OFFSET_VRS_LIST]);

	// Get vrl_header from .vrs (base + offset from vrl_list)
	// Counter is number of vrls to skip (ids and vrls are aligned according to the .vrs specification)
	vrl_cont->vrl_header = (struct vrl1_vgax_header far *)(vrs_cont->buffer + vrl_offs_list[counter]);

	// Get .vrl size by integer arithmetics (next vrl offset - current vrl offset)
	if(ids[counter+1]){
		vrl_cont->data_size = vrl_offs_list[counter+1] - vrl_offs_list[counter] - sizeof(struct vrl1_vgax_header);
	}
	// If we are retriving the last vrl, size is ids_list offset - current vrl offset, as next vrl offs is 0
	else{
		vrl_cont->data_size = vrs_cont->vrs_hdr->offset_table[VRS_HEADER_OFFSET_SPRITE_ID_LIST] - vrl_offs_list[counter] - sizeof(struct vrl1_vgax_header);
	}

	// Retrive line offsets form .vrs
	vrl_cont->line_offsets = vrs_cont->vrl_line_offsets[counter];

	return 0;
}

//void draw_vrl1_vgax_modex_strip(unsigned char far *draw,unsigned char far *s);

//===========================================================================

//#define NEWVRSDRAWFUN
void DrawVRL (unsigned int x,unsigned int y,struct vrl1_vgax_header *hdr,vrl1_vgax_offset_t *lineoffs/*array hdr->width long*/,unsigned char *data,unsigned int datasz) {
	unsigned char far *draw;
	unsigned int vram_offset = (y * vga_state.vga_draw_stride) + (x >> 2),sx;
	unsigned int vramlimit = vga_state.vga_draw_stride_limit;
	unsigned char vga_plane = (x & 3);
	unsigned char *s;

//	byte	outputvars[72][128];
	word a;//,by,bxmax,bymax;

	printf("DrawVRL:\n");

	/* draw one by one */
	for (sx=0;sx < hdr->width;sx++) {
		draw = vga_state.vga_graphics_ram + vram_offset;
		vga_write_sequencer(0x02/*map mask*/,1 << vga_plane);
		s = data + lineoffs[sx];
		draw_vrl1_vgax_modex_strip(draw,s);

		if(!sx)
		for(a=0;a<hdr->height;a++)//hdr->width*
		{
//			if((*(s+a)==0x0) && (*(s+(a+1))==0x20) && (*(s+(a+2))==0xff)) a+=2;
//			if(*(s+a)<=13)
//				if (!(a%4) && a ) printf("\n");
//				sprintf(outputvars[sx][by], "%02x", *(s+a));
				printf("[%02u]	%u\n", a, *(s+a));
//				by++;
		}

		/* end of a vertical strip. next line? */
		if ((++vga_plane) == 4) {
			if (--vramlimit == 0) break;
			vram_offset++;
			vga_plane = 0;
		}
	}
#if 0
	bxmax = sx; bymax = by;
	for(by=0;by<bymax;by++)
	{
		for(sx=0;sx<bxmax;sx++)
		{
//			if (!(sx%hdr->width) && sx ) printf("\n	");
			printf("%02x ", outputvars[sx][by]);
		}
		printf("\n	");
	}
#endif
}

//===========================================================================

char* get_curr_anim_name(struct sprite *spri)
{
	// Retrive animation name list
	struct vrs_header far *vrs = spri->spritesheet.vrs_hdr;
	uint32_t far *anim_names_offsets = 	(uint32_t far *)
						((byte far *)vrs +
						 vrs->offset_table[VRS_HEADER_OFFSET_ANIMATION_NAME_LIST]);

	return (char *)(vrs + anim_names_offsets[spri->curr_anim]);
}

void init_anim(struct sprite *spri, int anim_index)
{
	struct vrs_header far *vrs = spri->spritesheet.vrs_hdr;
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

	spri->curr_anim_list = (struct vrs_animation_list_entry_t *)anim_list;
}

int set_anim_by_id(struct sprite *spri, int anim_id)
{
	int new_anim_index = 0;
	int iter_id;
	struct vrs_header far *vrs = spri->spritesheet.vrs_hdr;
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
	struct vrs_header far *vrs = spri->spritesheet.vrs_hdr;
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

void animate_spri(entity_t *enti, video_t *video)
{
#define INC_PER_FRAME if(enti->q&1) enti->persist_aniframe++; if(enti->persist_aniframe>4) enti->persist_aniframe = 1;
	unsigned int i,o,o2; int j;
	int x,y,rx,ry,w,h;
	VGA_RAM_PTR omemptr = (VGA_RAM_PTR)video->page[0].data;// save original mem ptr

	x=enti->spri.x;
	y=enti->spri.y;
	VL_Initofs(video);

	// Depending on delay, update indices
//#define FRAME1 modexDrawSpriteRegion(pip[(pip->video->p)].page, x, y, 48, player[pn].enti.dire, 24, 32,	PLAYERBMPDATAPTR);
//#define FRAME2 modexDrawSpriteRegion(pip[(pip->video->p)].page, x, y, 24, player[pn].enti.dire, 24, 32,	PLAYERBMPDATAPTR); stand
//#define FRAME3 modexDrawSpriteRegion(pip[(pip->video->p)].page, x, y, 0, player[pn].enti.dire, 24, 32,	PLAYERBMPDATAPTR);
//#define FRAME4 modexDrawSpriteRegion(pip[(pip->video->p)].page, x, y, 24, player[pn].enti.dire, 24, 32,	PLAYERBMPDATAPTR); stand

	// copy active display (0) to offscreen buffer (0x4000)
	/*vga_state.vga_draw_stride_limit = vga_state.vga_draw_stride = vga_state.vga_stride;
	vga_setup_wm1_block_copy();
	vga_wm1_mem_block_copy(copy_ofs,	display_ofs,	vga_state.vga_stride * vga_state.vga_height);
	vga_restore_rm0wm0();*/

	switch(enti->spri.delay)
	{
		// Delay = 0 means that sprite should loop. Nothing to change here
		case 0:
		break;

		// Delay = 1 means that on next time unit sprite should be changed
		case 1:
			INC_PER_FRAME
			enti->spri.curr_anim_spri++;

			// If we hit the end of an animation sequence, restart it
			if(!(	enti->spri.curr_spri_id = enti->spri.curr_anim_list[enti->spri.curr_anim_spri].sprite_id)){
				enti->spri.curr_anim_spri = 0;
				enti->spri.curr_spri_id = enti->spri.curr_anim_list[enti->spri.curr_anim_spri].sprite_id;
			}
			enti->spri.delay = enti->spri.curr_anim_list[enti->spri.curr_anim_spri].delay;

		// Delay > 1 means that we should not change sprite yet. Decrease delay
		default:
			enti->spri.delay--;
		break;
	}

	// Events go here



	// Draw sprite
	j = get_vrl_by_id(&enti->spri.spritesheet, enti->spri.curr_spri_id, &enti->spri.sprite_vrl_cont);
	if(j < 0)
	{
		//Quit (gv, "Error retriving required sprite");
		return;
	}

	// render box bounds. y does not need modification, but x and width must be multiple of 4
	if(!video->vga_state.rss)
	{
		if (x >= enti->overdraww) rx = (x - enti->overdraww) & (~3);
			else rx = -(video->page[0].dx);
		if (y >= enti->overdrawh) ry = (y - enti->overdrawh);
			else ry = -(video->page[0].dy);
		h = enti->spri.sprite_vrl_cont.vrl_header->height + enti->overdrawh + y - ry;
		w = (x + enti->spri.sprite_vrl_cont.vrl_header->width + (enti->overdraww*2) + 3 - rx) & (~3) - enti->overdraww;//round up
		if ((rx+w) > video->page[0].width) w = video->page[0].width-rx;
		if ((ry+h) > video->page[0].height) h = video->page[0].height-ry;

		if(!video->vga_state.bgps){
			// block copy pattern to where we will draw the sprite
			vga_setup_wm1_block_copy();
			o2 = video->ofs.offscreen_ofs;
			o = video->ofs.pattern_ofs + (ry * video->page[0].stridew) + (rx >> 2); // source offscreen
			for (i=0;i < h;i++,o += video->page[0].stridew,o2 += (w >> 2)) vga_wm1_mem_block_copy(o2,o,w >> 2);

			// must restore Write Mode 0/Read Mode 0 for this code to continue drawing normally
			vga_restore_rm0wm0();
		}
		// replace VGA stride with our own and mem ptr. then sprite rendering at this stage is just (0,0)
		vga_state.vga_draw_stride = w >> 2;
		vga_state.vga_graphics_ram = omemptr + video->ofs.offscreen_ofs;
	}else{ rx=ry=w=h=0; vga_state.vga_graphics_ram = (VGA_RAM_PTR)video->page[0].data; }
	vga_state.vga_draw_stride_limit = (video->page[0].width + 3 - x) >> 2;//round up

	// then the sprite. note modding ram ptr means we just draw to (x&3,0)
#ifndef SPRITE
	modexClearRegion(&video->page[0], x, y, 16, 32, 1);
#else
#ifndef NEWVRSDRAWFUN
	draw_vrl1_vgax_modex
#else
	DrawVRL
#endif
	(
		x-rx,
		y-ry,
		enti->spri.sprite_vrl_cont.vrl_header,
		enti->spri.sprite_vrl_cont.line_offsets,
		enti->spri.sprite_vrl_cont.buffer + sizeof(struct vrl1_vgax_header),
		enti->spri.sprite_vrl_cont.data_size
	);
#endif
#ifdef __DEBUG_SPRI__
//	if(ggvv->player[0].enti.q<5)
	if(dbg_delayanimation)
	{
		{
			delay(250);
		//	static struct glob_game_vars *ggvv; IN_Ack(ggvv);
		//	{ while(!IN_KeyDown(sc_Space)/* && !IN_KeyDown(sc_Escape)*/){} delay(250); }
		}
	}
#endif
	if(!video->vga_state.rss)
	{
		// restore ptr
		vga_state.vga_graphics_ram = omemptr;

		if(!video->vga_state.bgps){
			// block copy to visible RAM from offscreen
			vga_setup_wm1_block_copy();
			o = video->ofs.offscreen_ofs; // source offscreen
			o2 = (ry * video->page[0].stridew) + (rx >> 2); // dest visible (original stride)
			for (i=0;i < h;i++,o += vga_state.vga_draw_stride,o2 += video->page[0].stridew) vga_wm1_mem_block_copy(o2,o,w >> 2);
			// must restore Write Mode 0/Read Mode 0 for this code to continue drawing normally
			vga_restore_rm0wm0();
		}
	}
	// restore stride
	vga_state.vga_draw_stride_limit = vga_state.vga_draw_stride = video->page[0].stridew;

	vga_state.vga_graphics_ram = video->vga_state.omemptr;
	video->dorender=1;
}
