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
Quit (char *error) function


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

static	char *ParmStringsexmm[] = {"noems","noxms",""};

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
	boolean emmcfems;
	static char	emmname[] = "EMMXXXX0";	//fix by andrius4669
//		mov	dx,OFFSET emmname
	__asm {
		//LEA	DX, emmname	//fix by andrius4669
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
		error:
		//
		// EMS is bad
		//
		mov     emmcfems,0
		End:
	}
	return(emmcfems);
}


/*
======================
=
= MML_SetupEMS
=
=======================
*/

byte MML_SetupEMS(mminfo_t *mm)
{
	char	str[80];//,str2[10];
	byte	err;
	boolean errorflag=false;
	union REGS CPURegs;

	unsigned int EMSVer = 0;
	//byte	EMS_status;
	unsigned	totalEMSpages,freeEMSpages,EMSpageframe,EMSpagesmapped,EMShandle;
	totalEMSpages = freeEMSpages = EMSpageframe = EMSpagesmapped = 0;

	__asm
		{
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
		mov	[EMSpageframe],bx

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

low:
		cmp	bx,4
		jle	getpages					// there is only 1,2,3,or 4 pages
		mov	bx,4						// we can't use more than 4 pages

getpages:
		mov	[EMSpagesmapped],bx
		mov	ah,EMS_ALLOCPAGES			// allocate up to 64k of EMS
		int	EMS_INT
		or	ah,ah
		jnz	error
		mov	[EMShandle],dx
		jmp End
error:
		mov	err,ah
		mov	errorflag,1
		jmp End
noEMS:
End:
	}
	if(errorflag==true)
	{
		//err = CPURegs.h.ah;
		strcpy(str,"MML_SetupEMS: EMS error 0x");
		//itoa(err,str2,16);
		//strcat(str,&err);
		//printf("%s\n",str);
		printf("%s%x\n",str,err);
		return err;
	}
	mm->totalEMSpages=totalEMSpages;
	mm->freeEMSpages=freeEMSpages;
	mm->EMSpageframe=EMSpageframe;
	mm->EMSpagesmapped=EMSpagesmapped;
	mm->EMShandle=EMShandle;
	mm->EMSVer=EMSVer;
	return 0;
}


/*
======================
=
= MML_ShutdownEMS
=
=======================
*/

void MML_ShutdownEMS(mminfo_t *mm)
{
	boolean errorflag=false;
	unsigned EMShandle=mm->EMShandle;

	if(!EMShandle)
		return;
	__asm
	{
		mov	ah,EMS_FREEPAGES
		mov	dx,[EMShandle]
		int	EMS_INT
		or	ah,ah
		jz	ok
		mov	errorflag,1
		ok:
	}
	if(errorflag==true) printf("MML_ShutdownEMS: Error freeing EMS!");	//++++ add something
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

byte MM_MapEMS(mminfo_t *mm)
{
	char	str[80];//,str2[10];
	unsigned	/*err, */EMShandle;
	byte err;
	boolean	errorflag=false;
	int	i;
	union REGS CPURegs;
	EMShandle=mm->EMShandle;

	for (i=0;i<MAPPAGES;i++)
	{
		__asm
		{
			mov	ah,EMS_MAPPAGE
			mov	bx,[i]			// logical page
			mov	al,bl			// physical page
			mov	dx,[EMShandle]	// handle
			int	EMS_INT
			or	ah,ah
			jnz	error
			jmp End
			error:
			mov	err,ah
			mov	errorflag,1
			End:
		}
		if(errorflag==true)
		{
			//err = CPURegs.h.ah;
			strcpy(str,"MM_MapEMS: EMS error 0x");
			//itoa(err,str2,16);
			//strcat(str,&err);
			//printf("%s\n",str);
			printf("%s%x\n",str, err);
			//printf("FACK! %x\n", err);
			return err;
		}
	}
	return 0;
}

void MM_MapXEMS(mminfo_t *mm)
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

//  Regs.ax = &H5000                           //Map the pages in the buffer
//  Regs.cx = NumPages                         //to the pageframe
//  Regs.dx = Handle
//  Regs.ds = VARSEG(MapInfo$)
//  Regs.si = SADD(MapInfo$)
//  InterruptX &H67, Regs, Regs
//	EMS.Error = (Regs.ax AND &HFF00&) \ &H100  //Store the status code

//END SUB
}

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

