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
#include "src/lib/16_vrs.h"
#include "src/lib/typedefst.h"

extern global_game_variables_t gvar;

// Read .vrs file into far memory
int read_vrs(char *filename, struct vrs_container *vrs_cont){
	int fd;
	dword size;
	byte huge *buffer;
	// Open filename, get size of file,
	// populate the vrs_container if all tests pass
	fd = open(filename, O_RDONLY|O_BINARY);
	size = filelength(fd);
	close(fd);
	// Insert sanity cheks later
	CA_LoadFile(filename, buffer, gvar->mm, gvar->mmi);
	vrs_cont->size = size;
	vrs_cont->buffer = buffer;
	return 0;
}

// Seek and return a specified .vrl blob from .vrs blob in far memory
struct vrl_container get_vrl_by_id(struct vrs_container huge *vrs_cont, uint16_t id){
	uint16_t huge *ids;
	uint32_t huge *vrl_list;
	struct vrl_container huge *vrl_cont;
	int counter = 0;
	// If id is invalid, return null
	if(id == 0){
		// Probably add an error message?
		return 0;
	}
	// Get id list from .vrs blob (base + offset)
	ids = (uint16_t huge*)vrs_cont->buffer + 
		(dword)vrs_cont->vrs_hdr->offset_table[VRS_HEADER_OFFSET_SPRITE_ID_LIST];
	// Loop through the id list until we found the right one or hit the end of the list
	// Counter is keeping track of the offset(in ids/vrl blobs)
	while(ids[counter] != id && ids[counter]){
		counter++;
	}
	// Return null if we couldn't find the requested id
	if(!ids[counter]){
		// Error message?
		return 0;
	}
	// Get vrl list from .vrs blob (base + offset)
	vrl_list = (uint32_t huge *)(vrs_cont->buffer + 
					vrs_cont->vrs_hdr->offset_table[VRS_HEADER_OFFSET_VRS_LIST]);
	// Allocate memory for vrl_cont
	vrl_cont = (struct vrl_container)malloc(sizeof(struct vrl_container));
	// Get vrl_header from .vrs (base + offset from vrl_list)
	// Counter is number of vrls to skip (ids and vrls are aligned according to the .vrs specification)
	vrl_cont->vrl_header = (struct vrl1_vgax_header huge *)(vrs_cont->buffer + vrl_list[counter]);
	// Get .vrl size by integer arithmetics (next vrl - current vrl)
	// Untested. May be an incorrect way to do so
	vrl_cont->size = vrl_list[counter+1] - vrl_list[counter];
	return vrl_cont;
}
