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

// NEWMM.C

/*
=============================================================================

			ID software memory manager
			--------------------------

Primary coder: John Carmack

RELIES ON
---------
Quit (global_game_variables_t *gvar, char *error) function


WORK TO DO
----------
MM_SizePtr to change the size of a given pointer

Multiple purge levels utilized

EMS / XMS unmanaged routines

=============================================================================
*/
/*

Open Watcom port by sparky4

*/
#include "src/lib/16_mm.h"
#include "src/lib/16_ca.h"
#include <malloc.h>
#pragma hdrstop

#pragma warn -pro
#pragma warn -use

/*
=============================================================================

							LOCAL INFO

=============================================================================
*/

#define LOCKBIT		0x80	// if set in attributes, block cannot be moved
#define PURGEBITS	3		// 0-3 level, 0= unpurgable, 3= purge first
#define PURGEMASK	0xfffc
#define BASEATTRIBUTES	0	// unlocked, non purgable

//#define MAXUMBS		10

/*typedef struct mmblockstruct
{
	unsigned	start,length;
	unsigned	attributes;
	memptr		*useptr;	// pointer to the segment start
	struct mmblockstruct far *next;
} mmblocktype;*/


//#define GETNEWBLOCK {if(!(mmnew=mmfree))Quit("MM_GETNEWBLOCK: No free blocks!") ;mmfree=mmfree->next;}
//

#define GETNEWBLOCK {if(!gvar->mm.mmfree)MML_ClearBlock(gvar);gvar->mm.mmnew=gvar->mm.mmfree;gvar->mm.mmfree=gvar->mm.mmfree->next;}

#define FREEBLOCK(x) {*x->useptr=NULL;x->next=gvar->mm.mmfree;gvar->mm.mmfree=x;}

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

/*mminfotype	mminfo;
memptr		bufferseg;
boolean		mmerror;*/

void		(* beforesort) (void);
void		(* aftersort) (void);

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

/*boolean		mmstarted;

void far	*farheap;
void		*nearheap;

mmblocktype	far mmblocks[MAXBLOCKS]
			,far *mmhead,far *mmfree,far *mmrover,far *mmnew;

boolean		bombonerror;*/

//unsigned	totalEMSpages,freeEMSpages,EMSpageframe,EMSpagesmapped,EMShandle;

void		(* XMSaddr) (void);		// far pointer to XMS driver

/*unsigned	numUMBs,UMBbase[MAXUMBS];*/

//==========================================================================

//
// local prototypes
//

boolean		MML_CheckForEMS (void);
void 		MML_ShutdownEMS (void);
void 		MM_MapEMS (void);
boolean 	MML_CheckForXMS (void);
void 		MML_ShutdownXMS (void);
//void		MML_UseSpace (unsigned segstart, unsigned seglength);
//void 		MML_ClearBlock (void);

//==========================================================================
#ifndef __16_PM__
#if 0
static	char *ParmStringsexmm[] = {"noems","noxms",""};
#endif
#endif
/*
======================
=
= MML_CheckForEMS
=
= Routine from p36 of Extending DOS
=
=======================
*/

boolean MML_CheckForEMS(void)
{
	boolean	emmcfems = false;
	word		EMSPageFrame = 0;
	byte	err=0, str[64];
	static char	emmname[] = "EMMXXXX0";	//fix by andrius4669
	__asm {
		mov	dx,OFFSET emmname	//fix by andrius4669
		mov	ax,0x3d00
		int	EMM_INT		// try to open EMMXXXX0 device
		jc	error

		mov	bx,ax
		mov	ax,0x4400

		int	EMM_INT		// get device info
		jc	error

		and	dx,0x80
		jz	error

		mov	ax,0x4407

		int	EMM_INT		// get status
		jc	error
		or	al,al
		jz	error

		mov	ah,0x3e
		int	EMM_INT		// close handle
		jc	error

		//
		// pageframe check
		//
		mov	ah,EMS_GETFRAME
		int	EMS_INT			// find the page frame address
		or	ah,ah
		jnz	error
		mov	[EMSPageFrame],bx

		//
		// EMS is good
		//
		mov	emmcfems,1
		jmp	End
#ifdef __BORLANDC__
	}
#endif
		error:
#ifdef __BORLANDC__
	__asm {
#endif
		//
		// EMS is bad
		//
		mov	err,ah
		mov	emmcfems,0
#ifdef __BORLANDC__
	}
#endif
		End:
#ifdef __WATCOMC__
	}
#endif

	//
	// Pageframe switch to determine if there is one!
	//
	if(!EMSPageFrame)
	{
		emmcfems = false;
#if defined(__DEBUG_PM__) || defined(__DEBUG_MM__)
		printf("MML_CheckForEMS: EMS error No Pageframe!\nAddress detected to be %04x\n", EMSPageFrame);
#endif
	}else 	if(!emmcfems)// if there is an error and page frame is not 0000
	{
		strcpy(str,"MML_CheckForEMS: EMS error ");
		MM_EMSerr(str, err);
		printf("%s\n",str);
	}

	return(emmcfems);
}

#ifndef __16_PM__
#if 0
/*
======================
=
= MML_SetupEMS
=
=======================
*/

byte MML_SetupEMS(global_game_variables_t *gvar)
{
	byte	str[160];
	byte	err;
	boolean errorflag=false;

	unsigned int EMSVer = 0;
	//byte	EMS_status;
	unsigned	totalEMSpages,freeEMSpages,EMSPageFrame,EMSpagesmapped,EMSHandle;
	totalEMSpages = freeEMSpages = EMSPageFrame = EMSpagesmapped = 0;

	__asm {
		mov	ah,EMS_STATUS
		int	EMS_INT						// make sure EMS hardware is present
		or	ah,ah
		//mov	[EMS_status],ah
		jnz	error

		mov	ah,EMS_VERSION
		int	EMS_INT
		or	ah,ah
		jnz	error
		mov	[EMSVer],ax				//	set EMSVer
		cmp	al,0x32						// only work on ems 3.2 or greater
		jb	error

		mov	ah,EMS_GETFRAME
		int	EMS_INT						// find the page frame address
		or	ah,ah
		jnz	error
		mov	[EMSPageFrame],bx

		mov	ah,EMS_GETPAGES
		int	EMS_INT						// find out how much EMS is there
		or	ah,ah
		jnz	error
		mov	[totalEMSpages],dx
		mov	[freeEMSpages],bx
		or	bx,bx
		jz	noEMS						// no EMS at all to allocate
											//EXPAND DONG!!!!
		cmp	[EMSVer],0x40
		jb	low
		cmp	bx,[freeEMSpages]
		jle	getpages
		mov	bx,[freeEMSpages]
		jmp	getpages
#ifdef __BORLANDC__
	}
#endif
	low:
#ifdef __BORLANDC__
	__asm {
#endif
		cmp	bx,4
		jle	getpages					// there is only 1,2,3,or 4 pages
		mov	bx,4						// we can't use more than 4 pages
#ifdef __BORLANDC__
	}
#endif
	getpages:
#ifdef __BORLANDC__
	__asm {
#endif
		mov	[EMSpagesmapped],bx
		mov	ah,EMS_ALLOCPAGES			// allocate up to 64k of EMS
		int	EMS_INT
		or	ah,ah
		jnz	error
		mov	[EMSHandle],dx
		jmp End
#ifdef __BORLANDC__
	}
#endif
	error:
#ifdef __BORLANDC__
	__asm {
#endif
		mov	err,ah
		mov	errorflag,1
		jmp End
#ifdef __BORLANDC__
	}
#endif
noEMS:
End:
#ifdef __WATCOMC__
	}
