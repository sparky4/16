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

#ifndef _16_MAP__H_
#define _16_MAP__H_

#include "src/lib/16_head.h"
#include "src/lib/jsmn/jsmn.h"
//#include "stdio.h" included in 16_head.h
//#include "stdlib.h" included in 16_head.h
#include "src/lib/16_vl.h"
#include "src/lib/16_ca.h"
#include "src/lib/16_mm.h"

//TODO: 16_mm and 16_ca must handle this
typedef struct {
	bitmap_t huge *btdata;		//old
	planar_buf_t huge *data;	//old
	word tileHeight;
	word tileWidth;
	unsigned int rows;
	unsigned int cols;
	#ifdef __DEBUG__
	boolean debug_text;	//show the value of the tile! wwww
	byte *debug_data;
	#endif
} tiles_t;

//TODO: 16_mm and 16_ca must handle this
//TODO: add variables from 16_ca
typedef struct {
	//long		planestart[3];
	//unsigned	planelength[3];
	byte	* huge *data;	//TODO: 16_mm and 16_ca must handle this
	tiles_t **tiles;		//TODO: 16_mm and 16_ca must handle this
	unsigned int width, height;		//this has to be positive
	char		name[16];
} map_t;

int loadmap(char *mn, map_t *map);

#endif/*_LIBMAPREAD_H_*/
