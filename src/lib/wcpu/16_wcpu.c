/* Project 16 Source Code~
 * Copyright (C) 2012-2021 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

#include "src/lib/wcpu/16_wcpu.h"

//#define USECPUPROBE

byte WCPU_detectfpu()
{
	byte fputype=0;
	word funk=0;
	boolean errflag=0;
	__asm {
		PUSHF						; we gonna modify flags, so back them up
.8087		fninit							; Initialize FPU
//		mov	[WORD PTR funk], 55AAh		; Set junk value
//		fnstsw	funk					; Store status word
//		cmp	[BYTE PTR funk], 0			; If it's not 0, no FPU
		mov	funk, 55AAh				; Set junk value
		fnstsw	funk					; Store status word
		cmp	funk, 0					; If it's not 0, no FPU
//		jne	short	_done
		jne	short	_err
		jmp	_fpu
		_err:
		mov	errflag, 1
		_fpu:
/*	}
	if(!errflag){
	printf(".");
	__asm {*/
		fnstcw	funk					; Store control word
		mov	ax, funk					; If the bits are not the way
		and	ax, 103Fh					; they should be, no FPU
		cmp	ax, 3Fh
//		jne	short _done
		je	short	__err
		jmp	__fpu
		__err:
		mov	errflag, 1
		__fpu:
	/*}}
	if(!errflag){
	printf(".");
	__asm {*/
//		and	[WORD PTR funk], 0FF7Fh		; Clear interrupt bit
		and	funk, 0FF7Fh				; Clear interrupt bit
		fldcw	funk						; Load control word
		fdisi							; Disable interrupts
		fstcw	funk						; Store control word
//		test	[BYTE PTR funk], 80h			; If it changed, it's an 8087
		test	funk, 80h					; If it changed, it's an 8087
		mov	fputype, 1

		_done:
		POPF						; restore flags we backed up earlier
	}//}
	//printf("WCPU_detectfpu():\n	fputype=%u\n	funk=%u	errflag=%u\n", fputype, funk, errflag);
#ifdef USECPUPROBE
	if (cpu_flags & CPU_FLAG_FPU) printf(" - With FPU\n");
#endif
	return fputype;
}

byte WCPU_detectcpu()
{
	byte cputype=0;
	__asm {
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

const char *WCPU_cpudetectmesg()
{
	const char *cpus;
	unsigned char cput;

	cput = WCPU_detectcpu();
	switch(cput)
	{
		case 0: cpus = "8086/8088 or 80186/80188"; break;
		case 1: cpus = "80286"; break;
		case 2: cpus = "80386 or newer"; break;
		default: cpus = "internal error"; break;
	}
	return cpus;
}

const char *WCPU_fpudetectmesg()
{
	char *fpus;
	unsigned char fput;
	word cput;

	fput = WCPU_detectfpu();
	cput = WCPU_detectcpu();
	switch(cput)
	{
		case 0: cput=80; break;
		case 1: cput=802; break;
		case 2: cput=803; break;
		default: cput=0; break;
	}
	switch(fput)
	{
		case 0: fpus = "none"; break;
		case 1: sprintf(fpus, "%d87", cput); break;
		default: fpus = "internal error"; break;
	}
	return fpus;
}

void WCPU_cpufpumesg()
{
#ifdef USECPUPROBE
	cpu_probe();
#endif
	printf("detected CPU type: %s\n",	WCPU_cpudetectmesg());
	printf("detected FPU type: %s\n",	WCPU_fpudetectmesg());
}
