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

#include "src/lib/modex16.h"
#include "src/lib/typdefst.h"
//#include <hw/cpu/cpu.h>
//#include <hw/dos/dos.h>
#include <hw/vga/vrl.h>
#include "src/lib/16_ca.h"

struct vrs_container{
	// Size of a .vrs lob in memory
	dword size;
	union{
		byte huge *buffer;
		struct vrs_header huge *vrs_hdr;
	};
	// Array of corresponding vrl line offsets
	vrl1_vgax_offset_t **vrl_line_offsets;
};

struct vrl_container{
	// Size of a .vrl blob in memory
	dword size;
	union{
		byte huge *buffer;
		struct vrl1_vgax_header huge *vrl_header;
	};
	// Pointer to a corresponding vrl line offsets struct
	vrl1_vgax_offset_t *line_offsets;
};

/* Read .vrs file into memory
* In:
* + char *filename - name of the file to load
* + struct vrs_container *vrs_cont - pointer to the vrs_container
* to load the file into
* Out:
* + int - 0 on succes, 1 on failure
*/
int read_vrs(global_game_variables_t *gvar, char *filename, struct vrs_container *vrs_cont);

/* Seek and return a specified .vrl blob from .vrs blob in memory
* In:
* + struct vrs_container *vrs_cont - pointer to the vrs_container
* with a loaded .vrs file
* + uint16_t id - id of the vrl to retrive
* Out:
* struct vrl_container* - a pointer to a vrl_container with a pointer
* to the requested .vrl blob
*/
struct vrl_container * get_vrl_by_id(struct vrs_container *vrs_cont, uint16_t id);

#endif