#endif
	if(errorflag==true)
	{
		strcpy(str,"MM_SetupEMS: EMS error ");
		MM_EMSerr(str, err);
		printf("%s\n",str);
		return err;
	}
	gvar->mm.totalEMSpages=totalEMSpages;
	gvar->mm.freeEMSpages=freeEMSpages;
	gvar->mm.EMSPageFrame=EMSPageFrame;
	gvar->mm.EMSpagesmapped=EMSpagesmapped;
	gvar->mm.EMSHandle=EMSHandle;
	gvar->mm.EMSVer=EMSVer;
	return 0;
}


/*
======================
=
= MML_ShutdownEMS
=
=======================
*/

void MML_ShutdownEMS(global_game_variables_t *gvar)
{
	boolean errorflag=false;
	unsigned EMSHandle=gvar->mm.EMSHandle;

	if(!EMSHandle)
		return;
	__asm {
		mov	ah,EMS_FREEPAGES
		mov	dx,[EMSHandle]
		int	EMS_INT
		or	ah,ah
		jz	ok
		mov	errorflag,1
#ifdef __BORLANDC__
	}
#endif
		ok:
#ifdef __WATCOMC__
	}
#endif
	if(errorflag==true)
		Quit (gvar, "MML_ShutdownEMS: Error freeing EMS!\n");	//++++ add something
}

/*
====================
=
= MM_MapEMS
=
= Maps the 64k of EMS used by memory manager into the page frame
= for general use.  This only needs to be called if you are keeping
= other things in EMS.
=
====================
*/

byte MM_MapEMS(global_game_variables_t *gvar)
{
	byte	str[160];
	unsigned	EMSHandle;
	byte err;
	boolean	errorflag=false;
	int	i;
	EMSHandle=gvar->mm.EMSHandle;

	for (i=0;i<4/*MAPPAGES*/;i++)
	{
		__asm {
			mov	ah,EMS_MAPPAGE
			mov	bx,[i]			// logical page
			mov	al,bl			// physical page
			mov	dx,[EMSHandle]	// handle
			int	EMS_INT
			or	ah,ah
			jnz	error
			jmp End
#ifdef __BORLANDC__
		}
#endif
			error:
#ifdef __BORLANDC__
		__asm {
#endif
			mov	err,ah
			mov	errorflag,1
#ifdef __BORLANDC__
		}
#endif
			End:
#ifdef __WATCOMC__
		}
#endif
		if(errorflag==true)
		{
			strcpy(str,"MM_MapEMS: EMS error ");
			MM_EMSerr(str, err);
			printf("%s\n",str);
			return err;
		}
	}
	gvar->mmi.EMSmem = (i)*0x4000lu;
	return 0;
}

byte MM_MapXEMS(global_game_variables_t *gvar)
{
//SUB EMS.MapXPages (PhysicalStart, LogicalStart, NumPages, Handle)

	//Maps up to 4 logical EMS pages to physical pages in the page frame, where:
	//PhysicalStart = Physical page first logical page is mapped to
	//LogicalStart  = First logical page to map
	//NumPages      = Number of pages to map (1 to 4)
	//Handle        = EMS handle logical pages are allocated to

  /*//Create a buffer containing the page information
//  FOR x = 0 TO NumPages - 1
//    MapInfo$ = MapInfo$ + MKI$(LogicalStart + x) + MKI$(PhysicalStart + x)
//  NEXT*/

//  Regs.ax = 0x5000                           //Map the pages in the buffer
//  Regs.cx = NumPages                         //to the pageframe
//  Regs.dx = Handle
//  Regs.ds = VARSEG(MapInfo$)
//  Regs.si = SADD(MapInfo$)
//  InterruptX 0x67, Regs, Regs
//	EMS.Error = (Regs.ax AND 0xFF00&) \ 0x100  //Store the status code

//END SUB
	byte	str[160];
	byte err;
	word	EMSHandle;
	boolean	errorflag=false;
	int	i;
	EMSHandle=gvar->mm.EMSHandle;

	if(gvar->mm.EMSVer<0x40)
		return 5;

	for (i=0;i<MAPPAGES;i++)
	{
		__asm {
			mov	ah,EMS_MAPXPAGE
			mov	cx,[i]			// logical page
			mov	al,bl			// physical page
			mov	dx,[EMSHandle]	// handle
			int	EMS_INT
			or	ah,ah
			jnz	error
			jmp End
#ifdef __BORLANDC__
		}
#endif
			error:
#ifdef __BORLANDC__
		__asm {
#endif
			mov	err,ah
			mov	errorflag,1
#ifdef __BORLANDC__
		}
#endif
			End:
#ifdef __WATCOMC__
		}
#endif
		if(errorflag==true)
		{
			//strcpy(str,"MM_MapXEMS: EMS error 0x");
			strcpy(str,"MM_MapXEMS: EMS error ");
			//itoa(err,str2,16);
			MM_EMSerr(str, err);
			printf("%s\n",str);
			//printf("%s%x\n",str, err);
			//printf("FACK! %x\n", err);
			return err;
		}
	}
	gvar->mmi.EMSmem = (i)*0x4000lu;
	return 0;
}
#endif
#endif
//==========================================================================

/*
======================
=
= MML_CheckForXMS
=
= Check for XMM driver
=
=======================
*/

boolean MML_CheckForXMS(void)
{
	//numUMBs = 0;
	boolean	errorflag=false;

	__asm {
		mov	ax,0x4300
		int	0x2f				// query status of installed diver
		cmp	al,0x80
		je	good
		mov	errorflag,1
#ifdef __BORLANDC__
	}
#endif
		good:
#ifdef __WATCOMC__
	}
#endif
	if(errorflag==true) return false;
	else return true;
}

#ifndef __16_PM__
#if 0
/*
======================
=
= MML_SetupXMS
=
= Try to allocate all upper memory block
=
=======================
*/

void MML_SetupXMS(global_game_variables_t *gvar)
{
	word	base,size;


	__asm {
		mov	ax,0x4310
		int	0x2f
		mov	[WORD PTR XMSaddr],bx
		mov	[WORD PTR XMSaddr+2],es		// function pointer to XMS driver
	}
getmemory:
	__asm {
		mov	ah,XMS_ALLOCUMB
		mov	dx,0xffff					// try for largest block possible
		//mov     ax,dx						// Set available Kbytes.
		call	[DWORD PTR XMSaddr]
		or	ax,ax
		jnz	gotone

		cmp	bl,0xb0						// error: smaller UMB is available
		jne	done;

		mov	ah,XMS_ALLOCUMB
		call	[DWORD PTR XMSaddr]		// DX holds largest available UMB
		or	ax,ax
		jz	done						// another error...
#ifdef __BORLANDC__
	}
#endif
		gotone:
#ifdef __BORLANDC__
	__asm {
#endif
		mov	[base],bx
		mov	[size],dx
#ifdef __BORLANDC__
	}
#endif
		done:
#ifdef __WATCOMC__
	}