boolean MML_CheckForXMS(mminfo_t *mm)
{
	boolean	errorflag=false;
	mm->numUMBs = 0;

	__asm
	{
		mov	ax,0x4300
		int	0x2f				// query status of installed diver
		cmp	al,0x80
		je	good
		mov	errorflag,1
		good:
	}
	if(errorflag==true) return false;
	else return true;
}


/*
======================
=
= MML_SetupXMS
=
= Try to allocate all upper memory block
=
=======================
*/

void MML_SetupXMS(mminfo_t *mm, mminfotype *mmi)
{
	unsigned	base,size;

getmemory:
	__asm
	{
		mov	ax,0x4310
		int	0x2f
		mov	[WORD PTR XMSaddr],bx
		mov	[WORD PTR XMSaddr+2],es		// function pointer to XMS driver

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

gotone:
		mov	[base],bx
		mov	[size],dx
done:
	}
	printf("base=%u	", base); printf("size=%u\n", size);
	MML_UseSpace(base,size, mm);
	mmi->XMSmem += size*16;
	mm->UMBbase[mm->numUMBs] = base;
	mm->numUMBs++;
	if(mm->numUMBs < MAXUMBS)
		goto getmemory;
}


/*
======================
=
= MML_ShutdownXMS
=
======================
*/

void MML_ShutdownXMS(mminfo_t *mm)
{
	int	i;
	unsigned	base;

	for (i=0;i<mm->numUMBs;i++)
	{
		base = mm->UMBbase[i];
		__asm
		{
			mov	ah,XMS_FREEUMB
			mov	dx,[base]
			call	[DWORD PTR XMSaddr]
		}
	}
}

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

