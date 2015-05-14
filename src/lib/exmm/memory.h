// Cut this out as memory.h

/*      File:       Memory.h
 *      Module:     All Modules
 *      Author(s):  Chris Somers
 *      Date:       August 5, 1993
 *      Version:    V.1.1

        modified by Alex Russell to simplify.
 */


#ifndef _MEMORY_DEF
#define _MEMORY_DEF 1

#include "emmret.h"
#include "emmsize.h"

#define     CMM                  0
#define     EMM                  1

extern int      OpenEMM(void);
extern void     CloseEMM(void);
extern int      EMMInstalled(void);
extern void far *EMMalloc(int *Handle, int Pages);
extern int      EMMRealloc(int Handle, int Pages);
extern void     EMMFree(int Handle);
extern int      MapEMM(int Handle, int Start, int Pages);
extern void     UnmapEMM(int Handle, int Start, int Pages);
extern int      UseEMM(int Handle, int Start, int Pages);
extern void     SaveEMM(int Handle, int Start, int Pages);
extern unsigned long EMMCoreLeft(void);

#endif
