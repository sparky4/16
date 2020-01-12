/* Project 16 Source Code~
 * Copyright (C) 2012-2020 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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
	heap check
*/

#ifndef __16HC__
#define __16HC__

#include "src/lib/16_head.h"

typedef struct heapusage
{
	unsigned long h_free, h_total, h_used;
	int heap_status;
} hc_use_t;

#ifdef __WATCOMC__
#define NPTR __near
#endif

#ifdef __BORLANDC__
#define NPTR

#define _HEAPBADBEGIN   2       /* heap header is corrupted */
#define _HEAPBADNODE    3       /* heap entry is corrupted */
#define _HEAPBADPTR     5       /* invalid heap entry pointer (_heapwalk) */
#endif

//long HC_Newfarcoreleft();
void NPTR* HC_LargestFreeBlock(size_t* Size);
size_t HC_coreleft(void);
void far* HC_LargestFarFreeBlock(dword* Size);
unsigned long HC_farcoreleft(void);
//size_t HC_GetFreeSize(void);
unsigned long HC_GetFarFreeSize(void);
size_t HC_GetNearFreeSize(void);

void HC_heapdump(global_game_variables_t *gvar);
void HCL_heapstatLogWrite(global_game_variables_t *gvar, int heap_status, byte *str);
void HCL_heapstat(int heap_status);

void HC_OpenDebug(global_game_variables_t *gvar);
void HC_CloseDebug(global_game_variables_t *gvar);

#ifdef __WATCOMC__
unsigned long farcoreleft();
unsigned long coreleft();
#endif

#endif /* __16HC__ */
