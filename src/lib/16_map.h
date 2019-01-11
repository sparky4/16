/* Project 16 Source Code~
 * Copyright (C) 2012-2019 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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
#include "src/lib/16_tdef.h"
#include "src/lib/jsmn/jsmn.h"
//#include "stdio.h" included in 16_head.h
//#include "stdlib.h" included in 16_head.h
#include "src/lib/16_vl.h"
#include "src/lib/16_ca.h"
#include "src/lib/16_mm.h"
#include "src/lib/16_pm.h"


//#define DEBUG_MAPDATA
//#define DEBUG_MAPVARS
//0000++#define DEBUG_OBVARS
//#define DEBUG_JS

//typedefs moved to 16_tdef.h

int jsoneq(const char *json, jsmntok_t *tok, const char *s);
//word dump(const char *js, jsmntok_t *t, size_t count, word indent, char *js_sv, map_t *map, dword q);
//int loadmap(char *mn, map_t *map, global_game_variables_t *gvar);
int newloadmap(char *mn, map_t *map);
int CA_loadmap(char *mn, map_t *map, global_game_variables_t *gvar);

#endif/*_16_LIB__H_*/
