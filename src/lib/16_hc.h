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
/*
	heap test stuff
*/

#ifndef __16HC__
#define __16HC__

#include "src/lib/16_head.h"

#ifdef __BORLANDC__
#define CHIKAKU			near
#define _nmalloc(size)          malloc(size)
#define _nfree(block)           free(block)
#define _nrealloc(block,size)   realloc(block,size)
#define _ncalloc(num,size)      calloc(num,size)
#define _nheapmin()             0
#endif
#ifdef __WATCOMC__
//#define SETTOUJ I_CHIKAKU	_n
#define CHIKAKU			__near
#endif

void CHIKAKU* HC_LargestFreeBlock(size_t* Size);
size_t HC_coreleft(void);
void far* HC_LargestFarFreeBlock(size_t* Size);
size_t HC_farcoreleft(void);
//void huge* HC_LargestHugeFreeBlock(size_t* Size);
//size_t HC_hugecoreleft(void);
//void __based(__self)* LargestBasedFreeBlock(size_t* Size);
//size_t _basedcoreleft(void);
size_t HC_GetFreeSize(void);
size_t HC_GetFarFreeSize(void);
size_t HC_GetNearFreeSize(void);
void HC_heapdump(global_game_variables_t *gvar);
void HC_heapstat(global_game_variables_t *gvar, int heap_status, byte *str);
void HC_heapstat0(int heap_status);

void HC_OpenDebug(global_game_variables_t *gvar);
void HC_CloseDebug(global_game_variables_t *gvar);

#endif /* __16HC__ */
