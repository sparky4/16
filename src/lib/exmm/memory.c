
/*      File:       Memory.c
 *      Module:     All Modules
 *      Author(s):  Chris Somers
 *      Date:       December 1, 1992
 *      Version:    V.1.1w

        minor mods by Alex Russell to simplify

        Must use memory model with FAR code

		Open Watcom patch by sparky4~

 */



#if !defined(__LARGE__) && !defined(__COMPACT__) && !defined(__HUGE__)
#error Invalid memory model for compiling MEMORY.C
#endif

#include <stdio.h>
#include <dos.h>
#include <mem.h>

#include "memory.h"

 //static globals --------------------------------

static int  ActiveEMList[MAXEMHANDLES];
static unsigned int  EMMSeg;

 //forward declarations ---------------------------------

static int  EMPresent(void);
static int  EMReady(void);
static unsigned int  GetEMMSeg(void);
static int  GetEMHandle(int NumPages);
static int  EMMap(int Handle, int LogPg, int PhyPg);
static int  FreeEMHandle(int Handle);
static int  GetNumPages(int Handle);
static int  EMStateSave(int Handle);
static void EMStateRestore(int Handle);

/********************************************************************/

int
EMVer(void)
{
	int EMSver;
	_asm
	{
		mov		ah,0x46
		int		0x67
		mov		EMSver,ax
	}
	return(EMSver);
}

/********************************************************************/
int
OpenEMM(void)
{
    if (!EMPresent() || !EMReady()) return(NOTREADY);
    if (!(EMMSeg = GetEMMSeg())) return(NOTREADY);  /*lint !e720 */
    return(SUCCESS);
}               /* End of OpenEMM() */

/********************************************************************/

void
CloseEMM(void)
{
    int     i;

    if (!EMMSeg) return;
    for (i = 0; i < MAXEMHANDLES; i++) {
        if (ActiveEMList[i]) {
            FreeEMHandle(ActiveEMList[i]);
            ActiveEMList[i] = 0;
        }
    }
    EMMSeg = 0;
}               /* End of CloseEMM() */

/********************************************************************/

int
EMMInstalled(void)
{
    return((EMMSeg) ? TRUE : FALSE);        /* successfully opened? */
}               /* End of EMMInstalled() */

/********************************************************************/

unsigned long
EMMCoreLeft(void)
{
	unsigned      Pages;
	unsigned long RtnVal = 0UL;
	unsigned short interr=0;

	_asm {
		mov     ah,0x42             /* get EMM free page count */
		int     0x67
		or      ah,ah
		js      InternalError       /* returns 80, 81, or 84 hex on error */
		mov     Pages,bx            /* number of unallocated 16K pages */
		jmp End
		InternalError:
		mov		interr,1
		End:
	}
	if(!interr)
	RtnVal = ((unsigned long)Pages << 14);  /* Pages * 16K rtns bytes*/

	return(RtnVal);
}               /* End of EMMCoreLeft() */

/********************************************************************/

void far *
EMMalloc(int *Handle, int Pages)
{
    int     i;
    char    *RtnPtr = NULL;

    if (!EMMSeg) {
        *Handle = NOTREADY;
        return(NULL);
    }
    if ((Pages < 1) || (Pages > 1020)) {
        *Handle = VALUE_OUTF_RANGE;
        return (NULL);
    }
    for (i = 0; (i < MAXEMHANDLES) && (ActiveEMList[i]); i++) ;
    if (i == MAXEMHANDLES) {
        *Handle = NOFREEITEMS;
        return (NULL);
    }
    if ((ActiveEMList[i] = GetEMHandle(Pages)) > 0) {
        RtnPtr = MK_FP(EMMSeg, 0);
    }
    *Handle = ActiveEMList[i];
    return((void far *)RtnPtr);
}               /* End of EMMalloc() */

/********************************************************************/

int
EMMRealloc(int Handle, int Pages)
{
	int     RtnCode = FALSE;

	if (!EMMSeg || (Pages < 0) || (Pages > 1020)) {
		return (FALSE);
	}
	_asm {
		mov     ah,0x51             /* change # of pages */
		mov     bx,Pages
		mov     dx,Handle
		int     0x67
		or      ah,ah
		js      NoGo                /* returns 80 to 88 hex on error */
		mov		RtnCode,TRUE
		NoGo:
	}

//NoGo:
	return(RtnCode);
}               /* End of EMMRealloc() */

/********************************************************************/

void
EMMFree(int Handle)
{
    int     i, j;

    if (!EMMSeg) return;
    for (i = 0; (i < MAXEMHANDLES) && (ActiveEMList[i] != Handle); i++) ;
    if (i >= MAXEMHANDLES) return;
    j = 16;
    while (j--) {
        if (FreeEMHandle(ActiveEMList[i])) break;
    }
    ActiveEMList[i] = 0;
}               /* End of EMMFree() */

/********************************************************************/

int                                         /* EMM map for application */
MapEMM(int Handle, int Start, int Pages)
{
    int     i;

    if (!EMMSeg) return(NOTREADY);
    for (i = 0; (i < MAXEMHANDLES) && (ActiveEMList[i] != Handle); i++) ;
    if (i == MAXEMHANDLES) return (NO_DATA);
    if ((GetNumPages(Handle) < Pages) || (Pages < 1) || (Pages > 4)) {
        return (VALUE_OUTF_RANGE);
    }
    for (i = Start; i < Start + Pages; i++) {
        if (!EMMap(Handle, i, i - Start)) return(NO_DATA);
    }
    return(SUCCESS);
}               /* End of MapEMM() */

