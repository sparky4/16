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
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "src/lib/16_snd.h"

void opl2out(word data, word reg)
{
	__asm
	{
		mov     ax,reg
		mov     dx,word ptr [OPLPORT]
		or      ah,ah
		jz      @@1
		add     dx,2
@@1:    out     dx,al
		mov     cx,6
@@2:    in      al,dx
		loop    @@2
		inc     dl
		mov     ax,data
		out     dx,al
		dec     dl
		mov     cx,36
@@3:    in      al,dx
		loop    @@3
	}
}

void opl3out(word data, word reg)
{
	__asm
	{
		mov     ax,reg
		mov     dx,word ptr [OPLPORT]
		or      ah,ah
		jz      @@1
		add     dx,2
@@1:    out     dx,al
		inc     dl
		mov     ax,data
		out     dx,al
		dec     dl
		mov     cx,26
@@2:    in      al,dx
		loop    @@2
	}
}

void opl3exp(word data)
{
	__asm
	{
		mov     ax,data
		mov     dx,word ptr [OPLPORT]
		add     dx,2
		out     dx,al
		mov     cx,6
@@1:    in      al,dx
		loop    @@1
		inc     dl
		mov     al,ah
		out     dx,al
		mov     cx,36
@@2:    in      al,dx
		loop    @@2
	}
}