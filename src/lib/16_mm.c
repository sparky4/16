/* Catacomb Apocalypse Source Code
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
#pragma hdrstop

#pragma warn -pro
#pragma warn -use

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

void		(* beforesort) (void);
void		(* aftersort) (void);
void		(* XMSaddr) (void);		// far pointer to XMS driver

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/
#ifndef __16_PM__
static	char *ParmStringsexmm[] = {"noems","noxms",""};
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
	boolean emmcfems=0;
	static char	emmname[] = "EMMXXXX0";	//fix by andrius4669
	__asm {
		mov	dx,OFFSET emmname	//fix by andrius4669
		mov	ax,0x3d00
		int	0x21		// try to open EMMXXXX0 device
		jc	error

		mov	bx,ax
		mov	ax,0x4400

		int	0x21		// get device info
		jc	error

		and	dx,0x80
		jz	error

		mov	ax,0x4407

		int	0x21		// get status
		jc	error
		or	al,al
		jz	error

		mov	ah,0x3e
		int	0x21		// close handle
		jc	error
		//
		// EMS is good
		//
		mov     emmcfems,1
		jmp End
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
		mov     emmcfems,0
#ifdef __BORLANDC__
	}
#endif
		End:
#ifdef __WATCOMC__
	}
#endif
	return(emmcfems);
}

#ifndef __16_PM__
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

/*void MML_UseSpace(word segstart, dword seglength, global_game_variables_t *gvar)
{
	//huge mmblocktype huge *scan,huge *last;
	word		segm=1;
	word	oldend;
	dword		segmlen;
	dword		extra;

	scan = last = gvar->mm.mmhead;
	gvar->mm.mmrover = gvar->mm.mmhead;		// reset rover to start of memory

//
// search for the block that contains the range of segments
//
	while(scan->start+scan->length < segstart)
	{
		last = scan;
		scan = scan->next;
	}

//
// take the given range out of the block
//
	oldend = scan->start + scan->length;
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
	if(segstart == scan->start)
	{
		last->next = scan->next;			// unlink block
		FREEBLOCK(scan);
		scan = last;
	}
	else
		scan->length = segstart-scan->start;	// shorten block

	if(extra > 0)
	{
		GETNEWBLOCK;
		gvar->mm.mmnew->useptr = NULL;

		gvar->mm.mmnew->next = scan->next;
		scan->next = gvar->mm.mmnew;
		gvar->mm.mmnew->start = segstart+seglength;
		gvar->mm.mmnew->length = extra;
		gvar->mm.mmnew->attributes = LOCKBIT;
	}//else if(segm>0) goto segu;

}*/
void MML_UseSpace(word segstart, dword seglength, global_game_variables_t *gvar)
{
	mmblocktype far *scan,far *last;
	word	oldend;
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
	extra = oldend - (segstart+((unsigned)seglength));
	if (extra < 0)
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
	}

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