#endif
//	printf("base=%u	", base); printf("size=%u\n", size);
	MML_UseSpace(base,size, gvar);
	gvar->mmi.XMSmem += size*16;
	gvar->mm.UMBbase[gvar->mm.numUMBs] = base;
	gvar->mm.numUMBs++;
	if(gvar->mm.numUMBs < MAXUMBS)
		goto getmemory;
}


/*
======================
=
= MML_ShutdownXMS
=
======================
*/

void MML_ShutdownXMS(global_game_variables_t *gvar)
{
	int	i;
	unsigned	base;

	for (i=0;i<gvar->mm.numUMBs;i++)
	{
		base = gvar->mm.UMBbase[i];
		__asm {
			mov	ah,XMS_FREEUMB
			mov	dx,[base]
			call	[DWORD PTR XMSaddr]
		}
	}
}
#endif
#endif
//==========================================================================

/*
======================
=
= MML_UseSpace
=
= Marks a range of paragraphs as usable by the memory manager
= This is used to mark space for the near heap, far heap, ems page frame,
= and upper memory blocks
=
======================
*/

/*
	extra = oldend - (segstart+seglength);

	segmlen=extra;

	//++++emsver stuff!
	if(segm>1)/// || extra>=0x10000lu)
	//if(extra>0xfffflu)
	{
		scan->blob=segm;

		//MML_UseSpace(segstart, seglength, gvar);

		printf("MML_UseSpace: Segment spans two blocks!\n");
	//}
	printf("segm=%u		", segm);
	printf("ex=%lu	", extra);
	printf("old=%u	", oldend);
	printf("start+seglen=%lu\n", segstart+seglength);
	printf("segsta=%x	", segstart);
	printf("len=%lu	", scan->length);
	printf("seglen=%lu	", seglength);
	printf("segmlen=%lu\n", segmlen);
	}
//++++todo: linked list of segment!
*/
void MML_UseSpace (unsigned segstart, unsigned seglength, global_game_variables_t *gvar)
{
	mmblocktype far *scan,far *last;
	unsigned	oldend;
	sdword		extra;
	//word segm=1;

	scan = last = gvar->mm.mmhead;
	gvar->mm.mmrover = gvar->mm.mmhead;		// reset rover to start of memory

//
// search for the block that contains the range of segments
//
	while (scan->start+scan->length < segstart)
	{
		last = scan;
		scan = scan->next;
	}

//
// find out how many blocks it spans!
//
	/*for(;seglength>=0x10000;seglength-=0xFFFF)
	{
		//printf("	seglen=%lu\n", segmlen);
		segm++;
	}*/

//
// take the given range out of the block
//
	oldend = scan->start + scan->length;
	extra = oldend - (segstart+seglength);
	if (extra < 0)
#ifdef __DEBUG_MM__
	{
		printf("========================================\n");
		printf("start=%x	", scan->start);
		printf("old=%u	", oldend);
		printf("start+seglen=%lu\n", segstart+seglength);
		printf("segsta=%x	", segstart);
		printf("len=%lu	", scan->length);
		printf("seglen=%lu	", seglength);
		printf("\n");
		printf("MML_UseSpace: Segment spans two blocks!	%d\n", extra);
		printf("========================================\n");
		//return;
	}
#else
		Quit (gvar, "MML_UseSpace: Segment spans two blocks!");
#endif

	if (segstart == scan->start)
	{
		last->next = scan->next;			// unlink block
		FREEBLOCK(scan);
		scan = last;
	}
	else
		scan->length = segstart-scan->start;	// shorten block

	if (extra > 0)
	{
		GETNEWBLOCK;
		gvar->mm.mmnew->useptr = NULL;

		gvar->mm.mmnew->next = scan->next;
		scan->next = gvar->mm.mmnew;
		gvar->mm.mmnew->start = segstart+seglength;
		gvar->mm.mmnew->length = extra;
		gvar->mm.mmnew->attributes = LOCKBIT;
	}//else if(segm>0) goto segu;

}

//==========================================================================

/*
====================
=
= MML_ClearBlock
=
= We are out of blocks, so free a purgable block
=
====================
*/

void MML_ClearBlock (global_game_variables_t *gvar)
{
	mmblocktype far *scan;//,far *last;

	scan = gvar->mm.mmhead->next;

	while(scan)
	{
		if(!(scan->attributes&LOCKBIT) && (scan->attributes&PURGEBITS) )
		{
			MM_FreePtr (scan->useptr, gvar);
			return;
		}
		scan = scan->next;
	}

	Quit (gvar, "MM_ClearBlock: No purgable blocks!\n");
}


//==========================================================================

/*
===================
=
= MM_Reset
=
===================
*/

void MM_Reset (global_game_variables_t *gvar)
{
//	//has to be 16
//	if(sizeof(mmblocktype)!=16)
//		return;

#ifdef __BORLANDC__
	strcpy(gvar->handle.datadumpfilename, "mmdump.16b");
#endif
#ifdef __WATCOMC__
	strcpy(gvar->handle.datadumpfilename, "mmdump.16w");
#endif

#ifdef __BORLANDC__
	strcpy(gvar->handle.heapdumpfilename, "heap.16b");
#endif
#ifdef __WATCOMC__
	strcpy(gvar->handle.heapdumpfilename, "heap.16w");
#endif
}


//==========================================================================

/*
===================
=
= MM_Startup
=
= Grabs all space from turbo with malloc/farmalloc
= Allocates bufferseg misc buffer
=
===================
*/

