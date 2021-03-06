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
#include "src/lib/16_vrs.h"

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
		enti->spri.spritesheet.vrl_line_offsets[i] = vrl1_vgax_genlineoffsets(curr_vrl, (byte *)curr_vrl + sizeof(struct vrl1_vgax_header), vrl_size);
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
