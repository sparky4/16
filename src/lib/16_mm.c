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

#include "src/lib/16_mm.h"

/*
======================
=
= MML_CheckForEMS
=
= Routine from p36 of Extending DOS
=
=======================
*/

boolean MML_CheckForEMS (void)
{
	boolean emmcfems;
	char	emmname[] = "EMMXXXX0";
//		mov	dx,OFFSET emmname
	__asm {
		LEA	DX, emmname	//fix by andrius4669
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

void MML_SetupEMS (void)
{
	char	str[80],str2[10];
	unsigned	err;
	boolean errorflag=false;
	union REGS CPURegs;

	EMSVer = 0;
	totalEMSpages = freeEMSpages = EMSpageframe = EMSpagesmapped = 0;

	__asm
		{
		mov	ah,EMS_STATUS
		int	EMS_INT						// make sure EMS hardware is present
		or	ah,ah
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
		mov	errorflag,1
		jmp End
noEMS:
End:
	}
	if(errorflag==true)
	{
		err = CPURegs.h.ah;
		strcpy(str,"MML_SetupEMS: EMS error 0x");
		itoa(err,str2,16);
		strcpy(str,str2);
		printf("%s\n",str);
	}
}


/*
======================
=
= MML_ShutdownEMS
=
=======================
*/

void MML_ShutdownEMS (void)
{
	boolean errorflag=false;
	if (!EMShandle)
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

void MM_MapEMS (void)
{
	char	str[80],str2[10];
	unsigned	err;
	boolean	errorflag=false;
	int	i;
	union REGS CPURegs;

	for (i=0;i<EMSpagesmapped;i++)
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
			mov	errorflag,1
			End:
		}
		if(errorflag==true)
		{
			err = CPURegs.h.ah;
			strcpy(str,"MM_MapEMS: EMS error 0x");
			itoa(err,str2,16);
			strcpy(str,str2);
			printf("%s\n",str);
		}
	}
	return;
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

boolean MML_CheckForXMS (void)
{
	boolean	errorflag=false;
	numUMBs = 0;

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

void MML_SetupXMS (void)
{
	unsigned	base,size;

	__asm
	{
		mov	ax,0x4310
		int	0x2f
		mov	[WORD PTR XMSaddr],bx
		mov	[WORD PTR XMSaddr+2],es		// function pointer to XMS driver
	}
getmemory:
	__asm
	{
		mov	ah,XMS_ALLOCUMB
		mov	dx,0xffff					// try for largest block possible
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
	MML_UseSpace (base,size);
	mminfo.XMSmem += size*16;
	UMBbase[numUMBs] = base;
	numUMBs++;
	if (numUMBs < MAXUMBS)
		goto getmemory;
}


/*
======================
=
= MML_ShutdownXMS
=
======================
*/

void MML_ShutdownXMS (void)
{
	int	i;
	unsigned	base;

	for (i=0;i<numUMBs;i++)
	{
		base = UMBbase[i];
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

void MML_UseSpace (unsigned segstart, unsigned seglength)
{
	mmblocktype far *scan,far *last;
	unsigned	oldend;
	long		extra;

	scan = last = mmhead;
	mmrover = mmhead;		// reset rover to start of memory

//
// search for the block that contains the range of segments
//
	while (scan->start+scan->length < segstart)
	{
		last = scan;
		scan = scan->next;
	}

//
// take the given range out of the block
//
	oldend = scan->start + scan->length;
	extra = oldend - (segstart+seglength);
	if (extra < 0)
	{
		printf("MML_UseSpace: Segment spans two blocks!");
		return;
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
		mmnew->next = scan->next;
		scan->next = mmnew;
		mmnew->start = segstart+seglength;
		mmnew->length = extra;
		mmnew->attributes = LOCKBIT;
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

void MML_ClearBlock (void)
{
	mmblocktype far *scan,far *last;

	scan = mmhead->next;

	while (scan)
	{
		if (!(scan->attributes&LOCKBIT) && (scan->attributes&PURGEBITS) )
		{
			MM_FreePtr(scan->useptr);
			return;
		}
		scan = scan->next;
	}

	printf("MM_ClearBlock: No purgable blocks!");
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

void MM_Startup (void)
{
	int i;
	dword length;
	void far 	*start;
	unsigned 	segstart,seglength,endfree;

	if (mminfo.mmstarted)
		MM_Shutdown ();


	mminfo.mmstarted = true;
	mminfo.bombonerror = true;
//
// set up the linked list (everything in the free list;
//
	mmhead = NULL;
	mmfree = &mmblocks[0];
	for (i=0;i<MAXBLOCKS-1;i++)
		mmblocks[i].next = &mmblocks[i+1];
	mmblocks[i].next = NULL;

//
// locked block of all memory until we punch out free space
//
	GETNEWBLOCK;
	mmhead = mmnew;				// this will allways be the first node
	mmnew->start = 0;
	mmnew->length = 0xffff;
	mmnew->attributes = LOCKBIT;
	mmnew->next = NULL;
	mmrover = mmhead;


//
// get all available near conventional memory segments
//
//----	length=coreleft();
	_nheapgrow();
	length=_memavl();
	start = (void far *)(nearheap = malloc(length));

	length -= 16-(FP_OFF(start)&15);
	length -= SAVENEARHEAP;
	seglength = length / 16;			// now in paragraphs
	segstart = FP_SEG(start)+(FP_OFF(start)+15)/16;
	MML_UseSpace (segstart,seglength);
	mminfo.nearheap = length;

//
// get all available far conventional memory segments
//
//----	length=farcoreleft();
	_fheapgrow();
	length=_memavl();
	start = farheap = _fmalloc(length);
	length -= 16-(FP_OFF(start)&15);
	length -= SAVEFARHEAP;
	seglength = length / 16;			// now in paragraphs
	segstart = FP_SEG(start)+(FP_OFF(start)+15)/16;
	MML_UseSpace (segstart,seglength);
	mminfo.farheap = length;
	mminfo.mainmem = mminfo.nearheap + mminfo.farheap;


//
// detect EMS and allocate up to 64K at page frame
//
	mminfo.EMSmem = 0;
	for (i = 1;i < __argc;i++)
	{
		if ( US_CheckParm(__argv[i],ParmStringsexmm) == 0)
			goto emsskip;				// param NOEMS
	}

	if (MML_CheckForEMS())
	{
		//printf("EMS1\n");
		MML_SetupEMS();					// allocate space
		//printf("EMS2\n");
		MML_UseSpace (EMSpageframe,EMSpagesmapped*0x400);
		//printf("EMS3\n");
		MM_MapEMS();					// map in used pages
		//printf("EMS4\n");
		mminfo.EMSmem = EMSpagesmapped*0x4000l;
	}

//
// detect XMS and get upper memory blocks
//
emsskip:
	mminfo.XMSmem = 0;
	for (i = 1;i < __argc;i++)
	{
		if ( US_CheckParm(__argv[i],ParmStringsexmm) == 0)
			goto xmsskip;				// param NOXMS
	}

	if (MML_CheckForXMS())
	{
//		printf("XMS!\n");
		MML_SetupXMS();					// allocate as many UMBs as possible
	}

//
// allocate the misc buffer
//
xmsskip:
	mmrover = mmhead;		// start looking for space after low block

	MM_GetPtr (&bufferseg,BUFFERSIZE);
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

void MM_Shutdown (void)
{
  if (!mminfo.mmstarted)
	return;

  _ffree (farheap);
  free (nearheap);
  hfree(hugeheap);
  MML_ShutdownEMS ();
  MML_ShutdownXMS ();
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

void MM_GetPtr (memptr *baseptr,dword size)
{
	mmblocktype far *scan,far *lastscan,far *endscan
				,far *purge,far *next;
	int			search;
	unsigned	needed,startseg;

	needed = (size+15)/16;		// convert size from bytes to paragraphs

	GETNEWBLOCK;				// fill in start and next after a spot is found
	mmnew->length = needed;
	mmnew->useptr = baseptr;
	mmnew->attributes = BASEATTRIBUTES;

	for (search = 0; search<3; search++)
	{
	//
	// first search:	try to allocate right after the rover, then on up
	// second search: 	search from the head pointer up to the rover
	// third search:	compress memory, then scan from start
		if (search == 1 && mmrover == mmhead)
			search++;

		switch (search)
		{
		case 0:
			lastscan = mmrover;
			scan = mmrover->next;
			endscan = NULL;
			break;
		case 1:
			lastscan = mmhead;
			scan = mmhead->next;
			endscan = mmrover;
			break;
		case 2:
			MM_SortMem ();
			lastscan = mmhead;
			scan = mmhead->next;
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
				lastscan->next = mmnew;
				mmnew->start = *(unsigned *)baseptr = startseg;
				mmnew->next = scan;
				while ( purge != scan)
				{	// free the purgable block
					next = purge->next;
					FREEBLOCK(purge);
					purge = next;		// purge another if not at scan
				}
				mmrover = mmnew;
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

	if (mminfo.bombonerror)
		printf(OUT_OF_MEM_MSG,(size-mminfo.nearheap));
	else
		mminfo.mmerror = true;
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

void MM_FreePtr (memptr *baseptr)
{
	mmblocktype far *scan,far *last;

	last = mmhead;
	scan = last->next;

	if (baseptr == mmrover->useptr)	// removed the last allocated block
		mmrover = mmhead;

	while (scan->useptr != baseptr && scan)
	{
		last = scan;
		scan = scan->next;
	}

	if (!scan)
	{
		printf("MM_FreePtr: Block not found!");
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

void MM_SetPurge (memptr *baseptr, int purge)
{
	mmblocktype far *start;

	start = mmrover;

	do
	{
		if (mmrover->useptr == baseptr)
			break;

		mmrover = mmrover->next;

		if (!mmrover)
			mmrover = mmhead;
		else if (mmrover == start)
		{
			printf("MM_SetPurge: Block not found!");
			return;
		}

	} while (1);

	mmrover->attributes &= ~PURGEBITS;
	mmrover->attributes |= purge;
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

void MM_SetLock (memptr *baseptr, boolean locked)
{
	mmblocktype far *start;

	start = mmrover;

	do
	{
		if (mmrover->useptr == baseptr)
			break;

		mmrover = mmrover->next;

		if (!mmrover)
			mmrover = mmhead;
		else if (mmrover == start)
		{
			printf("MM_SetLock: Block not found!");
			return;
		}

	} while (1);

	mmrover->attributes &= ~LOCKBIT;
	mmrover->attributes |= locked*LOCKBIT;
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

void MM_SortMem (void)
{
	mmblocktype far *scan,far *last,far *next;
	unsigned	start,length,source,dest,oldborder;
	int			playing;

	//
	// lock down a currently playing sound
	//
/*++++	playing = SD_SoundPlaying ();
	if (playing)
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

	if (beforesort)
		beforesort();

	scan = mmhead;

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
				FREEBLOCK(scan);
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

	mmrover = mmhead;

	if (aftersort)
		aftersort();

//	VW_ColorBorder (oldborder);

/*++++	if (playing)
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

void MM_ShowMemory (void)
{
	mmblocktype far *scan;
	unsigned color,temp;//, i;
	long	end,owner;
	char    scratch[80],str[10];

//****	VW_SetDefaultColors();
//****	VW_SetLineWidth(40);
//++++mh	temp = bufferofs;
//++++mh	bufferofs = 0;
//****	VW_SetScreen (0,0);

	scan = mmhead;

	end = -1;

//CA_OpenDebug ();

	while (scan)
	{
		if (scan->attributes & PURGEBITS)
			color = 5;		// dark purple = purgable
		else
			color = 9;		// medium blue = non purgable
		if (scan->attributes & LOCKBIT)
			color = 12;		// red = locked
		if (scan->start<=end)
		{
			printf("MM_ShowMemory: Memory block order currupted!");
			return;
		}
		end = scan->start+scan->length-1;
//++++		VW_Hlin(scan->start,(unsigned)end,0,color);
//++++		VW_Plot(scan->start,0,15);
		if (scan->next->start > end+1)
//++++			VW_Hlin(end+1,scan->next->start,0,0);	// black = free

//****#if 0
printf("Location:");
printf("%Fp\t", scan->start);
strcpy (scratch,"Size:");
ltoa ((long)scan->length*16,str,10);
strcat (scratch,str);
strcat (scratch,"\tOwner:0x");
owner = (unsigned)scan->useptr;
ultoa (owner,str,16);
strcat (scratch,str);
strcat (scratch,"\n");
//++++write (debughandle,scratch,strlen(scratch));
fprintf(stdout, "%s", scratch);
//****#endif

		scan = scan->next;
	}

//CA_CloseDebug ();

//++++mh	IN_Ack();
//****	VW_SetLineWidth(64);
//++++mh	bufferofs = temp;
}
//****#endif

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

dword MM_UnusedMemory (void)
{
	unsigned free;
	mmblocktype far *scan;

	free = 0;
	scan = mmhead;

	while (scan->next)
	{
		free += scan->next->start - (scan->start + scan->length);
		scan = scan->next;
	}

//	return free*16l;
	return free;
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

dword MM_TotalFree (void)
{
	unsigned free;
	mmblocktype far *scan;

	free = 0;
	scan = mmhead;

	while (scan->next)
	{
		if ((scan->attributes&PURGEBITS) && !(scan->attributes&LOCKBIT))
			free += scan->length;
		free += scan->next->start - (scan->start + scan->length);
		scan = scan->next;
	}

//	return free*16l;
	return free;
}

//==========================================================================

/*
=====================
=
= MM_Report
=
=====================
*/

void MM_Report(void)
{
	printf("EMM %x available\n", EMSVer);
	printf("totalEMSpages=%u\n", totalEMSpages);
	printf("freeEMSpages=%u\n", freeEMSpages);
	printf("EMSpageframe=%Fp\n", EMSpageframe);
	printf("near=%lu\n", mminfo.nearheap);
	printf("far=%lu\n", mminfo.farheap);
	printf("EMSmem=%lu\n", mminfo.EMSmem);
	printf("XMSmem=%lu\n", mminfo.XMSmem);
	printf("mainmem=%lu\n", mminfo.mainmem);
	printf("UnusedMemory=%lu\n", MM_UnusedMemory());
	printf("TotalFree=%lu\n", MM_TotalFree());
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

void MM_BombOnError (boolean bomb)
{
	mminfo.bombonerror = bomb;
}

//==========================================================================

///////////////////////////////////////////////////////////////////////////
//
//      US_CheckParm() - checks to see if a string matches one of a set of
//              strings. The check is case insensitive. The routine returns the
//              index of the string that matched, or -1 if no matches were found
//
///////////////////////////////////////////////////////////////////////////
int
US_CheckParm(char *parm,char **strings)
{
	char    cp,cs,
			*p,*s;
	int             i;

	while (!isalpha(*parm)) // Skip non-alphas
		parm++;

	for (i = 0;*strings && **strings;i++)
	{
		for (s = *strings++,p = parm,cs = cp = 0;cs == cp;)
		{
			cs = *s++;
			if (!cs)
				return(i);
			cp = *p++;

			if (isupper(cs))
				cs = tolower(cs);
			if (isupper(cp))
				cp = tolower(cp);
		}
	}
	return(-1);
}