void MM_Startup (global_game_variables_t *gvar)
{
	int i;
	unsigned 	long length;
	void far	*start;
	unsigned	segstart,seglength;//,endfree;

	if(gvar->mm.mmstarted)
		MM_Shutdown (gvar);

	MM_Reset (gvar);
	gvar->mm.mmstarted = true;
	gvar->mm.bombonerror = true;
//
// set up the linked list (everything in the free list;
//
	gvar->mm.mmhead = NULL;
	gvar->mm.mmfree = &(gvar->mm.mmblocks[0]);
	for (i=0;i<MAXBLOCKS-1;i++)
		gvar->mm.mmblocks[i].next = &(gvar->mm.mmblocks[i+1]);
	gvar->mm.mmblocks[i].next = NULL;

//
// locked block of all memory until we punch out free space
//
	GETNEWBLOCK;
	gvar->mm.mmhead = gvar->mm.mmnew;				// this will allways be the first node
	gvar->mm.mmnew->start = 0;
	gvar->mm.mmnew->length = 0xffff;
	gvar->mm.mmnew->attributes = LOCKBIT;
	gvar->mm.mmnew->next = NULL;
	gvar->mm.mmrover = gvar->mm.mmhead;


//
// get all available near conventional memory segments
//
	length=(word)coreleft();
	start = (void far *)(gvar->mm.nearheap = _nmalloc(length));

	length -= 16-(FP_OFF(start)&15);
	length -= SAVENEARHEAP;
	seglength = length / 16;			// now in paragraphs
	segstart = FP_SEG(start)+(FP_OFF(start)+15)/16;
	MML_UseSpace (segstart,seglength, gvar);
	gvar->mmi.nearheap = length;
	//0000printf("near:	start=%Fp	segstart=%x	seglen=%lu	len=%lu\n", start, segstart, (dword)seglength, length);
//
// get all available far conventional memory segments
//
	length=farcoreleft();
	start = gvar->mm.farheap = farmalloc(length);
	length -= 16-(FP_OFF(start)&15);
	length -= SAVEFARHEAP;
	seglength = length / 16;			// now in paragraphs
	segstart = FP_SEG(start)+(FP_OFF(start)+15)/16;
	MML_UseSpace (segstart,seglength, gvar);
	gvar->mmi.farheap = length;
	gvar->mmi.mainmem = gvar->mmi.nearheap + gvar->mmi.farheap;
	//0000printf("far:	start=%Fp	segstart=%x	seglen=%lu	len=%lu\n", start, segstart, (dword)seglength, length);
#if !defined(__16_PM__)// && defined(__WATCOMC__)
#if 0
	if(!dbg_debugpm) {
//
// detect EMS and allocate up to 64K at page frame
//
	gvar->mmi.EMSmem = 0;
//goto emsskip;	//0000
	for(i = 1;i < _argc;i++)
	{
		if(US_CheckParm(_argv[i],ParmStringsexmm) == 0)
			goto emsskip;				// param NOEMS
	}
	if(MML_CheckForEMS())
	{
		MML_SetupEMS(gvar);					// allocate space
		//16_PM: EMS4! AND EMS 3.2 MASSIVE DATA HANDLMENT!
		MML_UseSpace(gvar->mm.EMSPageFrame,(MAPPAGES)*0x4000lu, gvar);
		//if(gvar->pm.emm.EMSVer<0x40)
			MM_MapEMS(gvar);					// map in used pages
		//else
			//MM_MapXEMS(gvar);					// map in used pages
	}

//
// detect XMS and get upper memory blocks
//
emsskip:
	gvar->mmi.XMSmem = 0;
goto xmsskip;//0000
	for(i = 1;i < _argc;i++)
	{
		if(US_CheckParm( _argv[i],ParmStringsexmm) == 0)
			goto xmsskip;				// param NOXMS
	}
	if(MML_CheckForXMS())
	{
		MML_SetupXMS(gvar);					// allocate as many UMBs as possible
	}

	}
xmsskip:
#endif
#endif
//
// allocate the misc buffer
//
	gvar->mm.mmrover = gvar->mm.mmhead;		// start looking for space after low block

	MM_GetPtr (&(gvar->mm.bufferseg),BUFFERSIZE, gvar);
}

//==========================================================================

/*
====================
=
= MM_Shutdown
=
= Frees all conventional, EMS, and XMS allocated
=
====================
*/

void MM_Shutdown (global_game_variables_t *gvar)
{
	if(!(gvar->mm.mmstarted))
		return;

	_ffree(gvar->mm.farheap);//	printf("		far freed\n");
	_nfree(gvar->mm.nearheap);//	printf("		near freed\n");
#ifndef __16_PM__
#if 0
#ifdef __DEBUG__
	if(!dbg_debugpm) {
#endif
	if(MML_CheckForEMS()){ MML_ShutdownEMS(gvar); }//printf("		EMS freed\n"); }
	if(MML_CheckForXMS()){ MML_ShutdownXMS(gvar); }//printf("		XMS freed\n"); }
#ifdef __DEBUG__
	}
#endif
#endif
#endif
}

//==========================================================================

/*
====================
=
= MM_GetPtr
=
= Allocates an unlocked, unpurgable block
=
====================
*/

void MM_GetPtr (memptr *baseptr,dword size, global_game_variables_t *gvar)
{
	mmblocktype far *scan,far *lastscan,far *endscan
				,far *purge,far *next;
	int			search;
	unsigned	needed,startseg;

	needed = (size+15)/16;		// convert size from bytes to paragraphs

	GETNEWBLOCK;				// fill in start and next after a spot is found
	gvar->mm.mmnew->length = needed;
	gvar->mm.mmnew->useptr = baseptr;
	gvar->mm.mmnew->attributes = BASEATTRIBUTES;
	//if(gvar->mm.mmnew->useptr==NULL){
#ifdef __DEBUG_MM__
	if(dbg_debugmm>0){
	printf("===============================================================================\n");
	printf("		MM_GetPtr\n");
	printf("===============================================================================\n");
		//%04x
//		printf("	baseptr=%Fp	", baseptr); printf("useptr=%Fp\n", gvar->mm.mmnew->useptr);
//		//printf("	*baseptr=%Fp	", *baseptr); printf("*useptr=%Fp\n", *(gvar->mm.mmnew->useptr));
//		printf("	&baseptr=%Fp	", &baseptr); printf("&useptr=%Fp\n", &(gvar->mm.mmnew->useptr));

		printf("	baseptr=%04x	", baseptr); printf("useptr=%04x\n", gvar->mm.mmnew->useptr);
		//printf("	*baseptr=%04x	", *baseptr); printf("*useptr=%04x\n", *(gvar->mm.mmnew->useptr));
		printf("	&baseptr=%04u	", &baseptr); printf("&useptr=%04u\n", &(gvar->mm.mmnew->useptr));

		printf("	size is %lu\n", size);
	}
#endif
	//Quit (gvar, "gvar->mm.mmnew->useptr==NULL"); }

//tryagain:
	for (search = 0; search<3; search++)
	{
	//
	// first search:	try to allocate right after the rover, then on up
	// second search: 	search from the head pointer up to the rover
	// third search:	compress memory, then scan from start
		if (search == 1 && gvar->mm.mmrover == gvar->mm.mmhead)
			search++;

		switch (search)
		{
		case 0:
			lastscan = gvar->mm.mmrover;
			scan = gvar->mm.mmrover->next;
			endscan = NULL;
			break;
		case 1:
			lastscan = gvar->mm.mmhead;
			scan = gvar->mm.mmhead->next;
			endscan = gvar->mm.mmrover;
			break;
		case 2:
			MM_SortMem (gvar);
			lastscan = gvar->mm.mmhead;
			scan = gvar->mm.mmhead->next;
			endscan = NULL;
			break;
		}

		startseg = lastscan->start + lastscan->length;

		while (scan != endscan)
		{
			if (scan->start - startseg >= needed)
			{
			//
			// got enough space between the end of lastscan and
			// the start of scan, so throw out anything in the middle
			// and allocate the new block
			//
				purge = lastscan->next;
				lastscan->next = gvar->mm.mmnew;
				gvar->mm.mmnew->start = *(unsigned *)baseptr = startseg;
				gvar->mm.mmnew->next = scan;
				while ( purge != scan)
				{	// free the purgable block
					next = purge->next;
					FREEBLOCK(purge);
					purge = next;		// purge another if not at scan
				}
				gvar->mm.mmrover = gvar->mm.mmnew;
				return;	// good allocation!
			}

			//
			// if this block is purge level zero or locked, skip past it
			//
			if ( (scan->attributes & LOCKBIT)
				|| !(scan->attributes & PURGEBITS) )
			{
				lastscan = scan;
				startseg = lastscan->start + lastscan->length;
			}


			scan=scan->next;		// look at next line
		}
	}

	if (gvar->mm.bombonerror)
	{
#ifdef __WATCOMC__
		//heapdump();
#endif
		printf(OUT_OF_MEM_MSG,(size-gvar->mmi.nearheap));
		Quit (gvar, "for stability reasons the program will shut down! wwww\n");
	}
	else
		gvar->mm.mmerror = true;
}

