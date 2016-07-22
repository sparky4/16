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

//TODO: USE 16_MM! AND 16_CA!
// Read .vrs file into memory
int read_vrs(char *filename, struct vrs_container *vrs_cont){
	// Initialise a local copy of becessary variables
	// so vrs_cont won't be dirty on error
	int fd;
	dword size;
	byte *buffer;
	// Open filename, get size of file,
	// populate the vrs_container if all tests pass
	fd = open(filename, O_RDONLY|O_BINARY);
	// Insert sanity cheks later
	size = lseek(fd, 0, SEEK_END);
	buffer = malloc(size);
	lseek(fd, 0, SEEK_SET);
	read(fd, buffer, size);
	vrs_cont->size = size;
	vrs_cont->buffer = buffer;
	// 0 is an invalid value for ids under vrs specifications,
	// so it is safe to flush ids to this value
	vrs_cont->anchor_sprite_id = 0;
	vrs_cont->current_sprite_id = 0;
	return 0;
}

//TODO: USE 16_MM! AND 16_CA!
// Seek and return a specified .vrl blob from .vrs blob in memory
struct vrl_container* get_vrl_by_id(struct vrs_container *vrs_cont, uint16_t id){
	uint16_t *ids;
	uint32_t *vrl_list;
	struct vrl_container *vrl_cont;
	int counter = 0;
	// If id is invalid, return null
	if(id == 0){
		// Probably add an error message?
		return 0;
	}
	// Get id list from .vrs blob (base + offset)
	ids = (uint16_t*)vrs_cont->buffer + (dword)vrs_cont->vrs_hdr->offset_table[VRS_HEADER_OFFSET_SPRITE_ID_LIST];
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
	vrl_list = (uint32_t *)(vrs_cont->buffer + vrs_cont->vrs_hdr->offset_table[VRS_HEADER_OFFSET_VRS_LIST]);
	// Allocate memory for vrl_cont
	vrl_cont = (struct vrl_container*)malloc(sizeof(struct vrl_container));
	// Get vrl_header from .vrs (base + offset from vrl_list)
	// Counter is number of vrls to skip (ids and vrls are aligned according to the .vrs specification)
	vrl_cont->vrl_header = (struct vrl1_vgax_header*)(vrs_cont->buffer + vrl_list[counter]);
	// Get .vrl size by integer arithmetics (next vrl - current vrl)
	// Untested. May be an incorrect way to do so
	vrl_cont->size = vrl_list[counter+1] - vrl_list[counter];
	return vrl_cont;
}
