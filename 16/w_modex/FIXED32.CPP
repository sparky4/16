#include <stdio.h>

#include "fixed32.hpp"

Fixed32 SinTab[256];
Fixed32 CosTab[256];


void
initFixed32(void)
{
    FILE *fp;

    fp = fopen("sintab.dat", "rb");
    fread(SinTab, 4, 256, fp);
    fread(CosTab, 4, 256, fp);
    fclose(fp);
}


Fixed32
FixedMul(Fixed32 num1, Fixed32 num2)
{
    long Mm1, Mm2;
    short int MM, mm;
    short int hi1, hi2, lo1, lo2;

    hi1 = (num1 >> 16);
    hi2 = (num2 >> 16);
    lo1 = (num1 & 0xFFFF);
    lo2 = (num2 & 0xFFFF);

    MM  = (hi1 * hi2);
    Mm1 = (lo2 * hi1);
    Mm2 = (lo1 * hi2);
    mm  = (lo1 * lo2);

    return (Mm1 + Mm2 + mm + (((long)MM) << 16));
}


Fixed32
FixedDiv(Fixed32 numer, Fixed32 denom)
{
    return (numer / ROUND_FIXED_TO_INT(denom));
}


void
CosSin(Iangle theta, Fixed32 *Cos, Fixed32 *Sin)
{
    *Sin = SinTab[theta];
    *Cos = CosTab[theta];
}


/* ASM fixedpoint math routines
;--------------------------------------------------
; Sqrt - Fixed Point Square Root (High/Normal Precision)
;    IN     : ecx
;   OUT     : eax
;  Modified : ebx,ecx,edx
Sqrt         PROC

;This is the High Precision version for the sqrt.
;It gives the optimal 8.16 precision but takes
;a little longer (24 iterations, 48 bits intead of
;16 iterations and 32 bits)

	xor     eax,eax         ;eax is root
	mov     ebx,40000000h
sqrt1:
	mov     edx,ecx         ;edx = val
	sub     edx,ebx         ;val - bitsqr
	jb      sqrt2
	sub     edx,eax         ;val - root
	jb      sqrt2
	mov     ecx,edx         ;val >= (root+bitsqr) -> accept subs
	shr     eax,1           ;root >> 1
	or      eax,ebx         ;root | bitsqr
	shr     ebx,2           ;bitsqr>>2
	jnz     sqrt1
	jz      sqrt5
sqrt2:
	shr     eax,1           ;val < (root+bitsqr) -> dont change val
	shr     ebx,2           ;bitsqr>>2
	jnz     sqrt1
; we now have the 8.8 precision

sqrt5:
	mov     ebx,00004000h
	shl     eax,16
	shl     ecx,16
sqrt3:
	mov     edx,ecx         ;edx = val
	sub     edx,ebx         ;val - bitsqr
	jb      sqrt4
	sub     edx,eax         ;val - root
	jb      sqrt4
	mov     ecx,edx         ;val >= (root+bitsqr) -> accept subs
	shr     eax,1           ;root >> 1
	or      eax,ebx         ;root | bitsqr
	shr     ebx,2           ;bitsqr>>2
	jnz     sqrt3
	ret
sqrt4:
	shr     eax,1           ;val < (root+bitsqr) -> dont change val
	shr     ebx,2           ;bitsqr>>2
	jnz     sqrt3
	ret

Sqrt           ENDP
*/

