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

#ifndef _LIBMAPREAD_H_
#define _LIBMAPREAD_H_

#include "src/lib/jsmn/jsmn.h"
#include "src/lib/16_vl.h"
#include "src/lib/16_head.h"

//#define DEBUG_MAPDATA
//#define DEBUG_MAPVAR
//#define DEBUG_DUMPVARS
//#define DEBUG_JS

typedef struct {
	bitmap_t huge *btdata;
	planar_buf_t huge *data;
	word tileHeight;
	word tileWidth;
	unsigned int rows;
	unsigned int cols;
	boolean debug_text;	//show the value of the tile! wwww
	byte *debug_data;
} tiles_t;

typedef struct {
	byte	*data;
	tiles_t *tiles;
	int width;	//this has to be signed!
	int height;	//this has to be signed!
} map_t;

int jsoneq(const char *json, jsmntok_t *tok, const char *s);
word dump(const char *js, jsmntok_t *t, size_t count, word indent, char *js_sv, map_t *map, dword q);
int loadmap(char *mn, map_t *map);

#endif/*_LIBMAPREAD_H_*/
