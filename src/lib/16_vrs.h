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
#ifndef __16_VRS__
#define __16_VRS__

#include "src/lib/16_head.h"
#include "src/lib/modex16.h"
//#include <hw/cpu/cpu.h>
//#include <hw/dos/dos.h>
//#include <hw/vga/vga.h>

// Container for .vrs files loaded in memory with useful info
// Includes:
// + size of the .vrs blob in memory
// + pointer to the blob/vrs header
// + id of the curent (shown) animation
// + id of the first sprite of the curret animation (supplementary)
// + id of the current (shown) sprite

struct vrs_container{
	dword size;
	union{
		byte *buffer;
		struct vrs_header *vrs_hdr;
	};
	uint16_t anchor_sprite_id;
	uint16_t current_sprite_id;
};

// Container for .vrl files loaded in memory with useful info
// Includes:
// + size of the .vrl blob in memory
// + pointer to the blob/vrl header
struct vrl_container{
	dword size;
	union{
		byte *buffer;
		struct vrl1_vgax_header *vrl_header;
	};
};

// Read .vrs file into memory
// In:
// + char *filename - name of the file to load
// + struct vrs_container *vrs_cont - pointer to the vrs_container
// to load the file into
// Out:
// + int - 0 on succes, 1 on failure
int read_vrs(char *filename, struct vrs_container *vrs_cont);

// Seek and return a specified .vrl blob from .vrs blob in memory
// In:
// + struct vrs_container *vrs_cont - pointer to the vrs_container
// with a loaded .vrs file
// + uint16_t id - id of the vrl to retrive
// Out:
// struct vrl_container* - a pointer to a vrl_container with a pointer
// to the requested .vrl blob
struct vrl_container* get_vrl_by_id(struct vrs_container *vrs_cont, uint16_t id);

#endif