//==========================================================================

/*
====================
=
= MM_FreePtr
=
= Deallocates an unlocked, purgable block
=
====================
*/

void MM_FreePtr (memptr *baseptr, global_game_variables_t *gvar)
{
	mmblocktype far *scan,far *last;

	last = gvar->mm.mmhead;
	scan = last->next;

	if (baseptr == gvar->mm.mmrover->useptr)	// removed the last allocated block
		gvar->mm.mmrover = gvar->mm.mmhead;

	while (scan->useptr != baseptr && scan)
	{
		last = scan;
		scan = scan->next;
	}

	if (!scan)
		Quit (gvar, "MM_FreePtr: Block not found!");

	last->next = scan->next;

	FREEBLOCK(scan);
}
//==========================================================================

/*
=====================
=
= MM_SetPurge
=
= Sets the purge level for a block (locked blocks cannot be made purgable)
=
=====================
*/

void MM_SetPurge (memptr *baseptr, int purge, global_game_variables_t *gvar)
{
	mmblocktype far *start;

	start = gvar->mm.mmrover;

	do
	{
		if (gvar->mm.mmrover->useptr == baseptr)
			break;

		gvar->mm.mmrover = gvar->mm.mmrover->next;

		if (!gvar->mm.mmrover)
			gvar->mm.mmrover = gvar->mm.mmhead;
		else if (gvar->mm.mmrover == start)
			Quit (gvar, "MM_SetPurge: Block not found!");

	} while (1);

	gvar->mm.mmrover->attributes &= ~PURGEBITS;
	gvar->mm.mmrover->attributes |= purge;
}

//==========================================================================

/*
=====================
=
= MM_SetLock
=
= Locks / unlocks the block
=
=====================
*/

void MM_SetLock (memptr *baseptr, boolean locked, global_game_variables_t *gvar)
{
	mmblocktype far *start;

	start = gvar->mm.mmrover;

	do
	{
		if (gvar->mm.mmrover->useptr == baseptr)
			break;

		gvar->mm.mmrover = gvar->mm.mmrover->next;

		if (!gvar->mm.mmrover)
			gvar->mm.mmrover = gvar->mm.mmhead;
		else if (gvar->mm.mmrover == start)
			Quit (gvar, "MM_SetLock: Block not found!");

	} while(1);

	gvar->mm.mmrover->attributes &= ~LOCKBIT;
	gvar->mm.mmrover->attributes |= locked*LOCKBIT;
}

//==========================================================================

/*
=====================
=
= MM_SortMem
=
= Throws out all purgable stuff and compresses movable blocks
=
=====================
*/

void MM_SortMem (global_game_variables_t *gvar)
{
	mmblocktype far *scan,far *last,far *next;
	unsigned	start,length,source,dest,oldborder;
	//++++int			playing;

	//
	// lock down a currently playing sound
	//
/*++++	playing = SD_SoundPlaying ();
	if(playing)
	{
		switch (SoundMode)
		{
		case sdm_PC:
			playing += STARTPCSOUNDS;
			break;
		case sdm_AdLib:
			playing += STARTADLIBSOUNDS;
			break;
		}
		MM_SetLock(MEMPTRCONV audiosegs[playing],true);
	}


	SD_StopSound();*/

	oldborder = gvar->video.bordercolor;
	gvar->video.bordercolor = VL_modexPalOverscan(gvar->video.palette, 4);

	if (beforesort)
		beforesort();

	scan = gvar->mm.mmhead;

	last = NULL;		// shut up compiler warning

	while (scan)
	{
		if (scan->attributes & LOCKBIT)
		{
		//
		// block is locked, so try to pile later blocks right after it
		//
			start = scan->start + scan->length;
		}
		else
		{
			if (scan->attributes & PURGEBITS)
			{
			//
			// throw out the purgable block
			//
				next = scan->next;
				FREEBLOCK(scan); //MM_FreeBlock(scan, gvar);
				last->next = next;
				scan = next;
				continue;
			}
			else
			{
			//
			// push the non purgable block on top of the last moved block
			//
				if (scan->start != start)
				{
					length = scan->length;
					source = scan->start;
					dest = start;
					while (length > 0xf00)
					{
						movedata(source,0,dest,0,0xf00*16);
						length -= 0xf00;
						source += 0xf00;
						dest += 0xf00;
					}
					movedata(source,0,dest,0,length*16);

					scan->start = start;
					*(unsigned *)scan->useptr = start;
				}
				start = scan->start + scan->length;
			}
		}

		last = scan;
		scan = scan->next;		// go to next block
	}

	gvar->mm.mmrover = gvar->mm.mmhead;

	if (aftersort)
		aftersort();

	VL_ColorBorder (oldborder, &gvar->video);

/*++++	if(playing)
		MM_SetLock((memptr *)&audiosegs[playing],false);*/
}

//==========================================================================

#ifdef __BORLANDC__
extern char global_temp_status_text[512];
extern char global_temp_status_text2[512];
#endif
//#ifdef __WATCOMC__
//#define MMSMPANVID
#define MMSMSCANINFO
//#endif

/*
=====================
=
= MM_ShowMemory
=
=====================
*/

