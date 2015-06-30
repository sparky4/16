/* Catacomb Armageddon Source Code
 * Copyright (C) 1993-2014 Flat Rock Software
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

// ID_MM.H

#ifndef __16_EXMM__
#define __16_EXMM__

#include <string.h>
#include <malloc.h>
#include "src/lib/lib_head.h"
//++++mh	#include "src/lib/16_in.h"

//****#if 1		// 1 == Debug/Dev  ;  0 == Production/final
#define OUT_OF_MEM_MSG	"MM_GetPtr: Out of memory!\nYou were short :%ld bytes"
//****#else
//****#define OUT_OF_MEM_MSG	"\npee\n"
//****#endif


#define SAVENEARHEAP	0x400		// space to leave in data segment
#define SAVEFARHEAP		0			// space to leave in far heap

#define	BUFFERSIZE		0x1000		// miscelanious, allways available buffer

#define MAXBLOCKS		600


//--------

#define	EMS_INT			0x67

#define	EMS_STATUS		0x40
#define	EMS_GETFRAME	0x41
#define	EMS_GETPAGES	0x42
#define	EMS_ALLOCPAGES	0x43
#define	EMS_MAPPAGE		0x44
#define	EMS_FREEPAGES	0x45
#define	EMS_VERSION		0x46

//--------

#define	XMS_VERSION		0x00

#define	XMS_ALLOCHMA	0x01
#define	XMS_FREEHMA		0x02

#define	XMS_GENABLEA20	0x03
#define	XMS_GDISABLEA20	0x04
#define	XMS_LENABLEA20	0x05
#define	XMS_LDISABLEA20	0x06
#define	XMS_QUERYA20	0x07

#define	XMS_QUERYREE	0x08
#define	XMS_ALLOC		0x09
#define	XMS_FREE		0x0A
#define	XMS_MOVE		0x0B
#define	XMS_LOCK		0x0C
#define	XMS_UNLOCK		0x0D
#define	XMS_GETINFO		0x0E
#define	XMS_RESIZE		0x0F

#define	XMS_ALLOCUMB	0x10
#define	XMS_FREEUMB		0x11

//==========================================================================
//__segment                seg;
typedef void __based(__self) * memptr; //__based(seg) * memptr;

typedef struct
{
	dword	nearheap,farheap,EMSmem,XMSmem,mainmem;
	boolean		mmstarted, bombonerror, mmerror;
} mminfotype;

//==========================================================================

/*extern	mminfotype	mminfo;
extern	memptr		bufferseg;
extern	boolean		mmerror;

extern	void		(* beforesort) (void);
extern	void		(* aftersort) (void);*/

//==========================================================================

/*
=============================================================================

							LOCAL INFO

=============================================================================
*/

#define LOCKBIT		0x80	// if set in attributes, block cannot be moved
#define PURGEBITS	3		// 0-3 level, 0= unpurgable, 3= purge first
#define PURGEMASK	0xfffc
#define BASEATTRIBUTES	0	// unlocked, non purgable

#define MAXUMBS		10

typedef struct mmblockstruct
{
	unsigned	start,length;
	unsigned	attributes;
	memptr		*useptr;	// pointer to the segment start
	struct mmblockstruct far *next;
} mmblocktype;


//#define GETNEWBLOCK {if(!(mmnew=mmfree))Quit("MM_GETNEWBLOCK: No free blocks!");mmfree=mmfree->next;}
#define GETNEWBLOCK {if(!mmfree)MML_ClearBlock();mmnew=mmfree;mmfree=mmfree->next;}
#define FREEBLOCK(x) {*x->useptr=NULL;x->next=mmfree;mmfree=x;}

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

/*static mminfotype	mminfo;
static memptr		bufferseg;

static void		(* beforesort) (void);
static void		(* aftersort) (void);*/

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

/*static void huge	*hugeheap;
static void far	*farheap;
static void		*nearheap;

static mmblocktype	far mmblocks[MAXBLOCKS]
			,far *mmhead,far *mmfree,far *mmrover,far *mmnew;


static unsigned	totalEMSpages,freeEMSpages,EMSpageframe,EMSpagesmapped,EMShandle;
static unsigned int EMSVer;

static void		(* XMSaddr) (void);		// far pointer to XMS driver

static unsigned	numUMBs,UMBbase[MAXUMBS];*/

//==========================================================================

void MM_Startup (void);
void MM_Shutdown (void);
void MM_MapEMS (void);

void MM_GetPtr (memptr *baseptr,dword size);
void MM_FreePtr (memptr *baseptr);

void MM_SetPurge (memptr *baseptr, int purge);
void MM_SetLock (memptr *baseptr, boolean locked);
void MM_SortMem (void);

void MM_ShowMemory (void);

dword MM_UnusedMemory (void);
dword MM_TotalFree (void);
void MM_Report(void);
//int MM_EMSVer(void);

void MM_BombOnError (boolean bomb);

//==========================================================================

//
// local prototypes
//

boolean		MML_CheckForEMS (void);
void 		MML_ShutdownEMS (void);
void 		MM_MapEMS (void);
boolean 	MML_CheckForXMS (void);
void 		MML_ShutdownXMS (void);
void		MML_UseSpace (unsigned segstart, unsigned seglength);
void 		MML_ClearBlock (void);

//==========================================================================

#endif
