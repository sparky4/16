/* Project 16 Source Code~
 * Copyright (C) 2012-2016 sparky4 & pngwen & andrius4669 & joncampbell123
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
/* tab size = 8 */

#include "src/lib/wcpu/wcpu.h"

byte detectcpu()
{
	byte cputype;
	__asm
	{
		PUSHF			; we gonna modify flags, so back them up
		; first check if its 8086/8088 or 80186/80188
		PUSHF			; FLAGS -> STACK
		POP	AX		; STACK -> AX
		AND	AX, 00FFFh	; clear 12-15 bits (they are always 1 on 8086/8088 and 80186/80188)
		PUSH	AX		; AX -> STACK
		POPF			; STACK -> FLAGS
		; this is where magic happen
		PUSHF			; FLAGS -> STACK
		POP	AX		; STACK -> AX
		AND	AX, 0F000h	; 0-11 bits aren't important to us
		CMP	AX, 0F000h	; check if all 12-15 bits are set (simple comparision)
		JNE	_286p		; if no, 286+ CPU
		MOV	cputype, 0	; if yes, we are done, set cputype to 0 and end this
		JMP	_done
	_286p:
		; now check if its 286 or newer
		PUSHF			; FLAGS -> STACK
		POP	AX		; STACK -> AX
		OR	AX, 07000h	; set 12-14 bits (they are always cleared by 286 if its real mode)
		PUSH	AX		; AX -> STACK
		POPF			; STACK -> FLAGS
		; this is where magic happen
		PUSHF			; FLAGS -> STACK
		POP	AX		; STACK -> AX
		TEST	AX, 07000h	; check if at least 1 bit in 12-14 bits range is set (15th won't be set anyway)
		JNZ	_386p		; not all bits clear, its 386+
		MOV	cputype, 1	; all bits clear, its 286, we are done
		JMP	_done
	_386p:
		MOV	cputype, 2	; its 386 or newer, but we don't care if its newer at this point
	_done:
		POPF			; restore flags we backed up earlier
	}
	return cputype;
}

#ifdef TEST
int main(int argc, char **argv)
{
	const char *cpus;
	unsigned char cput;

	cput = detectcpu();
	switch(cput)
	{
		case 0: cpus = "8086/8088 or 186/88"; break;
		case 1: cpus = "286"; break;
		case 2: cpus = "386 or newer"; break;
		default: cpus = "internal error"; break;
	}
	printf("detected CPU type: %s\n", cpus);
	return 0;
}
#endif