void MML_ClearBlock(global_game_variables_t *gvar)
{
	//huge mmblocktype huge *scan,huge *last;
	mmblocktype __far *scan;//,far *last;

	scan = gvar->mm.mmhead->next;

	while(scan)
	{
		if(!(scan->attributes&LOCKBIT) && (scan->attributes&PURGEBITS))
		{
			MM_FreePtr(scan->useptr, gvar);
			return;
		}
		scan = scan->next;
	}

	printf("MM_ClearBlock: No purgable blocks!\n");
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

void MM_Startup(global_game_variables_t *gvar)
{
	int i;
	//dword length,seglength;
	dword length; word seglength;
	//huge void huge	*start;
	void far	*start;
	word	segstart;//,endfree;

	if(gvar->mm.mmstarted)
		MM_Shutdown(gvar);

	gvar->mm.mmstarted = true;
	gvar->mm.bombonerror = true;

//
// set up the linked list (everything in the free list;
//
	gvar->mm.mmhead = NULL;
	gvar->mm.mmfree = &(gvar->mm.mmblocks[0]);
	for(i=0;i<MAXBLOCKS-1;i++)
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
#ifdef __WATCOMC__
	_nheapgrow();
	length=(dword)_memavl();//(dword)GetFreeSize();
	//huge start = (void huge *)(gvar->mm.nearheap = _nmalloc(length));
	start = (void far *)(gvar->mm.nearheap = _nmalloc(length));
#endif
#ifdef __BORLANDC__
	length=coreleft();
	//huge start = (void huge *)(gvar->mm.nearheap = malloc(length));
	start = (void far *)(gvar->mm.nearheap = malloc(length));
#endif
	length -= 16-(FP_OFF(start)&15);
	length -= SAVENEARHEAP;
	seglength = length / 16;			// now in paragraphs
	segstart = FP_SEG(start)+(FP_OFF(start)+15)/16;
	MML_UseSpace(segstart,seglength, gvar);
	gvar->mmi.nearheap = length;
	//printf("start=%Fp	segstart=%x	seglen=%lu	len=%lu\n", start, segstart, seglength, length);

//
// get all available far conventional memory segments
//
	//printf("_FARCORELEFT				%lu\n", _FCORELEFT);
#ifdef __WATCOMC__
	_fheapgrow();
	length=_FCORELEFT;//_fcoreleft();//(dword)GetFarFreeSize();//0xffffUL*4UL;
#endif
#ifdef __BORLANDC__
	printf("farcoreleft()				%lu\n", farcoreleft());
	printf("(farcoreleft()+32)-_FCORELEFT	%d\n", (sword)((farcoreleft()+32)-_FCORELEFT));
	length=farcoreleft();//_fcoreleft();//(dword)GetFarFreeSize();//0xffffUL*4UL;
#endif
	start = gvar->mm.farheap = _fmalloc(length);
	//start = gvar->mm.farheap = halloc(length, 1);
	length -= 16-(FP_OFF(start)&15);
	length -= SAVEFARHEAP;
	seglength = length / 16;			// now in paragraphs
	segstart = FP_SEG(start)+(FP_OFF(start)+15)/16;
	MML_UseSpace(segstart,seglength, gvar);
	gvar->mmi.farheap = length;
	//printf("start=%Fp	segstart=%x	seglen=%lu	len=%lu\n", start, segstart, seglength, length);

	gvar->mmi.mainmem = gvar->mmi.nearheap + gvar->mmi.farheap;

#ifndef __16_PM__
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
//
// allocate the misc buffer
//
	gvar->mm.mmrover = gvar->mm.mmhead;		// start looking for space after low block

	MM_GetPtr(&(gvar->mm.bufferseg),BUFFERSIZE, gvar);
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

void MM_Shutdown(global_game_variables_t *gvar)
{
	if(!(gvar->mm.mmstarted))
		return;

	_ffree(gvar->mm.farheap);//	printf("		far freed\n");
#ifdef __WATCOMC__
	_nfree(gvar->mm.nearheap);//	printf("		near freed\n");
#endif
#ifdef __BORLANDC__
	free(gvar->mm.nearheap);//	printf("		near freed\n");
#endif
#ifndef __16_PM__
#ifdef __DEBUG__
	if(!dbg_debugpm) {
#endif
	if(MML_CheckForEMS()){ MML_ShutdownEMS(gvar); }//printf("		EMS freed\n"); }
	if(MML_CheckForXMS()){ MML_ShutdownXMS(gvar); }//printf("		XMS freed\n"); }
#ifdef __DEBUG__
	}
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

void MM_GetPtr (memptr *baseptr, dword size, global_game_variables_t *gvar)
{
	//huge mmblocktype huge *scan,huge *lastscan,huge *endscan,huge *purge,huge *next;
	mmblocktype far *scan,far *lastscan,far *endscan,far *purge,far *next;
	int			search;
	unsigned	needed,startseg;

	needed = (size+15)/16;		// convert size from bytes to paragraphs

	GETNEWBLOCK;				// fill in start and next after a spot is found
	gvar->mm.mmnew->length = needed;
	gvar->mm.mmnew->useptr = baseptr;
	//if(gvar->mm.mmnew->useptr==NULL){
#ifdef __DEBUG_MM__
		printf("MM_GetPtr\n");
		//%04x
		printf("	baseptr=%Fp	", baseptr); printf("useptr=%Fp\n", gvar->mm.mmnew->useptr);
		printf("	*baseptr=%Fp	", *baseptr); printf("*useptr=%Fp\n", *(gvar->mm.mmnew->useptr));
		printf("	&baseptr=%Fp	", &baseptr); printf("&useptr=%Fp\n", &(gvar->mm.mmnew->useptr));
#endif
	//exit(-5); }
	gvar->mm.mmnew->attributes = BASEATTRIBUTES;

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
		printf("for stability reasons the program will shut down! wwww\n");
		MM_Shutdown(gvar);
		exit(-1);
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
= Allocates an unlocked, unpurgable block
=
====================
*/

void MM_FreePtr(memptr *baseptr, global_game_variables_t *gvar)
{
	//huge mmblocktype huge *scan,huge *last;
	mmblocktype far *scan,far *last;

	last = gvar->mm.mmhead;
	scan = last->next;

	if(baseptr == gvar->mm.mmrover->useptr)	// removed the last allocated block
		gvar->mm.mmrover = gvar->mm.mmhead;

	while(scan->useptr != baseptr && scan)
	{
		last = scan;
		scan = scan->next;
	}

	if(!scan)
	{
		printf("MM_FreePtr: Block not found!\n");
		return;
	}

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


void MM_SetPurge(memptr *baseptr, int purge, global_game_variables_t *gvar)
{
	//huge mmblocktype huge *start;
	mmblocktype far *start;

	start = gvar->mm.mmrover;

	do
	{
		if(gvar->mm.mmrover->useptr == baseptr)
			break;

		gvar->mm.mmrover = gvar->mm.mmrover->next;

		if(!gvar->mm.mmrover)
			gvar->mm.mmrover = gvar->mm.mmhead;
		else if(gvar->mm.mmrover == start)
		{
			Quit (gvar, "MM_SetPurge: Block not found!");
			//return;
		}

	} while(1);

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

void MM_SetLock(memptr *baseptr, boolean locked, global_game_variables_t *gvar)
{
	//huge mmblocktype huge *start;
	mmblocktype far *start;

	start = gvar->mm.mmrover;

	do
	{
		if(gvar->mm.mmrover->useptr == baseptr)
			break;

		gvar->mm.mmrover = gvar->mm.mmrover->next;

		if(!gvar->mm.mmrover)
			gvar->mm.mmrover = gvar->mm.mmhead;
		else if(gvar->mm.mmrover == start)
		{
			Quit (gvar, "MM_SetLock: Block not found!");
			//return;
		}

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

void MM_SortMem(global_game_variables_t *gvar)
{
	//huge mmblocktype huge *scan,huge *last,huge *next;
	mmblocktype far *scan,far *last,far *next;
	unsigned	start,length,source,dest;//++++,oldborder;
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
		MM_SetLock(&(memptr)audiosegs[playing],true);
	}


	SD_StopSound();*/
//	oldborder = bordercolor;
//	VW_ColorBorder (15);

	if(beforesort)
		beforesort();

	scan = gvar->mm.mmhead;

	last = NULL;		// shut up compiler warning

	while(scan)
	{
		if(scan->attributes & LOCKBIT)
		{
		//
		// block is locked, so try to pile later blocks right after it
		//
			start = scan->start + scan->length;
		}
		else
		{
			if(scan->attributes & PURGEBITS)
			{
			//
			// throw out the purgable block
			//
				next = scan->next;
				FREEBLOCK(scan);
				//MM_FreeBlock(scan, gvar);
				last->next = next;
				scan = next;
				continue;
			}
			else
			{
			//
			// push the non purgable block on top of the last moved block
			//
				if(scan->start != start)
				{
					length = scan->length;
					source = scan->start;
					dest = start;
					while(length > 0xf00)
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

	if(aftersort)
		aftersort();

//	VW_ColorBorder (oldborder);

/*++++	if(playing)
		MM_SetLock(&(memptr)audiosegs[playing],false);*/
}

//==========================================================================

/*
=====================
=
= MM_ShowMemory
=
=====================
*/

void MM_ShowMemory(global_game_variables_t *gvar)
{
	//huge mmblocktype huge *scan;
	mmblocktype far *scan;
	//word temp;
	sdword	end,owner;
	//word chx,chy;
	word w;
	//dword wwww;
	byte    scratch[160],scratch0[4096],str[16];//[[[[scratch1[160],
	//byte d = '#';
//[[[[	VW_SetDefaultColors();
//[[[[	VW_SetLineWidth(40);
//++++mh	temp = bufferofs;
//++++mh	bufferofs = 0;
//[[[[	VW_SetScreen (0,0);
	scan = gvar->mm.mmhead;
	end = -1;

	CA_OpenDebug (gvar);
	w=0;
	while(scan)
	{
		strcpy(scratch, AARESET);
		if(scan->attributes & PURGEBITS)
			strcpy(scratch0, AAMAGENTA);		// dark purple = purgable
		else
			strcpy(scratch0, AABLUE);		// medium blue = non purgable
		if(scan->attributes & LOCKBIT)
 			strcpy(scratch0, AARED);		// red = locked
		if(scan->start<=end)
		{
			printf("\nend==%d\n\n", end);
			strcat(scratch, "MM_ShowMemory: Memory block order currupted!\n");
			strcat(scratch, "End's Size: ");
			ultoa (end,str,10);
			strcat (scratch,str);
			strcat(scratch, "\nscan->start's Size: ");
			ultoa (scan->start,str,10);
			strcat (scratch,str);
			write(gvar->handle.debughandle,scratch,strlen(scratch));
			//modexprint(&page, chx, chy, 1, 0, 24, "\nMM_ShowMemory: Memory block order currupted!\n");
			break;
		}
		end = scan->start+(scan->length)-1;
//++++		chy = scan->start/320;
//++++		chx = scan->start%320;
				//modexhlin(page, scan->start, (unsigned)end, chy, color);
				//for(chx=scan->start;chx+4>=(word)end;chx+=4)
				//{
//++++					modexClearRegion(page, chx, chy, 4, 4, color);
				//}
//++++		VW_Hlin(scan->start,(unsigned)end,0,color);
		for(w=(scan->start)/80;w<=end/80;w++)
		{
			//printf("+	%u	%lu\n", w, scan->length);
			strcat(scratch0, "+");
		}
		//++==++==optional strcat(scratch0, AARESET); strcat(scratch0, AAGREY); strcat(scratch0,"_");
//++++		VW_Plot(scan->start,0,15);
//++++				modexClearRegion(page, chx, chy, 4, 4, 15);
//++++			VW_Hlin(end+1,scan->next->start,0,0);	// black = free

		//wwww=(dword)(scan->next->start)-(dword)scan->start;
		//wwww=(dword)scan->start+(dword)(scan->next->start);
		if (scan->next && scan->next->start >= end+1)
		{
			strcat(scratch0, AARESET);
			//++==++==optional strcat(scratch0, "\n");
			strcat(scratch0,AAGREEN);
			for(w=(end+1)/80;w<=((scan->next->start-scan->start)/80);w++)
			//for(w=(wwww)/80;w<=((end+1)/80);w++)
			//for(w=(end+1)/80;w<=((wwww)/80);w++)
			{
				//printf("0	%x	%u	%lu\n", scan->next->start, w, scan->length);
				strcat(scratch0,"0");
			}
			//printf("==================\n");
			//printf("w=%u	wwww=%lu	start=%04x	next=%04x	end=%lu\n", w/80, wwww/80, scan->start, (scan->next->start), end+1);
			//printf("==================\n");
			strcat(scratch0, "\n");
			//getch();
		}/*else {//if(scan->next->start <= scan->start){
			scan->next->start=scan->start+0x1000;
			wwww=(dword)(scan->next->start)-(dword)scan->start;
			strcat(scratch0, AARESET);
			strcat(scratch0, "\n");
			strcat(scratch0,AAGREEN);
			for(w=(end+1);w<=(0x1000/80);w++)
			{
				//printf("0	%x	%x	%u\n", scan->start, w);
				strcat(scratch0,"0");
			}
			printf("================\n");
			printf("w=%x	start=%x	next=%x	end=%u	%lu\n", w, scan->start, (scan->next->start), end+1, wwww);
			printf("================\n");
			getch();
		}*/
		strcat(scratch0, AARESET);
		//strcat(scratch0,"\n");
			//for(chx=scan->next->start;chx+4>=(word)end+1;chx+=4)
			//{
//				chx+=scan->next->start;
//				modexClearRegion(page, chx, chy, 4, 4, 2);
			//}
					//modexhlin(page, end+1,scan->next->start, chy, 0);
/*		y = scan->start/320;
		x = scan->start%320;
		VW_Hlin(x,x+end,y,color);
		VW_Plot(x,y,15);*/
//++++			VW_Hlin(x+end+1,x+(scan->next->start-scan->start),y,0);	// black = free
		strcat(scratch,"Seg:");
		ultoa (scan->start,str,16);
		strcat (scratch,str);
		strcat (scratch,"\tSize:");
		ultoa ((unsigned)scan->length,str,10);
		strcat (scratch,str);
		strcat (scratch,"\tOwner:0x");
		owner = (unsigned)scan->useptr;
		ultoa (owner,str,16);
		strcat (scratch,str);
		strcat (scratch,"\n");
		write(gvar->handle.debughandle,scratch,strlen(scratch));
		write(gvar->handle.debughandle,scratch0,strlen(scratch0));
//modexprint(page, chx, chy, 1, 0, 24, &scratch);
//++++chy+=4;
//fprintf(stdout, "%s", scratch);

		scan = scan->next;
	}
	/*strcpy(scratch1, AARESET);
	strcat(scratch1, "========================================\n");
	strcat(scratch1, "near=	 ");
	ultoa (*(gvar->mm.nearheap),str,10);
	strcat (scratch1,str);
	strcat(scratch1, "	far= ");
	ultoa (*(gvar->mm.farheap),str,10);
	strcat (scratch1,str);
	strcat(scratch1, "\n");
	//strcat(scratch1, "&near=	%Fp ", &(gvar->mm.nearheap));
	//strcat(scratch1, "&far=	%Fp", &(gvar->mm.farheap));
	//strcat(scratch1, "\n");
	strcat(scratch1, "========================================\n");
	write(gvar->handle.debughandle,scratch1,strlen(scratch1));*/


	CA_CloseDebug (gvar);

//++++mh	IN_Ack();
//****	VW_SetLineWidth(64);
//++++mh	bufferofs = temp;
}

//==========================================================================

/*
=====================
=
= MM_DumpData
=
=====================
*/

void MM_DumpData(global_game_variables_t *gvar)
{
	//huge mmblocktype huge *scan,huge *best;
	mmblocktype far *scan,far *best;
	long	lowest,oldlowest;
	word	owner;
	byte	lock,purge;
	FILE	*dumpfile;

	free(gvar->mm.nearheap);
#ifdef __BORLANDC__
		dumpfile = fopen ("mmdump.16b","w");
#endif
#ifdef __WATCOMC__
		dumpfile = fopen ("mmdump.16w","w");
#endif
	if (!dumpfile){
		printf("MM_DumpData: Couldn't open MMDUMP.16!\n");
		return;
	}

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

	fclose(dumpfile);
	printf("MMDUMP.16 created.\n");
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

dword MM_UnusedMemory(global_game_variables_t *gvar)
{
	dword free;
	//huge mmblocktype huge *scan;
	mmblocktype far *scan;

	free = 0;
	scan = gvar->mm.mmhead;

	while(scan->next)
	{
		free += scan->next->start - (scan->start + scan->length);
		scan = scan->next;
	}

	return free*16lu;
//	return free;
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

dword MM_TotalFree(global_game_variables_t *gvar)
{
	dword free;
	//huge mmblocktype huge *scan;
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
//	return free;
}

//==========================================================================

/*
=====================
=
= MM_Report
=
=====================
*/

void MM_Report_(global_game_variables_t *gvar)
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

void MM_BombOnError(boolean bomb, global_game_variables_t *gvar)
{
	gvar->mm.bombonerror = bomb;
}

/*void MM_GetNewBlock(global_game_variables_t *gvar)
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
}*/

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