void MM_ShowMemory (global_game_variables_t *gvar)
{
	mmblocktype far *scan;
	unsigned color,temp,x,y		,q,w/*,width*/,sw;
	sdword	end,owner;
	byte		scratch[160],scratch0[4096],str[16];
#ifdef MMSMSCANINFO
	mmshowmemoryinfo_t scaninfo[MAXBLOCKS];
	byte scratch1[4];
	unsigned		/*xpos,ypos, */oldq,maxq;
	boolean		done,restarted,mmsmscaninfoxyposinew;
	ScanCode		scancode;
#endif

	if(!gvar->video.page[0].width) gvar->video.page[0].sw = gvar->video.page[0].width = 320;	//to prevent division by 0
//--	VL_SetLineWidth(40, gvar);
	//temp = gvar->video.ofs.bufferofs;
	//gvar->video.ofs.bufferofs = gvar->video.ofs.displayofs;
	temp = BDOFSCONV gvar->video.BOFS;
	gvar->video.BOFS = gvar->video.DOFS;
#ifdef MMSMSCANINFO
	oldq = 0; restarted = false; mmsmscaninfoxyposinew = false;
reset:
#endif
	scan = gvar->mm.mmhead;

	end = -1; w = 0; q = 0;

	//width = gvar->video.page[0].width;
	sw = gvar->video.page[0].sw;

	CA_OpenDebug (gvar);
	while (scan)
	{
#ifdef MMSMSCANINFO
		scaninfo[q].scan = scan;
#endif
		strcpy(scratch, AARESET);
		if(scan->attributes & PURGEBITS)
		{
			color = 6;		// dark purple = purgable
			strcpy(scratch0, AAMAGENTA);		// dark purple = purgable
		}else{
			color = 2;		// medium blue = non purgable
			strcpy(scratch0, AABLUE);		// medium blue = non purgable
		}
		if(scan->attributes & LOCKBIT)
		{
			color = 1;		// red = locked
 			strcpy(scratch0, AARED);		// red = locked
		}
		if (scan->start<=end)
		{
			printf("\nend==%d\n\n", end);
			strcat(scratch, "MM_ShowMemory: Memory block order currupted!\n");
			strcat(scratch, "End's Size: ");	ultoa (end,str,10);	strcat (scratch,str);
			strcat(scratch, "\nscan->start's Size: ");	ultoa (scan->start,str,10);	strcat (scratch,str);
			write(gvar->handle.debughandle,scratch,strlen(scratch));
			Quit (gvar, "MM_ShowMemory: Memory block order currupted!");
		}
		end = scan->length-1;
		y = gvar->video.page[0].dx+(scan->start/sw);
		x = gvar->video.page[0].dy+(scan->start%sw);
#if 0
//def MMSMSCANINFO
		if(restarted){
			y += gvar->video.page[0].dy;
			x += gvar->video.page[0].dx;
		}
#if 0
		else{
			scaninfo[q].y = y;
			scaninfo[q].x = x;
		}
#endif
#endif
		VW_Hlin(x,x+end,y,color,gvar);
		VL_Plot(x,y,5,gvar);
//++==++==optional			strcat(scratch0, AARESET); strcat(scratch0, AAGREY); strcat(scratch0,"_");
		for(w=(scan->start)/80;w<=end/80;w++)
		{
//printf("+	%u	%lu\n", w, scan->length);
			strcat(scratch0, "+");
		}

		if (scan->next && scan->next->start > end+1)
		{
			VW_Hlin(x+end+1,x+(scan->next->start-scan->start),y,3,gvar);	// black = free//now green
			strcat(scratch0, AARESET);
//++==++==optional			strcat(scratch0, "\n");
			strcat(scratch0,AAGREEN);
			for(w=(end+1)/80;w<=((scan->next->start-scan->start)/80);w++)
			{
//printf("0	%x	%u	%lu\n", scan->next->start, w, scan->length);
				strcat(scratch0,"0");
			}
			//printf("==================\n");
			//printf("w=%u	start=%04x	next=%04x	end=%lu\n", w/80, scan->start, (scan->next->start), end+1);
			//printf("==================\n");
			strcat(scratch0, "\n");
		}
#if 0
		else {//if(scan->next->start <= scan->start){
			scan->next->start=scan->start+0x1000;
			strcat(scratch0, AARESET);
			strcat(scratch0, "\n");
			strcat(scratch0,AAGREEN);
			for(w=(end+1);w<=(0x1000/80);w++)
			{
				//printf("0	%x	%x	%u\n", scan->start, w);
				strcat(scratch0,"0");
			}
			printf("================\n");
			printf("w=%x	start=%x	next=%x	end=%u\n", w, scan->start, (scan->next->start), end+1);
			printf("================\n");
			getch();
		}
#endif
		strcat(scratch0, AARESET);
		//strcat(scratch0,"\n");
		strcat(scratch,"Block:");
		ultoa (q,str,10);
		strcat (scratch,str);
		strcat(scratch,"\tSeg:");
		ultoa (scan->start,str,16);
		strcat (scratch,str);
//		strcat(scratch, AABLACK); strcat(scratch,"\t"); strcat(scratch, AARESET);
		strcat (scratch,"\tSize:");
		ultoa ((unsigned)scan->length,str,10);
		strcat (scratch,str);
//		strcat(scratch, AABLACK); strcat(scratch,"\t"); strcat(scratch, AARESET);
		strcat (scratch,"\tOwner:0x");
		owner = (unsigned)scan->useptr;
		ultoa (owner,str,16);
		strcat (scratch,str);
		strcat (scratch,"\n");
		write(gvar->handle.debughandle,scratch,strlen(scratch));
		write(gvar->handle.debughandle,scratch0,strlen(scratch0));
//0000fprintf(stdout, "\n[%s]", scratch);
//0000fprintf(stdout, "[\n%s\n]", scratch0);
//0000fprintf(stdout, "[%u]\n", q);

		scan = scan->next;
		q++;

//0000if(gvar->video.VL_Started && color!=6) IN_Ack(gvar);
	}

	CA_CloseDebug (gvar);

#ifdef MMSMPANVID
	{
		int dx,dy,odx,ody;
		odx = gvar->video.page[0].dx;
		ody = gvar->video.page[0].dy;
		dx = dy = 0;

		while (!(scancode = gvar->in.inst->LastScan)){}
		while(!gvar->in.inst->Keyboard[sc_Escape] && !gvar->in.inst->Keyboard[sc_Space])
		{
			if(gvar->in.inst->Keyboard[sc_UpArrow])
			{
				if(dy>0)
					dy--;
			}
			else if(gvar->in.inst->Keyboard[sc_DownArrow])
			{
				if(dy<gvar->video.page[0].height-gvar->video.page[0].sh)
					dy++;
			}
			if(gvar->in.inst->Keyboard[sc_LeftArrow])
			{
				if(dx>0)
					dx--;
			}
			else if(gvar->in.inst->Keyboard[sc_RightArrow])
			{
				if(dx<gvar->video.page[0].width-gvar->video.page[0].sw)
					dx++;
			}

			modexPanPage(&gvar->video.page[0], dx, dy);
			VL_ShowPage(&gvar->video.page[0], 1, 1);
		}

		gvar->video.page[0].dx = odx;
		gvar->video.page[0].dy = ody;
	}
#endif
#ifdef MMSMSCANINFO
	if (gvar->video.VL_Started) IN_Ack(gvar);
	maxq = q-1;
	if(restarted) q = oldq;
	else q = 0;
	restarted = false;

	while (!(scancode = gvar->in.inst->LastScan)){}
//	IN_ClearKey(scancode);
//	IN_Ack(gvar);

	for (done = false;!done;)
	{
		if(scaninfo[q].scan->attributes & PURGEBITS)
		{
			color = 6;		// dark purple = purgable
			strcpy(scratch1, AAMAGENTA);		// dark purple = purgable
			//printf("%s", AAMAGENTA);
		}else{
			color = 2;		// medium blue = non purgable
			strcpy(scratch1, AABLUE);		// medium blue = non purgable
			//printf("%s", AABLUE);
		}
		if(scaninfo[q].scan->attributes & LOCKBIT)
		{
			color = 1;		// red = locked
			strcpy(scratch1, AARED);		// red = locked
			//printf("%s", AARED);
		}
		end = scaninfo[q].scan->length-1;

		gvar->video.print.t=1;
		gvar->video.print.tlsw=1;
		gvar->video.print.color=color;
		gvar->video.print.bgcolor=8;
#define MMSMPRINTMEMINFO	VL_print(global_temp_status_text, 0, gvar); gvar->video.print.y+=8;
//modexprint(&(gvar->video.page[0]), xpos, ypos, 1, 1, color, 8, gvar->video.VL_Started, global_temp_status_text);
#ifdef __WATCOMC__
		if(gvar->video.VL_Started)
		{
			VL_ShowPage(&gvar->video.page[0], 1, 0);
			modexClearRegion(&gvar->video.page[0], gvar->video.page[0].dx, gvar->video.page[0].dy, gvar->video.page[0].sw, gvar->video.page[0].sh, 8);
		}else
#endif
		{
			clrscr();
		}
		sprintf(global_temp_status_text, "block #%04u", q); MMSMPRINTMEMINFO
//		sprintf(global_temp_status_text, "%Fp", scaninfo[q].scan->useptr); MMSMPRINTMEMINFO
		sprintf(global_temp_status_text, "start:  %04x", (unsigned)scaninfo[q].scan->start); MMSMPRINTMEMINFO
		sprintf(global_temp_status_text, "useptr: %04x", (unsigned)scaninfo[q].scan->useptr); MMSMPRINTMEMINFO
		sprintf(global_temp_status_text, "size: %05u", (unsigned)scaninfo[q].scan->length); MMSMPRINTMEMINFO
		if (scaninfo[q].scan->next && scaninfo[q].scan->next->start > end+1)
		{
			sprintf(global_temp_status_text, "free: %05u", (unsigned)(scaninfo[q].scan->next->start-scaninfo[q].scan->start)); MMSMPRINTMEMINFO
		}else	/*ypos*/gvar->video.print.y+=8;
		if(gvar->video.VL_Started)
		{
			y = scaninfo[q].scan->start/sw;
			x = scaninfo[q].scan->start%sw;
			if(!mmsmscaninfoxyposinew)
			{
				y = gvar->video.print.y;//ypos;
				x = gvar->video.print.x;//xpos;
			}else{
				//y = scaninfo[q].y;
				//x = scaninfo[q].x;
				y += gvar->video.page[0].dy;
				x += gvar->video.page[0].dx;
			}
			VW_Hlin(x,x+end,y,color,gvar);
			VL_Plot(x,y,5,gvar);
		}
		else
		{
			printf("%s", scratch1);
			printf("%s", AAGREY); printf("_");
			for(w=(scaninfo[q].scan->start)/80;w<=end/80;w++)
			{
				//strcat(scratch1, "+");
				printf("+");
			}
		}


		if (scaninfo[q].scan->next && scaninfo[q].scan->next->start > end+1) if(!gvar->video.VL_Started)
		{
			//strcat(scratch1, AARESET);
			printf("%s", AARESET);
			//strcat(scratch1,AAGREEN);
			printf("%s", AAGREEN);
			for(w=(end+1)/80;w<=((scaninfo[q].scan->next->start-scaninfo[q].scan->start)/80);w++)
			{
				//strcat(scratch1,"0");
				printf("0");
			}
		}else VW_Hlin(x+end+1,x+(scaninfo[q].scan->next->start-scaninfo[q].scan->start),y,3,gvar);	// black = free//now green


		if(gvar->video.VL_Started)
		{
			//if (scan->next && scan->next->start > end+1) free
			/*xpos*/gvar->video.print.x = gvar->video.page[0].dx;
			/*ypos*/gvar->video.print.y = gvar->video.page[0].dy;
		}
		else
		{
			//printf("%s\n", scratch1);
			printf("%s", AARESET);
			printf("\n");
		}


		while (!(scancode = gvar->in.inst->LastScan)){}

		IN_ClearKey(scancode);
		switch (scancode)
		{
			case sc_Enter:
				if(!mmsmscaninfoxyposinew) mmsmscaninfoxyposinew = true;
				else mmsmscaninfoxyposinew = false;
				//mmsmscaninfoxyposinew!=mmsmscaninfoxyposinew;
			break;
			case sc_LeftArrow:
				if(q>0) q--;
				else	q = maxq;
			break;
			case sc_RightArrow:
				if(q<maxq) q++;
				else q = 0;
			break;
			case sc_UpArrow:
				if(q>9) q-=10;
				else	q = maxq;
			break;
			case sc_DownArrow:
				if(q<maxq-9) q+=10;
				else q = 0;
			break;
			case sc_Space:
				oldq = q;
				restarted = true;
#ifdef __WATCOMC__
				if(gvar->video.VL_Started) modexClearRegion(&gvar->video.page[0], gvar->video.page[0].dx, gvar->video.page[0].dy, gvar->video.page[0].sw, gvar->video.page[0].sh, 8);
#endif
				goto reset;

			case sc_Escape:
				done = true;
			break;
		}
	}
#else
	if(gvar->video.VL_Started) IN_Ack(gvar);
#endif

	gvar->video.BOFS = (byte __far *)temp;
}