void MML_UseSpace(/*d*/word segstart, dword seglength, mminfo_t *mm)
{
	mmblocktype huge *scan,huge *last;
	word		segm;
	dword	oldend;
	dword		extra;

	scan = last = mm->mmhead;
	mm->mmrover = mm->mmhead;		// reset rover to start of memory

//
// search for the block that contains the range of segments
//
	while(scan->start+scan->length < segstart)
	{
		last = scan;
		scan = scan->next;
	}

	//find out how many blocks it spans!
	if(seglength>0xffffu)
	{
//		segm=seglength/0x4000u;
		segm=seglength/0xffffu;
	}
	else segm=1;

	//++++emsver stuff!
	if(segm>1/*extra>0xfffflu*/)
	{
		/*__asm
		{
			push	ds
			mov	ax,ds
			inc		ax
			mov	ds,ax
		}*/


//MML_UseSpace(?segstart?, ?length?, mm);

		/*__asm
		{
			pop ds
		}*/
		//printf("MML_UseSpace: Segment spans two blocks!\n");
	}

//
// take the given range out of the block
//
	oldend = scan->start + scan->length;
	extra = oldend - (segstart+seglength);

printf("segm=%u	", segm);
printf("ex=%lu	", extra);
printf("start+seglen=%lu	", segstart+seglength);
printf("len=%u	", scan->length);
printf("segsta=%x	", segstart);
printf("seglen=%lu\n", seglength);

//segu:
//++++todo: linked list of segment!
//printf("segm=%lu\n", segm);
	if(segstart == scan->start)
	{
		last->next = scan->next;			// unlink block
		MM_FreeBlock(scan, mm);
		scan = last;
	}
	else
		scan->length = segstart-scan->start;	// shorten block

//	segm--;

	if(extra > 0)
	{
		MM_GetNewBlock(mm);
		mm->mmnew->next = scan->next;
		scan->next = mm->mmnew;
		mm->mmnew->start = segstart+seglength;
		mm->mmnew->length = extra;
		mm->mmnew->attributes = LOCKBIT;
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

void MML_ClearBlock(mminfo_t *mm)
{
	mmblocktype huge *scan,huge *last;

	scan = mm->mmhead->next;

	while(scan)
	{
		if(!(scan->attributes&LOCKBIT) && (scan->attributes&PURGEBITS))
		{
			MM_FreePtr(scan->useptr, mm);
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

void MM_Startup(mminfo_t *mm, mminfotype *mmi)
{
	int i;
	dword length;
	void huge	*start;
	unsigned	segstart,seglength,endfree;

	if(mm->mmstarted)
		MM_Shutdown(mm);

	mm->mmstarted = true;
	mm->bombonerror = true;
//
// set up the linked list (everything in the free list;
//
	mm->mmhead = NULL;
	mm->mmfree = &(mm->mmblocks[0]);
	for(i=0;i<MAXBLOCKS-1;i++)
	{
		mm->mmblocks[i].next = &(mm->mmblocks[i+1]);
	}
	mm->mmblocks[i].next = NULL;

//
// locked block of all memory until we punch out free space
//
	MM_GetNewBlock(mm);
	mm->mmhead = mm->mmnew;				// this will allways be the first node
	mm->mmnew->start = 0;
	mm->mmnew->length = 0xffff;
	mm->mmnew->attributes = LOCKBIT;
	mm->mmnew->next = NULL;
	mm->mmrover = mm->mmhead;

//	farlen=_bios_memsize()*1024;

//
// get all available near conventional memory segments
//
//----	length=coreleft();
	_nheapgrow();
	length=_memavl();
	start = (void huge *)(mm->nearheap = malloc(length));
	length -= 16-(FP_OFF(start)&15);
	length -= SAVENEARHEAP;
	seglength = length / 16;			// now in paragraphs
	segstart = FP_SEG(start)+(FP_OFF(start)+15)/16;
	MML_UseSpace(segstart,seglength, mm);
	mmi->nearheap = length;
	//printf("near heap ok!\n");

//
// get all available far conventional memory segments
//
//----	length=farcoreleft();
	_fheapgrow();
	length=_memavl();
	start = mm->farheap = halloc(length, sizeof(byte));
	//start = mm->farheap = _fmalloc(length);
	length -= 16-(FP_OFF(start)&15);
	length -= SAVEFARHEAP;
	seglength = length / 16;			// now in paragraphs
	segstart = FP_SEG(start)+(FP_OFF(start)+15)/16;
	MML_UseSpace(segstart,seglength, mm);
	mmi->farheap = length;
	mmi->mainmem = mmi->nearheap + mmi->farheap;
	//printf("far heap ok!\n");


//
// detect EMS and allocate up to 64K at page frame
//
	printf("EMS!\n\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");	//bug!
	mmi->EMSmem = 0;
	for(i = 1;i < __argc;i++)
	{
		if(US_CheckParm(__argv[i],ParmStringsexmm) == 0)
			goto emsskip;				// param NOEMS
	}
	printf("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");	//bug!
	if(MML_CheckForEMS())
	{
printf("EMS1\n");
		MML_SetupEMS(mm);					// allocate space
printf("EMS2\n");
		printf("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");	//bug!
		//TODO: EMS4! AND EMS 3.2 MASSIVE DATA HANDLMENT!
		MML_UseSpace(mm->EMSpageframe,(MAPPAGES)*0x4000lu, mm);
printf("EMS3\n");
		MM_MapEMS(mm);					// map in used pages
printf("EMS4\n");
		mmi->EMSmem = (MAPPAGES)*0x4000lu;
	}

//
// detect XMS and get upper memory blocks
//
emsskip:
	mmi->XMSmem = 0;
	for(i = 1;i < __argc;i++)
	{
		if(US_CheckParm(__argv[i],ParmStringsexmm) == 0)
			goto xmsskip;				// param NOXMS
	}
	printf("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");	//bug!
	if(MML_CheckForXMS(mm))
	{
		printf("XMS!\n");
		//MML_SetupXMS(mm, mmi);					// allocate as many UMBs as possible
	}

//
// allocate the misc buffer
//
xmsskip:
	mm->mmrover = mm->mmhead;		// start looking for space after low block

	MM_GetPtr(&(mm->bufferseg),BUFFERSIZE, mm, mmi);
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

void MM_Shutdown(mminfo_t *mm)
{
	if(!(mm->mmstarted))
		return;

	_ffree(mm->farheap);
	printf("far freed\n");
	free(mm->nearheap);
	printf("near freed\n");
	//hfree(mm->hugeheap);
	//printf("huge freed\n");
	if(MML_CheckForEMS()){ MML_ShutdownEMS(mm); printf("EMS freed\n"); }
	if(MML_CheckForXMS(mm)){ MML_ShutdownXMS(mm); printf("XMS freed\n"); }
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

void MM_GetPtr(memptr *baseptr,dword size, mminfo_t *mm, mminfotype *mmi)
{
	mmblocktype huge *scan,huge *lastscan,huge *endscan,huge *purge,huge *next;
	int			search;
	unsigned	needed,startseg;

	needed = (size+15)/16;		// convert size from bytes to paragraphs

	MM_GetNewBlock(mm);				// fill in start and next after a spot is found
	mm->mmnew->length = needed;
	mm->mmnew->useptr = baseptr;
	mm->mmnew->attributes = BASEATTRIBUTES;

	for(search = 0; search<3; search++)
	{
	//
	// first search:	try to allocate right after the rover, then on up
	// second search: 	search from the head pointer up to the rover
	// third search:	compress memory, then scan from start
		if(search == 1 && mm->mmrover == mm->mmhead)
			search++;

		switch(search)
		{
		case 0:
			lastscan = mm->mmrover;
			scan = mm->mmrover->next;
			endscan = NULL;
			break;
		case 1:
			lastscan = mm->mmhead;
			scan = mm->mmhead->next;
			endscan = mm->mmrover;
			break;
		case 2:
			MM_SortMem(mm);
			lastscan = mm->mmhead;
			scan = mm->mmhead->next;
			endscan = NULL;
			break;
		}

		startseg = lastscan->start + lastscan->length;

		while(scan != endscan)
		{
			if(scan->start - startseg >= needed)
			{
			//
			// got enough space between the end of lastscan and
			// the start of scan, so throw out anything in the middle
			// and allocate the new block
			//
				purge = lastscan->next;
				lastscan->next = mm->mmnew;
				mm->mmnew->start = *(unsigned *)baseptr = startseg;
				mm->mmnew->next = scan;
				while(purge != scan)
				{	// free the purgable block
					next = purge->next;
					MM_FreeBlock(purge, mm);
					purge = next;		// purge another if not at scan
				}
				mm->mmrover = mm->mmnew;
				return;	// good allocation!
			}

			//
			// if this block is purge level zero or locked, skip past it
			//
			if((scan->attributes & LOCKBIT)
				|| !(scan->attributes & PURGEBITS) )
			{
				lastscan = scan;
				startseg = lastscan->start + lastscan->length;
			}


			scan=scan->next;		// look at next line
		}
	}

	if (mm->bombonerror)
		printf(OUT_OF_MEM_MSG,(size-mmi->nearheap));
	else
		mm->mmerror = true;
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

void MM_FreePtr(memptr *baseptr, mminfo_t *mm)
{
	mmblocktype huge *scan,huge *last;

	last = mm->mmhead;
	scan = last->next;

	if(baseptr == mm->mmrover->useptr)	// removed the last allocated block
		mm->mmrover = mm->mmhead;

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

	MM_FreeBlock(scan, mm);
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

void MM_SetPurge(memptr *baseptr, int purge, mminfo_t *mm)
{
	mmblocktype huge *start;

	start = mm->mmrover;

	do
	{
		if(mm->mmrover->useptr == baseptr)
			break;

		mm->mmrover = mm->mmrover->next;

		if(!mm->mmrover)
			mm->mmrover = mm->mmhead;
		else if(mm->mmrover == start)
		{
			printf("MM_SetPurge: Block not found!");
			return;
		}

	} while(1);

	mm->mmrover->attributes &= ~PURGEBITS;
	mm->mmrover->attributes |= purge;
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

void MM_SetLock(memptr *baseptr, boolean locked, mminfo_t *mm)
{
	mmblocktype huge *start;

	start = mm->mmrover;

	do
	{
		if(mm->mmrover->useptr == baseptr)
			break;

		mm->mmrover = mm->mmrover->next;

		if(!mm->mmrover)
			mm->mmrover = mm->mmhead;
		else if(mm->mmrover == start)
		{
			printf("MM_SetLock: Block not found!");
			return;
		}

	} while(1);

	mm->mmrover->attributes &= ~LOCKBIT;
	mm->mmrover->attributes |= locked*LOCKBIT;
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

void MM_SortMem(mminfo_t *mm)
{
	mmblocktype huge *scan,huge *last,huge *next;
	unsigned	start,length,source,dest,oldborder;
	int			playing;

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

	scan = mm->mmhead;

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
				MM_FreeBlock(scan, mm);
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

	mm->mmrover = mm->mmhead;

	if(aftersort)
		aftersort();

//	VW_ColorBorder (oldborder);

/*++++	if(playing)
		MM_SetLock(&(memptr)audiosegs[playing],false);*/
}


//==========================================================================

//****#if 0
/*
=====================
=
= MM_ShowMemory
=
=====================
*/

void MM_ShowMemory(page_t *page, mminfo_t *mm)
{
	mmblocktype huge *scan;
	word color,temp;
	long	end,owner;
	word chx,chy;
	byte    scratch[160],str[16];

//****	VW_SetDefaultColors();
//****	VW_SetLineWidth(40);
//++++mh	temp = bufferofs;
//++++mh	bufferofs = 0;
//****	VW_SetScreen (0,0);

	scan = mm->mmhead;

	end = -1;

CA_OpenDebug ();

	chx=0;
	chy=0;

	while(scan)
	{
		if(scan->attributes & PURGEBITS)
			color = 5;		// dark purple = purgable
		else
			color = 9;		// medium blue = non purgable
		if(scan->attributes & LOCKBIT)
			color = 12;		// red = locked
		if(scan->start<=end)
		{
			//printf(");
			write(debughandle,"\nMM_ShowMemory: Memory block order currupted!\n",strlen("\nMM_ShowMemory: Memory block order currupted!\n"));
			//modexprint(&page, chx, chy, 1, 0, 24, "\nMM_ShowMemory: Memory block order currupted!\n");
			return;
		}
		end = scan->start+scan->length-1;
		chy = scan->start/320;
		chx = scan->start%320;
				//modexhlin(page, scan->start, (unsigned)end, chy, color);
				//for(chx=scan->start;chx+4>=(word)end;chx+=4)
				//{
					modexClearRegion(page, chx, chy, 4, 4, color);
				//}

//++++		VW_Hlin(scan->start,(unsigned)end,0,color);

//++++		VW_Plot(scan->start,0,15);
				modexClearRegion(page, chx, chy, 4, 4, 15);
		if(scan->next->start > end+1)
//++++			VW_Hlin(end+1,scan->next->start,0,0);	// black = free
			//for(chx=scan->next->start;chx+4>=(word)end+1;chx+=4)
			//{
				chx+=scan->next->start;
				modexClearRegion(page, chx, chy, 4, 4, 2);
			//}
					//modexhlin(page, end+1,scan->next->start, chy, 0);

/*
		end = scan->length-1;
		y = scan->start/320;
		x = scan->start%320;
		VW_Hlin(x,x+end,y,color);
		VW_Plot(x,y,15);
		if (scan->next && scan->next->start > end+1)
			VW_Hlin(x+end+1,x+(scan->next->start-scan->start),y,0);	// black = free
*/

//****#if 0
printf("\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0");	//bug!
strcpy(scratch,"Seg:");
ultoa (scan->start,str,16);
strcat (scratch,str);
strcat (scratch,"\tSize:");
ltoa ((dword)scan->length*16,str,10);
strcat (scratch,str);
strcat (scratch,"\tOwner:0x");
owner = (unsigned)scan->useptr;
ultoa (owner,str,16);
strcat (scratch,str);
strcat (scratch,"\n");
write(debughandle,scratch,strlen(scratch));
//modexprint(page, chx, chy, 1, 0, 24, &scratch);
chy+=4;
//fprintf(stdout, "%s", scratch);
//****#endif

		scan = scan->next;
	}

CA_CloseDebug ();

//++++mh	IN_Ack();
//****	VW_SetLineWidth(64);
//++++mh	bufferofs = temp;
}
//****#endif

//==========================================================================

/*
=====================
=
= MM_DumpData
=
=====================
*/

void MM_DumpData(mminfo_t *mm)
{
	mmblocktype far *scan,far *best;
	long	lowest,oldlowest;
	word	owner;
	byte	lock,purge;
	FILE	*dumpfile;


	free (mm->nearheap);
	dumpfile = fopen ("mmdump.txt","w");
	if (!dumpfile){
		printf("MM_DumpData: Couldn't open MMDUMP.TXT!\n");
		return;
	}

	lowest = -1;
	do
	{
		oldlowest = lowest;
		lowest = 0xffff;

		scan = mm->mmhead;
		while (scan)
		{
			owner = (word)scan->useptr;

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
			,(word)lowest,lock,purge,best->length);
		}

	} while (lowest != 0xffff);

	fclose (dumpfile);
	printf("MMDUMP.TXT created.\n");
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

dword MM_UnusedMemory(mminfo_t *mm)
{
	dword free;
	mmblocktype huge *scan;

	free = 0;
	scan = mm->mmhead;

	while(scan->next)
	{
		free += scan->next->start - (scan->start + scan->length);
		scan = scan->next;
	}

	return free*16l;
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

dword MM_TotalFree(mminfo_t *mm)
{
	dword free;
	mmblocktype huge *scan;

	free = 0;
	scan = mm->mmhead;

	while(scan->next)
	{
		if((scan->attributes&PURGEBITS) && !(scan->attributes&LOCKBIT))
			free += scan->length;
		free += scan->next->start - (scan->start + scan->length);
		scan = scan->next;
	}

	return free*16l;
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

void MM_Report(page_t *page, mminfo_t *mm, mminfotype *mmi)
{
	if(MML_CheckForEMS())
	{
		printf("EMM v%x.%x available\n", mm->EMSVer>>4,mm->EMSVer&0x0F);
		printf("totalEMSpages=%u\n", mm->totalEMSpages);
		printf("freeEMSpages=%u\n", mm->freeEMSpages);
		printf("EMSpageframe=%x\n", mm->EMSpageframe);
	}
	if(MML_CheckForXMS(mm)) printf("XMSaddr=%Fp\n", *XMSaddr);
	printf("near=%lu\n", mmi->nearheap);
	printf("far=%lu\n", mmi->farheap);
	printf("EMSmem=%lu\n", mmi->EMSmem);
	printf("XMSmem=%lu\n", mmi->XMSmem);
	printf("mainmem=%lu\n", mmi->mainmem);
	printf("UnusedMemory=%lu\n", MM_UnusedMemory(mm));
	printf("TotalFree=%lu\n", MM_TotalFree(mm));
//	printf("\n");
//	printf("UnusedMemory=%lu kb\n", MM_UnusedMemory()/10248);
//	printf("TotalFree=%lu kb\n", MM_TotalFree()/10248);
}

//==========================================================================

/*
=====================
=
= MM_EMSVer
=
=====================


int MM_EMSVer(void)
{
	int EMSver;
	__asm
	{
		mov		ah,EMS_VERSION
		int		EMS_INT
		mov		EMSver,ax
	}
	return(EMSver);
}*/

//==========================================================================

/*
=====================
=
= MM_BombOnError
=
=====================
*/

void MM_BombOnError(boolean bomb, mminfo_t *mm)
{
	mm->bombonerror = bomb;
}

void MM_GetNewBlock(mminfo_t *mm)
{
	if(!mm->mmfree)
		MML_ClearBlock(mm);
	mm->mmnew=mm->mmfree;
	mm->mmfree=mm->mmfree->next;
	/*if(!(mm->mmnew=mm->mmfree))
	{
		printf("MM_GETNEWBLOCK: No free blocks!");
		return;
	}
	mm->mmfree=mm->mmfree->next;*/
}

void MM_FreeBlock(mmblocktype *x, mminfo_t *mm)
{
	x->useptr=NULL;
	x->next=mm->mmfree;
	mm->mmfree=x;
}

void MM_seguin(void)
{
	__asm
	{
		push	ds
		mov	ax,ds
		inc		ax
		mov	ds,ax
	}
}

void MM_segude(void)
{
	__asm
	{
		pop ds
	}
}

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