/********************************************************************/

void                                        /* EMM unmap for application */
UnmapEMM(int Handle, int Start, int Pages)
{
    int     i, j;

    if (!EMMSeg) return;
    for (i = 0; (i < MAXEMHANDLES) && (ActiveEMList[i] != Handle); i++) ;
    if (i == MAXEMHANDLES) return;
    j = Start + Pages;
    if ((Pages < 1) || (j > 4)) return;

    for (i = Start; i < j; i++) {
        EMMap(Handle, NONE, i);
    }
}               /* End of UnmapEMM() */

/********************************************************************/

int                     /* EMM map for devices - saves EMM state */
UseEMM(int Handle, int Start, int Pages)
{
    EMStateSave(Handle);
    return(MapEMM(Handle, Start, Pages));
}               /* End of UseEMM() */

/********************************************************************/

void                    /* EMM unmap for devices - restores EMM state */
SaveEMM(int Handle, int Start, int Pages)
{
    UnmapEMM(Handle, Start, Pages);
    EMStateRestore(Handle);
}               /* End of SaveEMM() */

/********************************************************************/

static int
EMPresent(void)
{
    int     i, Segment;
    char    EMName[] = "EMMXXXX0";
    char    *s, *t;

    _asm {                      /* can be replaced with getvect() */
        push    es
        mov     ax,0x3567       /* get vector for int 67h */
        int     0x21
        mov     ax,es
        mov     Segment,ax
        pop     es
    }
    t = MK_FP(Segment, 0x0A);   /* point to driver name */
    s = EMName;
    for (i = 0; (i < 8) && (*s++ == *t++); i++) ;   /* strncmp equivalent */

    if (i == 8) return(TRUE);
    return(FALSE);
}               /*End of EMPresent() */

/********************************************************************/

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

/********************************************************************/

static unsigned int
GetEMMSeg(void)
{
	unsigned int     EMSegment;

	_asm {
		mov     ah,0x41             /* get EMM page frame segment */
		int     0x67
		or      ah,ah
		js      NotReady            /* returns 80, 81, or 84 hex on error */
		mov     EMSegment,bx
		jmp End
		NotReady:
		mov     EMSegment,NOTREADY
		End:
	}
	return(EMSegment);              /*lint !e530 */

//NotReady:
//    return(NOTREADY);
}               /* End of GetEMMSeg() */

/********************************************************************/

static int
GetEMHandle(int NumPages)
{
	int     NewHandle;

	_asm {
		mov     ah,0x43             /* get handle and allocate EM */
		mov     bx,NumPages         /* number of 16K pages to allocate */
		int     0x67
		or      ah,ah               /* returns 80 to 89 hex on error */
		js      NoHandle
		mov     NewHandle,dx        /* retrieve handle */
		jmp End
		NoHandle:
		mov		NewHandle,NO_DATA
		End:
	}
	return(NewHandle);

//NoHandle:
//    return(NO_DATA);
}               /* End of GetEMHandle() */

/********************************************************************/

static int
EMMap(int Handle, int LogPg, int PhyPg)
{
	int     RtnCode = NO_DATA;

	_asm {
		mov     ax,PhyPg            /* physical page: 0 - 3 in AL only */
		mov     ah,0x44             /* map logical to physical page */
		mov     bx,LogPg            /* logical page: 0 - 1020 */
		mov     dx,Handle
		int     0x67
		or      ah,ah               /* returns 80 to 8B hex on error */
		js      NoMapping
		mov		RtnCode,SUCCESS
//		jmp End
		NoMapping:
//		End:
	}
//    RtnCode = SUCCESS;

//NoMapping:
	return(RtnCode);
}               /* End of EMMap() */

/********************************************************************/

static int
FreeEMHandle(int Handle)
{
	int FreeEMShandle;
	_asm {
		mov     ah,0x45             /* free handle and deallocate EM */
		mov     dx,Handle
		int     0x67
		or      ah,ah               /* returns 80 to 86 hex on error */
		js      NotFreed
		mov		FreeEMShandle,SUCCESS
		jmp End
		NotFreed:                           /* must retry if unsuccessful */
		mov		FreeEMShandle,NO_DATA
		End:
	}
	return(FreeEMShandle);

//NotFreed:                           /* must retry if unsuccessful */
//    return(NO_DATA);
}               /* End of FreeEMHandle() */

/********************************************************************/

static int
GetNumPages(int Handle)
{
	int     NumPages = 0;

	_asm {
		mov     ah,0x4C             /* get allocated pages for Handle */
		mov     dx,Handle
		int     0x67
		or      ah,ah               /* returns 80 to 84 hex on error */
		js      BadHandle
		mov     NumPages,bx
//		jmp End
		BadHandle:
//		End:
	}

//BadHandle:
	return(NumPages);
}               /* End of GetNumPages() */

/********************************************************************/

static int
EMStateSave(int Handle)
{
	int     RtnCode = NO_MEMORY;
	_asm {
		mov     ah,0x47             /* save page map under Handle */
		mov     dx,Handle
		int     0x67
		or      ah,ah
		js      Unsaved             /* out of save space error */
		mov		RtnCode,SUCCESS
		Unsaved:
	}
//    RtnCode = SUCCESS;

//Unsaved:
    return(RtnCode);
}               /* End of EMStateSave() */

/********************************************************************/

static void
EMStateRestore(int Handle)
{
    _asm {
        mov     ah,0x48             /* restore page map for Handle */
        mov     dx,Handle
        int     0x67                /* ignore error */
    }
}               /* End of EMStateRestore() */
