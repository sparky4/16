/* Project 16 Source Code~
 * Copyright (C) 2012-2015 sparky4 & pngwen & andrius4669
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

int heaphandle;

void __near* LargestFreeBlock(size_t* Size);
size_t _coreleft(void);
void far* LargestFarFreeBlock(size_t* Size);
size_t _farcoreleft(void);
void huge* LargestHugeFreeBlock(size_t* Size);
size_t _hugecoreleft(void);
//void __based(__self)* LargestBasedFreeBlock(size_t* Size);
//size_t _basedcoreleft(void);
size_t GetFreeSize(void);
size_t GetFarFreeSize(void);
size_t GetNearFreeSize(void);
void heapdump(void);
void heapstat(int heap_status, byte *str);
void heapstat0(int heap_status);

void HC_OpenDebug();
void HC_CloseDebug();

#endif /* __16HC__ */