//==========================================================================


/*
=====================
=
= MM_DumpData
=
=====================
*/

void MM_DumpData (global_game_variables_t *gvar)
{
	mmblocktype far *scan,far *best;
	long	lowest,oldlowest;
	unsigned	owner;
	char	lock,purge;
	FILE	*dumpfile;

	_nfree(gvar->mm.nearheap);
	dumpfile = fopen (gvar->handle.datadumpfilename, "w");
	if (!dumpfile)
		Quit (gvar, "MM_DumpData: Couldn't open MMDUMP.16!\n");

	lowest = -1;
	do
	{
		oldlowest = lowest;
		lowest = 0xffff;

		scan = gvar->mm.mmhead;
		while (scan)
		{
			owner = (unsigned)scan->useptr;

			if (owner && owner<lowest && owner > oldlowest)
			{
				best = scan;
				lowest = owner;
			}

			scan = scan->next;
		}

		if (lowest != 0xffff)
		{
			if (best->attributes & PURGEBITS)
				purge = 'P';
			else
				purge = '-';
			if (best->attributes & LOCKBIT)
				lock = 'L';
			else
				lock = '-';
			fprintf (dumpfile,"0x%p (%c%c) = %u\n"
			,(unsigned)lowest,lock,purge,best->length);
		}

	} while (lowest != 0xffff);

	fclose (dumpfile);

	//reset filename
#ifdef __BORLANDC__
	strcpy(gvar->handle.datadumpfilename, "mmdump.16b");
#endif
#ifdef __WATCOMC__
	strcpy(gvar->handle.datadumpfilename, "mmdump.16w");
#endif
//00	printf ("MMDUMP.16 created.\n");
}

//==========================================================================


/*
======================
=
= MM_UnusedMemory
=
= Returns the total free space without purging
=
======================
*/

dword MM_UnusedMemory (global_game_variables_t *gvar)
{
	unsigned free;
	mmblocktype far *scan;

	free = 0;
	scan = gvar->mm.mmhead;

	while(scan->next)
	{
		free += scan->next->start - (scan->start + scan->length);
		scan = scan->next;
	}

	return free*16lu;
}

//==========================================================================


/*
======================
=
= MM_TotalFree
=
= Returns the total free space with purging
=
======================
*/

dword MM_TotalFree (global_game_variables_t *gvar)
{
	unsigned free;
	mmblocktype far *scan;

	free = 0;
	scan = gvar->mm.mmhead;

	while(scan->next)
	{
		if((scan->attributes&PURGEBITS) && !(scan->attributes&LOCKBIT))
			free += scan->length;
		free += scan->next->start - (scan->start + scan->length);
		scan = scan->next;
	}

	return free*16lu;
}

//==========================================================================

/*
=====================
=
= MM_Report
=
=====================
*/

