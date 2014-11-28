/* tab size = 8 */

#include "lib\wtest\wtest.h"

static unsigned char detectcpu()
{
	unsigned char cputype;
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

/*int main(int argc, char **argv)
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
}*/

