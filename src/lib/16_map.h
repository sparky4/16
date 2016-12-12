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

//#define DEBUG_MAPDATA
//#define DEBUG_MAPVAR
//#define DEBUG_DUMPVARS
//#define DEBUG_JS

//TODO: 16_mm and 16_ca must handle this
typedef struct {
	bitmap_t huge *btdata;		//old
	planar_buf_t huge *data;	//old
	word tileHeight, tileWidth;
	unsigned int rows, cols;
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
	byte *data;			//TODO: 16_mm and 16_ca must handle this
	byte * huge *layerdata;	//TODO: 16_mm and 16_ca must handle this
	tiles_t *tiles;		//TODO: 16_mm and 16_ca must handle this
	tiles_t * huge *layertile;	//TODO: 16_mm and 16_ca must handle this
	int width, height;		//this has to be signed!
	char		name[16];
} map_t;

int jsoneq(const char *json, jsmntok_t *tok, const char *s);
word dump(const char *js, jsmntok_t *t, size_t count, word indent, char *js_sv, map_t *map, dword q);
int loadmap(char *mn, map_t *map);
int newloadmap(char *mn, map_t *map);

#endif/*_16_LIB__H_*/
