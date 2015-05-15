#include <stdio.h>
#include <stdlib.h>
#include <mem.h>

#include "emmret.h"
#include "emmsize.h"

#define     CMM                  0
#define     EMM                  1

static int
EMReady(void)
{
	int EMSready;
	_asm {
		mov     ah,0x40             /* get EM Manager Status */
		int     0x67
		or      ah,ah
		jns     Ready               /* returns 80, 81, or 84 hex on error */
		mov		EMSready,FALSE
		jmp End
		Ready:
		mov		EMSready,TRUE
		End:
	}
	return(EMSready);

//Ready:
//    return(TRUE);
}               /* End of EMReady() */

static int
EMVer(void)
{
	int EMSver;
	_asm {
		mov     ah,0x46
		int     0x67
		mov EMSver,ax
	}
	return(EMSver);
}

int main()
{
	int pee;
	int rpee;
	pee=EMVer();
	rpee=EMReady();
	printf("%x\n", pee);
	printf("%x\n", rpee);
	return 0;
}