void MM_Report_ (global_game_variables_t *gvar)
{
	printf("========================================\n");
	printf("		MM_Report_\n");
	printf("========================================\n");
	if(MML_CheckForEMS())
	{
		printf("	%cLIMEMS	%u\n", 0xC9, gvar->pm.emm.EMSPresent);
		printf("	%c%cEMM v%x.%x available\n", 0xC7, 0xC4, gvar->pm.emm.EMSVer>>4,gvar->pm.emm.EMSVer&0x0F);
		printf("	%c%ctotalEMSpages:	%u	", 0xC7, 0xC4, gvar->pm.emm.totalEMSpages); printf("freeEMSpages:	%u\n", gvar->pm.emm.freeEMSpages);
		printf("	%c%cEMSPageFrame:	%04x\n", 0xC7, 0xC4, gvar->pm.emm.EMSPageFrame);
		printf("	%c%cEMSmem:	%lu\n", 0xD3, 0xC4, gvar->mmi.EMSmem);
	}
	if(MML_CheckForXMS())
	{
		printf("	%cXMS	%u\n", 0xC9, gvar->pm.xmm.XMSPresent);
		printf("	%c%cXMS v%x.%x available\n", 0xC7, 0xC4, XMSVer>>8,XMSVer&0x0F);
		printf("	%c%cXMSDriver:	%Fp\n", 0xC7, 0xC4, XMSDriver);
		printf("	%c%cXMSHandle:	%04x\n", 0xC7, 0xC4, gvar->pm.xmm.XMSHandle);
		printf("	%c%cXMSmem:	%lu\n", 0xD3, 0xC4, gvar->mmi.XMSmem);
	}
	printf("	%cConv.	%u\n", 0xC9, gvar->pm.mm.MainPresent); DebugMemory_(gvar, 0);
	//printf("mainmem:	%lu\n", gvar->mmi.mainmem);
	//printf("Total convmem:	%lu	", gvar->mmi.mainmem); printf("TotalFree:	%lu	", MM_TotalFree(gvar)+gvar->mmi.EMSmem+gvar->mmi.XMSmem+gvar->mmi.XMSmem); printf("TotalUsed:	%lu\n", gvar->mmi.mainmem);
	//printf("			UnusedMemory:	%lu\n", MM_UnusedMemory(gvar));
	printf("nearheap:	%lu		", gvar->mmi.nearheap); printf("farheap:	%lu\n", gvar->mmi.farheap);
}

//==========================================================================

/*
=====================
=
= MM_EMSerr
=
=====================
*/

void MM_EMSerr(byte *stri, byte err)
{
	//Returns a text string describing the error code in EMS.Error.
	switch(err)
	{
		case 0x0:
			strcat(stri, "successful");
		break;
		case 0x80:
			strcat(stri, "internal error");
		break;
		case 0x81:
			strcat(stri, "hardware malfunction");
		break;
		case 0x82:
			strcat(stri, "busy .. retry later");
		break;
		case 0x83:
			strcat(stri, "invalid handle");
		break;
		case 0x84:
			strcat(stri, "undefined function requested by application");
		break;
		case 0x85:
			strcat(stri, "no more handles available");
		break;
		case 0x86:
			strcat(stri, "error in save or restore of mapping context");
		break;
		case 0x87:
			strcat(stri, "insufficient memory pages in system");
		break;
		case 0x88:
			strcat(stri, "insufficient memory pages available");
		break;
		case 0x89:
			strcat(stri, "zero pages requested");
		break;
		case 0x8A:
			strcat(stri, "invalid logical page number encountered");
		break;
		case 0x8B:
			strcat(stri, "invalid physical page number encountered");
		break;
		case 0x8C:
			strcat(stri, "page-mapping hardware state save area is full");
		break;
		case 0x8D:
			strcat(stri, "save of mapping context failed");
		break;
		case 0x8E:
			strcat(stri, "restore of mapping context failed");
		break;
		case 0x8F:
			strcat(stri, "undefined subfunction");
		break;
		case 0x90:
			strcat(stri, "undefined attribute type");
		break;
		case 0x91:
			strcat(stri, "feature not supported");
		break;
		case 0x92:
			strcat(stri, "successful, but a portion of the source region has been overwritten");
		break;
		case 0x93:
			strcat(stri, "length of source or destination region exceeds length of region allocated to either source or destination handle");
		break;
		case 0x94:
			strcat(stri, "conventional and expanded memory regions overlap");
		break;
		case 0x95:
			strcat(stri, "offset within logical page exceeds size of logical page");
		break;
		case 0x96:
			strcat(stri, "region length exceeds 1 MB");
		break;
		case 0x97:
			strcat(stri, "source and destination EMS regions have same handle and overlap");
		break;
		case 0x98:
			strcat(stri, "memory source or destination type undefined");
		break;
		case 0x9A:
			strcat(stri, "specified alternate map register or DMA register set not supported");
		break;
		case 0x9B:
			strcat(stri, "all alternate map register or DMA register sets currently allocated");
		break;
		case 0x9C:
			strcat(stri, "alternate map register or DMA register sets not supported");
		break;
		case 0x9D:
			strcat(stri, "undefined or unallocated alternate map register or DMA register set");
		break;
		case 0x9E:
			strcat(stri, "dedicated DMA channels not supported");
		break;
		case 0x9F:
			strcat(stri, "specified dedicated DMA channel not supported");
		break;
		case 0xA0:
			strcat(stri, "no such handle name");
		break;
		case 0xA1:
			strcat(stri, "a handle found had no name, or duplicate handle name");
		break;
		case 0xA2:
			strcat(stri, "attempted to wrap around 1M conventional address space");
		break;
		case 0xA3:
			strcat(stri, "source array corrupted");
		break;
		case 0xA4:
			strcat(stri, "operating system denied access");
		break;
		default:
			strcat(stri, "undefined error");
	}
}

//==========================================================================

/*
=====================
=
= MM_BombOnError
=
=====================
*/

void MM_BombOnError (boolean bomb, global_game_variables_t *gvar)
{
	gvar->mm.bombonerror = bomb;
}

#if 0
void MM_GetNewBlock(global_game_variables_t *gvar)
{
	if(!gvar->mm.mmfree)
		MML_ClearBlock(gvar);
	gvar->mm.mmnew=gvar->mm.mmfree;
	gvar->mm.mmfree=gvar->mm.mmfree->next;
	if(!(gvar->mm.mmnew=gvar->mm.mmfree))
	{
		printf("MM_GETNEWBLOCK: No free blocks!\n");
		return;
	}
	gvar->mm.mmfree=gvar->mm.mmfree->next;
}

void MM_FreeBlock(mmblocktype *x, global_game_variables_t *gvar)
{
	x->useptr=NULL;
	x->next=gvar->mm.mmfree;
	gvar->mm.mmfree=x;
}
#endif

void xms_call(byte v, global_game_variables_t *gvar)
{
	dword XMSDriver = gvar->pm.xmm.XMSDriver;
	__asm {
		mov	ah,[v]
		call [DWORD PTR XMSDriver]
	}
}

/*void MM_seguin(void)
{
	__asm {
		push	ds
		mov	ax,ds
		inc		ax
		mov	ds,ax
	}
}

void MM_segude(void)
{
	__asm {
		pop ds
	}
}*/

/*
pull data from far and put it into ds var
mov ax,es:si
mov x,ax
*/
/*
ss stack segment
sp top of stack
bp bottem of stack
*/
