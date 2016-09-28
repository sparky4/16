/* Project 16 Source Code~
 * Copyright (C) 2012-2016 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

//
//	ID_PM.C
//	Id Engine's Page Manager v1.0
//	Primary coder: Jason Blochowiak
//

#include "src/lib/16_pm.h"
#pragma hdrstop

#ifdef __DEBUG__
boolean dbg_debugpm=0;
#endif

//	Main Mem specific variables
	//boolean			MainPresent;
/*	memptr			MainMemPages[PMMaxMainMem];
	PMBlockAttr		gvar->pm.mm.MainMemUsed[PMMaxMainMem];
	int				gvar->pm.mm.MainPagesAvail;*/

//	EMS specific variables
	//boolean			EMSPresent;
/*	word			gvar->pm.emm.EMSAvail,gvar->pm.emm.EMSPagesAvail,gvar->pm.emm.EMSHandle,
					gvar->pm.emm.EMSPageFrame,gvar->pm.emm.EMSPhysicalPage;
	gvar->pm.emm.EMSListStruct	gvar->pm.emm.EMSList[EMSFrameCount];*/

//	XMS specific variables
	//boolean			XMSPresent;
	//word			gvar->pm.xmm.XMSAvail,gvar->pm.xmm.XMSPagesAvail,gvar->pm.xmm.XMSHandle;
	dword		XMSDriver;
/*	int				gvar->pm.xmm.XMSProtectPage = -1;

//	File specific variables
	char			gvar->pm.fi.PageFileName[13] = {"VSWAP."};
	int				PageFile = -1;
	word			gvar->pm.fi.ChunksInFile;
	word			PMSpriteStart,PMSoundStart;*/

//	General usage variables
/*	boolean			PMStarted,
					gvar->pm.PMPanicMode,
					gvar->pm.PMThrashing;
	word			gvar->pm.XMSPagesUsed,
					gvar->pm.EMSPagesUsed,
					MainPagesUsed,
					gvar->pm.PMNumBlocks;
	long			PMFrameCount;
	PageListStruct	far *gvar->pm.PMPages,
					_seg *gvar->pm.PMSegPages;*/

static	char		*ParmStrings[] = {"nomain","noems","noxms",nil};

/////////////////////////////////////////////////////////////////////////////
//
//	EMS Management code
//
/////////////////////////////////////////////////////////////////////////////

//
//	PML_MapEMS() - Maps a logical page to a physical page
//
byte
PML_MapEMS(word logical, byte physical, global_game_variables_t *gvar)
{
	byte	str[160];
	unsigned	EMShandle;
	byte err;
	boolean	errorflag=false;
	int	i;
	EMShandle=gvar->pm.emm.EMSHandle;

	__asm {
		mov	ah,EMS_MAPPAGE
		mov	al,physical
		mov	bx,logical
		mov	dx,EMShandle
		int	EMS_INT
		or	ah,ah
		jnz	errorme
		jmp	Endme
#ifdef __BORLANDC__
	}
#endif
		errorme:
#ifdef __BORLANDC__
	__asm {
#endif
		mov	err,ah
		mov	errorflag,1
#ifdef __BORLANDC__
	}
#endif
		Endme:
#ifdef __WATCOMC__
	}
#endif
	if(errorflag==true)
	{
		strcpy(str,"MM_MapEMS: EMS error ");
		MM_EMSerr(str, err);
		printf("%s\n",str);
		Quit("PML_MapEMS: Page mapping failed\n");
		return err;
	}
	return 0;
}

//
//	PML_StartupEMS() - Sets up EMS for Page Mgr's use
//		Checks to see if EMS driver is present
//      Verifies that EMS hardware is present
//		Make sure that EMS version is 3.2 or later
//		If there's more than our minimum (2 pages) available, allocate it (up
//			to the maximum we need)
//

boolean
PML_StartupEMS(global_game_variables_t *gvar)
{
	int		i;
	long	size;
	byte	str[64];
	byte err;

	boolean errorflag=0;
	static char	emmname[] = "EMMXXXX0";	//fix by andrius4669
	unsigned int EMSVer = 0;
	unsigned	totalEMSpages,freeEMSpages,EMSPageFrame,EMSHandle,EMSAvail;
	totalEMSpages = freeEMSpages = EMSPageFrame = 0;
	gvar->pm.emm.EMSPresent = false;			// Assume that we'll fail
	gvar->pm.emm.EMSAvail = 0;
	gvar->mmi.EMSmem = 0;

	__asm {
		mov	dx,OFFSET emmname	//fix by andrius4669
		mov	ax,0x3d00
		int	EMS_INT		// try to open EMMXXXX0 device
		jc	error1

		mov	bx,ax
		mov	ax,0x4400

		int	EMS_INT		// get device info
		jc	error1

		and	dx,0x80
		jz	error1

		mov	ax,0x4407

		int	EMS_INT		// get status
		jc	error1
		or	al,al
		jz	error1

		mov	ah,0x3e
		int	EMS_INT		// close handle
		jc	error1

		mov	ah,EMS_STATUS
		int	EMS_INT
		jc	error1			// make sure EMS hardware is present

		mov	ah,EMS_VERSION
		int	EMS_INT			// only work on EMS 3.2 or greater (silly, but...)
		or	ah,ah
		jnz	error1
		mov	[EMSVer],ax		//	set EMSVer
		cmp	al,0x32			// only work on ems 3.2 or greater
		jb	error1

		mov	ah,EMS_GETFRAME
		int	EMS_INT			// find the page frame address
		or	ah,ah
		jnz	error1
		mov	[EMSPageFrame],bx

		mov	ah,EMS_GETPAGES
		int	EMS_INT			// find out how much EMS is there
		or	ah,ah
		jnz	error1
		or	bx,bx
		jz	noEMS			// no EMS at all to allocate
		cmp	bx,2
		jl	noEMS			// Require at least 2 pages (32k)
		mov	[totalEMSpages],dx
		mov	[freeEMSpages],bx
		mov	[EMSAvail],bx
		jmp End1
#ifdef __BORLANDC__
	}
#endif
	error1:
#ifdef __BORLANDC__
	__asm {
#endif
		mov	err,ah
		mov	errorflag,1
		jmp End1
#ifdef __BORLANDC__
	}
#endif
noEMS:
End1:
#ifdef __WATCOMC__
	}
#endif
/*
	if(errorflag==false)
	{
		// Don't hog all available EMS
		size = gvar->pm.emm.EMSAvail * (long)EMSPageSize;
		if (size - (EMSPageSize * 2) > (gvar->pm.fi.ChunksInFile * (long)PMPageSize))
		{
			size = (gvar->pm.fi.ChunksInFile * (long)PMPageSize) + EMSPageSize;
			gvar->pm.emm.EMSAvail = size / EMSPageSize;
		}
	}
*/
	__asm {
		mov	ah,EMS_ALLOCPAGES
		mov	bx,[EMSAvail];
		int	EMS_INT
		or	ah,ah
		jnz	error2
		mov	[EMSHandle],dx
		jmp	End2
#ifdef __BORLANDC__
	}
#endif
	error2:
#ifdef __BORLANDC__
	__asm {
#endif
		mov	err,ah
		mov	errorflag,1
		jmp End2
#ifdef __BORLANDC__
	}
#endif
End2:
#ifdef __WATCOMC__
	}
#endif

	if(errorflag==false)
	{
	gvar->mmi.EMSmem = EMSAvail * (dword)EMSPageSize;

	// Initialize EMS mapping cache
	for (i = 0;i < EMSFrameCount;i++)
		gvar->pm.emm.EMSList[i].baseEMSPage = -1;

	}
	else
	{
		strcpy(str,"PML_StartupEMS: EMS error ");
		MM_EMSerr(str, err);
		printf("%s\n",str);
		return(gvar->pm.emm.EMSPresent);
	}

	gvar->pm.emm.EMSPresent = true;			// We have EMS
	gvar->pm.emm.EMSPageFrame = EMSPageFrame;
	gvar->pm.emm.EMSAvail = EMSAvail;
	gvar->pm.emm.EMSVer = EMSVer;
	gvar->pm.emm.EMSHandle = EMSHandle;
	gvar->pm.emm.freeEMSpages = freeEMSpages;
	gvar->pm.emm.totalEMSpages = totalEMSpages;

	return(gvar->pm.emm.EMSPresent);
}

//
//	PML_ShutdownEMS() - If EMS was used, deallocate it
//
void
PML_ShutdownEMS(global_game_variables_t *gvar)
{
	word EMSHandle;
	byte err;
	byte	str[64];
	boolean errorflag=false;

	EMSHandle=gvar->pm.emm.EMSHandle;

	if (gvar->pm.emm.EMSPresent)
	{
		__asm {
			mov	ah,EMS_FREEPAGES
			mov	dx,[EMSHandle]
			int	EMS_INT
			jc	errores
			jmp	Endes
#ifdef __BORLANDC__
		}
#endif
			errores:
#ifdef __BORLANDC__
		__asm {
#endif
			mov	err,ah
			mov	errorflag,1
			jmp	Endes
#ifdef __BORLANDC__
		}
#endif
			Endes:
#ifdef __WATCOMC__
		}
#endif
		if(errorflag==true)
		{
			strcpy(str,"PML_ShutdownEMS: Error freeing EMS ");
			MM_EMSerr(str, err);
			printf("%s\n",str);
			Quit("PML_ShutdownEMS: Error freeing EMS\n");
			//return;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//	XMS Management code
//
/////////////////////////////////////////////////////////////////////////////

//
//	PML_StartupXMS() - Starts up XMS for the Page Mgr's use
//		Checks for presence of an XMS driver
//		Makes sure that there's at least a page of XMS available
//		Allocates any remaining XMS (rounded down to the nearest page size)
//
boolean
PML_StartupXMS(global_game_variables_t *gvar)
{
	boolean errorflag;
	word XMSAvail, XMSHandle;
	errorflag=gvar->pm.xmm.XMSPresent = false;					// Assume failure
	gvar->pm.xmm.XMSAvail = gvar->mmi.XMSmem = 0;

	__asm {
		mov	ax,0x4300
		int	XMS_INT         				// Check for presence of XMS driver
		cmp	al,0x80
		jne	errorxu

		mov	ax,0x4310
		int	XMS_INT							// Get address of XMS driver
		mov	[WORD PTR XMSDriver],bx
		mov	[WORD PTR XMSDriver+2],es		// function pointer to XMS driver

		//XMS_CALL(XMS_QUERYFREE);			// Find out how much XMS is available
		mov	ah,XMS_QUERYFREE
		call	[DWORD PTR XMSDriver]
		mov	[XMSAvail],ax
		or	ax,ax				// AJR: bugfix 10/8/92
		jz	errorxu
		jmp	Endxu
#ifdef __BORLANDC__
	}
#endif
		errorxu:
#ifdef __BORLANDC__
	__asm {
#endif
		//mov	err,ah
		mov	errorflag,1
		jmp	Endxu
#ifdef __BORLANDC__
	}
#endif
		Endxu:
#ifdef __WATCOMC__
	}
#endif
	XMSAvail &= ~(PMPageSizeKB - 1);	// Round off to nearest page size
	if (XMSAvail < (PMPageSizeKB * 2)){	// Need at least 2 pages
		printf("PISS! %u\n", XMSAvail);
		goto error;
	}

	__asm {
		mov	dx,[XMSAvail]
		//XMS_CALL(XMS_ALLOC);				// And do the allocation
		mov	ah,XMS_ALLOC
		call	[WORD PTR XMSDriver]
		mov	[XMSHandle],dx
		or	ax,ax				// AJR: bugfix 10/8/92
		jz	errorxuu
		jmp	Endxuu
#ifdef __BORLANDC__
		}
#endif
		errorxuu:
#ifdef __BORLANDC__
	__asm {
#endif
		//mov	err,ah
		mov	[XMSAvail],0
		mov	errorflag,1
		jmp	Endxuu
#ifdef __BORLANDC__
	}
#endif
		Endxuu:
#ifdef __WATCOMC__
	}
#endif

	if(errorflag==false)
	{
		gvar->pm.xmm.XMSAvail = XMSAvail;
		gvar->mmi.XMSmem += gvar->pm.xmm.XMSAvail * (dword)1024;
		gvar->pm.xmm.XMSHandle = XMSHandle;

		gvar->pm.xmm.XMSPresent = true;
		printf("	%u	%u	%u\n", gvar->mmi.XMSmem, gvar->pm.xmm.XMSAvail, XMSAvail);
		getch();
	}else printf("	errorflag\n");
error:
	return(gvar->pm.xmm.XMSPresent);
}

//
//	PML_XMSCopy() - Copies a main/EMS page to or from XMS
//		Will round an odd-length request up to the next even value
//
void
PML_XMSCopy(boolean toxms,byte far *addr,word xmspage,word length, global_game_variables_t *gvar)
{
	dword	xoffset;
	struct
	{
		dword	length;
		word		source_handle;
		dword	source_offset;
		word		target_handle;
		dword	target_offset;
	} copy;

	if (!addr)
	{
		Quit("PML_XMSCopy: zero address\n");
		//return;
	}

	xoffset = (dword)xmspage * PMPageSize;

	copy.length = (length + 1) & ~1;
	copy.source_handle = toxms? 0 : gvar->pm.xmm.XMSHandle;
	copy.source_offset = toxms? (long)addr : xoffset;
	copy.target_handle = toxms? gvar->pm.xmm.XMSHandle : 0;
	copy.target_offset = toxms? xoffset : (long)addr;

	__asm {
		push si
	}
	_SI = (word)&copy;
	XMS_CALL(XMS_MOVE);
	__asm {
		pop	si
	}
	if (!_AX)
	{
		Quit("PML_XMSCopy: Error on copy");
		//return;
	}
}

#if 1
#define	PML_CopyToXMS(s,t,l,gvar)	PML_XMSCopy(true,(s),(t),(l),(gvar))
#define	PML_CopyFromXMS(t,s,l,gvar)	PML_XMSCopy(false,(t),(s),(l),(gvar))
#else
//
//	PML_CopyToXMS() - Copies the specified number of bytes from the real mode
//		segment address to the specified XMS page
//
void
PML_CopyToXMS(byte far *source,int targetpage,word length,global_game_variables_t *gvar)
{
	PML_XMSCopy(true,source,targetpage,length, global_game_variables_t *gvar);
}

//
//	PML_CopyFromXMS() - Copies the specified number of bytes from an XMS
//		page to the specified real mode address
//
void
PML_CopyFromXMS(byte far *target,int sourcepage,word length, global_game_variables_t *gvar)
{
	PML_XMSCopy(false,target,sourcepage,length, global_game_variables_t *gvar);
}
#endif

//
//	PML_ShutdownXMS()
//
void
PML_ShutdownXMS(global_game_variables_t *gvar)
{
	boolean errorflag=false;
	word XMSHandle = gvar->pm.xmm.XMSHandle;
	if (gvar->pm.xmm.XMSPresent)
	{
		__asm {
			mov	dx,[XMSHandle]
			//XMS_CALL(XMS_FREE);
			mov	ah,XMS_FREE
			call	[WORD PTR XMSDriver]
			or	bl,bl
			jz	errorxs
			jmp	Endxs
#ifdef __BORLANDC__
		}
#endif
			errorxs:
#ifdef __BORLANDC__
		__asm {
#endif
			//mov	err,ah
			mov	errorflag,1
			jmp	Endxs
#ifdef __BORLANDC__
		}
#endif
			Endxs:
#ifdef __WATCOMC__
		}
#endif
		if(errorflag==true)
		{
			Quit("PML_ShutdownXMS: Error freeing XMS");
			//return;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//	Main memory code
//
/////////////////////////////////////////////////////////////////////////////

//
//	PM_SetMainMemPurge() - Sets the purge level for all allocated main memory
//		blocks. This shouldn't be called directly - the PM_LockMainMem() and
//		PM_UnlockMainMem() macros should be used instead.
//
void
PM_SetMainMemPurge(int level, global_game_variables_t *gvar)
{
	int	i;

	if(gvar->pm.mm.MainPresent)
	for (i = 0;i < PMMaxMainMem;i++)
	{
#ifdef __DEBUG_PM__
		printf("PM_SetMainMemPurge()	info of gvar->pm.mm.MainMemPages[i]\n");
		printf("&	%Fp,	%Fp\n", &gvar->pm.mm.MainMemPages[i],	&(gvar->pm.mm.MainMemPages[i]));
#endif
		if (gvar->pm.mm.MainMemPages[i])
			MM_SetPurge(&(gvar->pm.mm.MainMemPages[i]),level, gvar);
	}

	else
	{
		Quit("MainPresent IS NULL\n");
	}
}

//
//	PM_CheckMainMem() - If something besides the Page Mgr makes requests of
//		the Memory Mgr, some of the Page Mgr's blocks may have been purged,
//		so this function runs through the block list and checks to see if
//		any of the blocks have been purged. If so, it marks the corresponding
//		page as purged & unlocked, then goes through the block list and
//		tries to reallocate any blocks that have been purged.
//	This routine now calls PM_LockMainMem() to make sure that any allocation
//		attempts made during the block reallocation sweep don't purge any
//		of the other blocks. Because PM_LockMainMem() is called,
//		PM_UnlockMainMem() needs to be called before any other part of the
//		program makes allocation requests of the Memory Mgr.
//
void
PM_CheckMainMem(global_game_variables_t *gvar)
{
	boolean			allocfailed;
	int				i,n;
	memptr			*p;
	PMBlockAttr		*used;
	PageListStruct	far *page;

	if (!gvar->pm.mm.MainPresent)
		return;

	for (i = 0,page = gvar->pm.PMPages;i < gvar->pm.fi.ChunksInFile;i++,page++)
	{
		n = page->mainPage;
		if (n != -1)						// Is the page using main memory?
		{
			if (!gvar->pm.mm.MainMemPages[n])			// Yep, was the block purged?
			{
				page->mainPage = -1;		// Yes, mark page as purged & unlocked
				page->locked = pml_Unlocked;
			}
		}
	}

	// Prevent allocation attempts from purging any of our other blocks
	PM_LockMainMem(gvar);
	allocfailed = false;
	for (i = 0,p = gvar->pm.mm.MainMemPages,used = gvar->pm.mm.MainMemUsed; i < PMMaxMainMem;i++,p++,used++)
	{
		if (!*p)							// If the page got purged
		{
			if (*used & pmba_Allocated)		// If it was allocated
			{
				*used &= ~pmba_Allocated;	// Mark as unallocated
				gvar->pm.mm.MainPagesAvail--;			// and decrease available count
			}

			if (*used & pmba_Used)			// If it was used
			{
				*used &= ~pmba_Used;		// Mark as unused
				gvar->pm.MainPagesUsed--;			// and decrease used count
			}

			if (!allocfailed)
			{
				MM_BombOnError(false, gvar);
				MM_GetPtr(p,PMPageSize, gvar);		// Try to reallocate
				if (gvar->mm.mmerror)					// If it failed,
					allocfailed = true;			//  don't try any more allocations
				else							// If it worked,
				{
					*used |= pmba_Allocated;	// Mark as allocated
					gvar->pm.mm.MainPagesAvail++;			// and increase available count
				}
				MM_BombOnError(true, gvar);
			}
		}
	}
	if (gvar->mm.mmerror)
		gvar->mm.mmerror = false;
}

//
//	PML_StartupMainMem() - Allocates as much main memory as is possible for
//		the Page Mgr. The memory is allocated as non-purgeable, so if it's
//		necessary to make requests of the Memory Mgr, PM_UnlockMainMem()
//		needs to be called.
//
void
PML_StartupMainMem(global_game_variables_t *gvar)
{
	int		i,n;
	memptr	*p;

	gvar->pm.mm.MainPagesAvail = 0;
	gvar->pm.mm.MainPresent = false;
	MM_BombOnError(false, gvar);
	for (i = 0,p = gvar->pm.mm.MainMemPages;i < PMMaxMainMem;i++,p++)
	{
		MM_GetPtr(p,PMPageSize, gvar);
		if (gvar->mm.mmerror)
			break;

		gvar->pm.mm.MainPagesAvail++;
		gvar->pm.mm.MainMemUsed[i] = pmba_Allocated;
	}
	MM_BombOnError(true, gvar);
	if (gvar->mm.mmerror)
		gvar->mm.mmerror = false;
	if (gvar->pm.mm.MainPagesAvail < PMMinMainMem)
	{
		Quit("PM_SetupMainMem: Not enough main memory");
		//return;
	}
	gvar->pm.mm.MainPresent = true;
}

//
//	PML_ShutdownMainMem() - Frees all of the main memory blocks used by the
//		Page Mgr.
//
void
PML_ShutdownMainMem(global_game_variables_t *gvar)
{
	int		i;
	memptr	*p;

	// DEBUG - mark pages as unallocated & decrease page count as appropriate
	for (i = 0,p = gvar->pm.mm.MainMemPages;i < PMMaxMainMem;i++,p++)
		if (*p)
			MM_FreePtr(p, gvar);
}

/////////////////////////////////////////////////////////////////////////////
//
//	File management code
//
/////////////////////////////////////////////////////////////////////////////

//
//	PML_ReadFromFile() - Reads some data in from the page file
//
void
PML_ReadFromFile(byte far *buf,long offset,word length, global_game_variables_t *gvar)
{
	if (!buf)
	{
		Quit("PML_ReadFromFile: Null pointer");
		//return;
	}
	if (!offset)
	{
		Quit("PML_ReadFromFile: Zero offset");
		//return;
	}
	if (lseek(gvar->pm.fi.PageFile,offset,SEEK_SET) != offset)
	{
		Quit("PML_ReadFromFile: Seek failed");
		//return;
	}
	if (!CA_FarRead(gvar->pm.fi.PageFile,buf,length, gvar))
	{
		Quit("PML_ReadFromFile: Read failed");
		//return;
	}
}

//
//	PML_OpenPageFile() - Opens the page file and sets up the page info
//
void
PML_OpenPageFile(global_game_variables_t *gvar)
{
	int				i;
	long			size;
				//__SEGA buf;
	memptr		buf;
	dword		far *offsetptr;
	word			far *lengthptr;
	PageListStruct	far *page;

	gvar->pm.fi.PageFile = open(gvar->pm.fi.PageFileName,O_RDONLY + O_BINARY);
	if (gvar->pm.fi.PageFile == -1)
	{
		Quit("PML_OpenPageFile: Unable to open page file");
		//return;
	}

	// Read in header variables
	read(gvar->pm.fi.PageFile,&gvar->pm.fi.ChunksInFile,sizeof(gvar->pm.fi.ChunksInFile));
	read(gvar->pm.fi.PageFile,&gvar->pm.fi.PMSpriteStart,sizeof(gvar->pm.fi.PMSpriteStart));
	read(gvar->pm.fi.PageFile,&gvar->pm.fi.PMSoundStart,sizeof(gvar->pm.fi.PMSoundStart));

	// Allocate and clear the page list
	gvar->pm.PMNumBlocks = gvar->pm.fi.ChunksInFile;
	MM_GetPtr((memptr *)gvar->pm.PMSegPages, sizeof(PageListStruct) * (gvar->pm.PMNumBlocks), gvar);
	MM_SetLock((memptr *)gvar->pm.PMSegPages,true, gvar);
	gvar->pm.PMPages = (PageListStruct far *)gvar->pm.PMSegPages;
	_fmemset(gvar->pm.PMPages,0,sizeof(PageListStruct) * gvar->pm.PMNumBlocks);

	// Read in the chunk offsets
	size = sizeof(dword) * gvar->pm.fi.ChunksInFile;
	MM_GetPtr(&buf, size, gvar);
	if (!CA_FarRead(gvar->pm.fi.PageFile,(byte far *)buf,size, gvar))
	{
		Quit("PML_OpenPageFile: Offset read failed");
		//return;
	}
	offsetptr = (dword far *)buf;
	for (i = 0,page = gvar->pm.PMPages;i < gvar->pm.fi.ChunksInFile;i++,page++)
		page->offset = *offsetptr++;
	MM_FreePtr(&buf, gvar);

	// Read in the chunk lengths
	size = sizeof(word) * gvar->pm.fi.ChunksInFile;
	MM_GetPtr(&buf,size, gvar);
	if (!CA_FarRead(gvar->pm.fi.PageFile,(byte far *)buf,size, gvar))
	{
		Quit("PML_OpenPageFile: Length read failed");
		//return;
	}
	lengthptr = (word far *)buf;
	for (i = 0,page = gvar->pm.PMPages;i < gvar->pm.fi.ChunksInFile;i++,page++)
		page->length = *lengthptr++;
	MM_FreePtr(&buf, gvar);
}

//
//  PML_ClosePageFile() - Closes the page file
//
void
PML_ClosePageFile(global_game_variables_t *gvar)
{
	if (gvar->pm.fi.PageFile != -1)
		close(gvar->pm.fi.PageFile);
	if (gvar->pm.PMSegPages)
	{
		MM_SetLock((memptr)gvar->pm.PMSegPages,false, gvar);
		MM_FreePtr((memptr)gvar->pm.PMSegPages, gvar);
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//	Allocation, etc., code
//
/////////////////////////////////////////////////////////////////////////////

//
//	PML_GetEMSAddress()
//
// 		Page is in EMS, so figure out which EMS physical page should be used
//  		to map our page in. If normal page, use EMS physical page 3, else
//  		use the physical page specified by the lock type
//
#ifndef __DEBUG_2__
#pragma argsused	// DEBUG - remove lock parameter
memptr
PML_GetEMSAddress(int page,PMLockType lock, global_game_variables_t *gvar)
{
	int		i,emspage;
	word	emsoff,emsbase,offset;

	emsoff = page & (PMEMSSubPage - 1);
	emsbase = page - emsoff;

	emspage = -1;
	// See if this page is already mapped in
	for (i = 0;i < EMSFrameCount;i++)
	{
		if (gvar->pm.emm.EMSList[i].baseEMSPage == emsbase)
		{
			emspage = i;	// Yep - don't do a redundant remapping
			break;
		}
	}

	// If page isn't already mapped in, find LRU EMS frame, and use it
	if (emspage == -1)
	{
		dword last = LONG_MAX;
		for (i = 0;i < EMSFrameCount;i++)
		{
			if (gvar->pm.emm.EMSList[i].lastHit < last)
			{
				emspage = i;
				last = gvar->pm.emm.EMSList[i].lastHit;
			}
		}

		gvar->pm.emm.EMSList[emspage].baseEMSPage = emsbase;
		PML_MapEMS(page / PMEMSSubPage,emspage, gvar);
	}

	if (emspage == -1)
		Quit("PML_GetEMSAddress: EMS find failed");

	gvar->pm.emm.EMSList[emspage].lastHit = gvar->pm.PMFrameCount;
	offset = emspage * EMSPageSizeSeg;
	offset += emsoff * PMPageSizeSeg;
	return((memptr)(gvar->pm.emm.EMSPageFrame + offset));
}
#else
memptr
PML_GetEMSAddress(int page,PMLockType lock, global_game_variables_t *gvar)
{
	word	emspage;

	emspage = (lock < pml_EMSLock)? 3 : (lock - pml_EMSLock);

	PML_MapEMS(page / PMEMSSubPage,emspage);

	return((memptr)(EMSPageFrame + (emspage * EMSPageSizeSeg)
			+ ((page & (PMEMSSubPage - 1)) * PMPageSizeSeg)));
}
#endif

//
//	PM_GetPageAddress() - Returns the address of a given page
//		Maps in EMS if necessary
//		Returns nil if block isn't cached into Main Memory or EMS
//
//
memptr
PM_GetPageAddress(int pagenum, global_game_variables_t *gvar)
{
	PageListStruct	far *page;

	page = &gvar->pm.PMPages[pagenum];
	if (page->mainPage != -1)
		return(gvar->pm.mm.MainMemPages[page->mainPage]);
	else if (page->emsPage != -1)
		return(PML_GetEMSAddress(page->emsPage,page->locked, gvar));
	else
		return(nil);
}

//
//	PML_GiveLRUPage() - Returns the page # of the least recently used
//		present & unlocked main/EMS page (or main page if mainonly is true)
//
int
PML_GiveLRUPage(boolean mainonly, global_game_variables_t *gvar)
{
	int				i,lru;
	long			last;
	PageListStruct	far *page;

	for (i = 0,page = gvar->pm.PMPages,lru = -1,last = LONG_MAX;i < gvar->pm.fi.ChunksInFile;i++,page++)
	{
		if
		(
			(page->lastHit < last)
		&&	((page->emsPage != -1) || (page->mainPage != -1))
		&& 	(page->locked == pml_Unlocked)
		&&	(!(mainonly && (page->mainPage == -1)))
		)
		{
			last = page->lastHit;
			lru = i;
		}
	}

	if (lru == -1)
		Quit("PML_GiveLRUPage: LRU Search failed");
	return(lru);
}

//
//	PML_GiveLRUXMSPage() - Returns the page # of the least recently used
//		(and present) XMS page.
//	This routine won't return the XMS page protected (by XMSProtectPage)
//
int
PML_GiveLRUXMSPage(global_game_variables_t *gvar)
{
	int				i,lru;
	long			last;
	PageListStruct	far *page;

	for (i = 0,page = gvar->pm.PMPages,lru = -1,last = LONG_MAX;i < gvar->pm.fi.ChunksInFile;i++,page++)
	{
		if
		(
			(page->xmsPage != -1)
		&&	(page->lastHit < last)
		&&	(i != gvar->pm.xmm.XMSProtectPage)
		)
		{
			last = page->lastHit;
			lru = i;
		}
	}
	return(lru);
}

//
//	PML_PutPageInXMS() - If page isn't in XMS, find LRU XMS page and replace
//		it with the main/EMS page
//
void
PML_PutPageInXMS(int pagenum, global_game_variables_t *gvar)
{
	int				usexms;
	PageListStruct	far *page;

	if (!gvar->pm.xmm.XMSPresent)
		return;

	page = &gvar->pm.PMPages[pagenum];
	if (page->xmsPage != -1)
		return;					// Already in XMS

	if (gvar->pm.XMSPagesUsed < gvar->pm.xmm.XMSPagesAvail)
		page->xmsPage = gvar->pm.XMSPagesUsed++;
	else
	{
		usexms = PML_GiveLRUXMSPage(gvar);
		if (usexms == -1)
			Quit("PML_PutPageInXMS: No XMS LRU");
		page->xmsPage = gvar->pm.PMPages[usexms].xmsPage;
		gvar->pm.PMPages[usexms].xmsPage = -1;
	}
	PML_CopyToXMS(PM_GetPageAddress(pagenum, gvar),page->xmsPage,page->length, gvar);
}

//
//	PML_TransferPageSpace() - A page is being replaced, so give the new page
//		the old one's address space. Returns the address of the new page.
//
memptr
PML_TransferPageSpace(int orig,int new, global_game_variables_t *gvar)
{
	memptr			addr;
	PageListStruct	far *origpage,far *newpage;

	if (orig == new)
		Quit("PML_TransferPageSpace: Identity replacement");

	origpage = &gvar->pm.PMPages[orig];
	newpage = &gvar->pm.PMPages[new];

	if (origpage->locked != pml_Unlocked)
		Quit("PML_TransferPageSpace: Killing locked page");

	if ((origpage->emsPage == -1) && (origpage->mainPage == -1))
		Quit("PML_TransferPageSpace: Reusing non-existent page");

	// Copy page that's about to be purged into XMS
	PML_PutPageInXMS(orig, gvar);

	// Get the address, and force EMS into a physical page if necessary
	addr = PM_GetPageAddress(orig, gvar);

	// Steal the address
	newpage->emsPage = origpage->emsPage;
	newpage->mainPage = origpage->mainPage;

	// Mark replaced page as purged
	origpage->mainPage = origpage->emsPage = -1;

	if (!addr)
		Quit("PML_TransferPageSpace: Zero replacement");

	return(addr);
}

//
//	PML_GetAPageBuffer() - A page buffer is needed. Either get it from the
//		main/EMS free pool, or use PML_GiveLRUPage() to find which page to
//		steal the buffer from. Returns a far pointer to the page buffer, and
//		sets the fields inside the given page structure appropriately.
//		If mainonly is true, free EMS will be ignored, and only main pages
//		will be looked at by PML_GiveLRUPage().
//
byte far *
PML_GetAPageBuffer(int pagenum,boolean mainonly, global_game_variables_t *gvar)
{
	byte			far *addr = nil;
	int				i,n;
	PMBlockAttr		*used;
	PageListStruct	far *page;

	page = &gvar->pm.PMPages[pagenum];
	if ((gvar->pm.EMSPagesUsed < gvar->pm.emm.EMSPagesAvail) && !mainonly)
	{
		// There's remaining EMS - use it
		page->emsPage = gvar->pm.EMSPagesUsed++;
		addr = PML_GetEMSAddress(page->emsPage,page->locked, gvar);
	}
	else if (gvar->pm.MainPagesUsed < gvar->pm.mm.MainPagesAvail)
	{
		// There's remaining main memory - use it
		for (i = 0,n = -1,used = gvar->pm.mm.MainMemUsed;i < PMMaxMainMem;i++,used++)
		{
			if ((*used & pmba_Allocated) && !(*used & pmba_Used))
			{
				n = i;
				*used |= pmba_Used;
				break;
			}
		}
		if (n == -1)
			Quit("PML_GetPageBuffer: MainPagesAvail lied");
		addr = gvar->pm.mm.MainMemPages[n];
		if (!addr)
			Quit("PML_GetPageBuffer: Purged main block");
		page->mainPage = n;
		gvar->pm.MainPagesUsed++;
	}
	else
		addr = PML_TransferPageSpace(PML_GiveLRUPage(mainonly, gvar),pagenum, gvar);

	if (!addr)
		Quit("PML_GetPageBuffer: Search failed");
	return(addr);
}

//
//	PML_GetPageFromXMS() - If page is in XMS, find LRU main/EMS page and
//		replace it with the page from XMS. If mainonly is true, will only
//		search for LRU main page.
//	XMSProtectPage is set to the page to be retrieved from XMS, so that if
//		the page from which we're stealing the main/EMS from isn't in XMS,
//		it won't copy over the page that we're trying to get from XMS.
//		(pages that are being purged are copied into XMS, if possible)
//
memptr
PML_GetPageFromXMS(int pagenum,boolean mainonly, global_game_variables_t *gvar)
{
	byte			far *checkaddr;
	memptr			addr = nil;
	PageListStruct	far *page;

	page = &gvar->pm.PMPages[pagenum];
	if (gvar->pm.xmm.XMSPresent && (page->xmsPage != -1))
	{
		gvar->pm.xmm.XMSProtectPage = pagenum;
		checkaddr = PML_GetAPageBuffer(pagenum,mainonly, gvar);
		if (FP_OFF(checkaddr))
			Quit("PML_GetPageFromXMS: Non segment pointer");
		addr = (memptr)FP_SEG(checkaddr);
		PML_CopyFromXMS(addr,page->xmsPage,page->length, gvar);
		gvar->pm.xmm.XMSProtectPage = -1;
	}

	return(addr);
}

//
//	PML_LoadPage() - A page is not in main/EMS memory, and it's not in XMS.
//		Load it into either main or EMS. If mainonly is true, the page will
//		only be loaded into main.
//
void
PML_LoadPage(int pagenum,boolean mainonly, global_game_variables_t *gvar)
{
	byte			far *addr;
	PageListStruct	far *page;

	addr = PML_GetAPageBuffer(pagenum,mainonly, gvar);
	page = &gvar->pm.PMPages[pagenum];
	PML_ReadFromFile(addr,page->offset,page->length, gvar);
}

//
//	PM_GetPage() - Returns the address of the page, loading it if necessary
//		First, check if in Main Memory or EMS
//		Then, check XMS
//		If not in XMS, load into Main Memory or EMS
//
#pragma warn -pia
memptr
PM_GetPage(int pagenum, global_game_variables_t *gvar)
{
	memptr	result;

	if (pagenum >= gvar->pm.fi.ChunksInFile)
		Quit("PM_GetPage: Invalid page request");

#ifdef __DEBUG_2__	// for debugging
	__asm {
		mov	dx,STATUS_REGISTER_1
		in	al,dx
		mov	dx,ATR_INDEX
		mov	al,ATR_OVERSCAN
		out	dx,al
		mov	al,10	// bright green
		out	dx,al
	}
#endif

	if (!(result = PM_GetPageAddress(pagenum, gvar)))
	{
		boolean mainonly = (pagenum >= gvar->pm.fi.PMSoundStart);
if (!gvar->pm.PMPages[pagenum].offset)	// JDC: sparse page
	Quit ("Tried to load a sparse page!");
		if (!(result = PML_GetPageFromXMS(pagenum,mainonly, gvar)))
		{
			if (gvar->pm.PMPages[pagenum].lastHit ==  gvar->pm.PMFrameCount)
				gvar->pm.PMThrashing++;

			PML_LoadPage(pagenum,mainonly, gvar);
			result = PM_GetPageAddress(pagenum, gvar);
		}
	}
	gvar->pm.PMPages[pagenum].lastHit =  gvar->pm.PMFrameCount;

#ifdef __DEBUG_2__	// for debugging
	__asm{
		mov	dx,STATUS_REGISTER_1
		in	al,dx
		mov	dx,ATR_INDEX
		mov	al,ATR_OVERSCAN
		out	dx,al
		mov	al,3	// blue
		out	dx,al
		mov	al,0x20	// normal
		out	dx,al
#endif

	return(result);
}
#pragma warn +pia

//
//	PM_SetPageLock() - Sets the lock type on a given page
//		pml_Unlocked: Normal, page can be purged
//		pml_Locked: Cannot be purged
//		pml_EMS?: Same as pml_Locked, but if in EMS, use the physical page
//					specified when returning the address. For sound stuff.
//
void
PM_SetPageLock(int pagenum,PMLockType lock, global_game_variables_t *gvar)
{
	if (pagenum < gvar->pm.fi.PMSoundStart)
		Quit("PM_SetPageLock: Locking/unlocking non-sound page");

	gvar->pm.PMPages[pagenum].locked = lock;
}

//
//	PM_Preload() - Loads as many pages as possible into all types of memory.
//		Calls the update function after each load, indicating the current
//		page, and the total pages that need to be loaded (for thermometer).
//
void
PM_Preload(boolean (*update)(word current,word total), global_game_variables_t *gvar)
{
	int				i,j,
					page,oogypage;
	word			current,total,
					totalnonxms,totalxms,
					mainfree,maintotal,
					emsfree,emstotal,
					xmsfree,xmstotal;
	memptr			addr;
	PageListStruct	far *p;

	mainfree = (gvar->pm.mm.MainPagesAvail - gvar->pm.MainPagesUsed) + (gvar->pm.emm.EMSPagesAvail - gvar->pm.EMSPagesUsed);
	xmsfree = (gvar->pm.xmm.XMSPagesAvail - gvar->pm.XMSPagesUsed);

	xmstotal = maintotal = 0;

	for (i = 0;i < gvar->pm.fi.ChunksInFile;i++)
	{
		if (!gvar->pm.PMPages[i].offset)
			continue;			// sparse

		if ( gvar->pm.PMPages[i].emsPage != -1 || gvar->pm.PMPages[i].mainPage != -1 )
			continue;			// already in main mem

		if ( mainfree )
		{
			maintotal++;
			mainfree--;
		}
		else if ( xmsfree && (gvar->pm.PMPages[i].xmsPage == -1) )
		{
			xmstotal++;
			xmsfree--;
		}
	}


	total = maintotal + xmstotal;

	if (!total)
		return;

	page = 0;
	current = 0;

//
// cache main/ems blocks
//
	while (maintotal)
	{
		while ( !gvar->pm.PMPages[page].offset || gvar->pm.PMPages[page].mainPage != -1
			||	gvar->pm.PMPages[page].emsPage != -1 )
			page++;

		if (page >= gvar->pm.fi.ChunksInFile)
			Quit ("PM_Preload: Pages>=gvar->pm.fi.ChunksInFile");

		PM_GetPage(page, gvar);

		page++;
		current++;
		maintotal--;
		update(current,total);
	}

//
// load stuff to XMS
//
	if (xmstotal)
	{
		for (oogypage = 0 ; gvar->pm.PMPages[oogypage].mainPage == -1 ; oogypage++)
		;
		addr = PM_GetPage(oogypage, gvar);
		if (!addr)
			Quit("PM_Preload: XMS buffer failed");

		while (xmstotal)
		{
			while ( !gvar->pm.PMPages[page].offset || gvar->pm.PMPages[page].xmsPage != -1 )
				page++;

			if (page >= gvar->pm.fi.ChunksInFile)
				Quit ("PM_Preload: Pages>=gvar->pm.fi.ChunksInFile");

			p = &gvar->pm.PMPages[page];

			p->xmsPage = gvar->pm.XMSPagesUsed++;
			if (gvar->pm.XMSPagesUsed > gvar->pm.xmm.XMSPagesAvail)
				Quit("PM_Preload: Exceeded XMS pages");
			if (p->length > PMPageSize)
				Quit("PM_Preload: Page too long");

			PML_ReadFromFile((byte far *)addr,p->offset,p->length, gvar);
			PML_CopyToXMS((byte far *)addr,p->xmsPage,p->length, gvar);

			page++;
			current++;
			xmstotal--;
			update(current,total);
		}

		p = &gvar->pm.PMPages[oogypage];
		PML_ReadFromFile((byte far *)addr,p->offset,p->length, gvar);
	}

	update(total,total);
}

/////////////////////////////////////////////////////////////////////////////
//
//	General code
//
/////////////////////////////////////////////////////////////////////////////

//
//	PM_NextFrame() - Increments the frame counter and adjusts the thrash
//		avoidence variables
//
//		If currently in panic mode (to avoid thrashing), check to see if the
//			appropriate number of frames have passed since the last time that
//			we would have thrashed. If so, take us out of panic mode.
//
//
void
PM_NextFrame(global_game_variables_t *gvar)
{
	int	i;

	// Frame count overrun - kill the LRU hit entries & reset frame count
	if (++gvar->pm.PMFrameCount >= LONG_MAX - 4)
	{
		for (i = 0;i < gvar->pm.PMNumBlocks;i++)
			gvar->pm.PMPages[i].lastHit = 0;
		gvar->pm.PMFrameCount = 0;
	}

//#if 0
	for (i = 0;i < gvar->pm.fi.PMSoundStart;i++)
	{
		if (gvar->pm.PMPages[i].locked)
		{
			char buf[40];
			sprintf(buf,"PM_NextFrame: Page %d is locked",i);
			Quit(buf);
		}
	}
//#endif

	if (gvar->pm.PMPanicMode)
	{
		// DEBUG - set border color
		if ((!gvar->pm.PMThrashing) && (!--gvar->pm.PMPanicMode))
		{
			// DEBUG - reset border color
		}
	}
	if (gvar->pm.PMThrashing >= PMThrashThreshold)
		gvar->pm.PMPanicMode = PMUnThrashThreshold;
	gvar->pm.PMThrashing = false;
}

//
//	PM_Reset() - Sets up caching structures
//
void
PM_Reset(global_game_variables_t *gvar)
{
	int				i;
	PageListStruct	far *page;

	gvar->pm.xmm.XMSPagesAvail = gvar->pm.xmm.XMSAvail / PMPageSizeKB;

	gvar->pm.emm.EMSPagesAvail = gvar->pm.emm.EMSAvail * (EMSPageSizeKB / PMPageSizeKB);
	gvar->pm.emm.EMSPhysicalPage = 0;

	gvar->pm.MainPagesUsed = gvar->pm.EMSPagesUsed = gvar->pm.XMSPagesUsed = 0;

	gvar->pm.PMPanicMode = false;

	gvar->pm.fi.PageFile = -1;
	gvar->pm.xmm.XMSProtectPage = -1;

	// Initialize page list
	for (i = 0,page = gvar->pm.PMPages;i < gvar->pm.PMNumBlocks;i++,page++)
	{
		page->mainPage = -1;
		page->emsPage = -1;
		page->xmsPage = -1;
		page->locked = false;
	}
}

//
//	PM_Startup() - Start up the Page Mgr
//
void
PM_Startup(global_game_variables_t *gvar)
{
	boolean	nomain,noems,noxms;
	int		i;

	if (gvar->pm.PMStarted)
		return;

	//strcpy(&(gvar->pm.fi.PageFileName), "VSWAP.");

	nomain = noems = noxms = false;
	for (i = 1;i <
#ifdef __WATCOMC__
	__argc
#endif
#ifdef __BORLANDC__
	_argc
#endif
	;i++)
	{
		switch (US_CheckParm(
#ifdef __WATCOMC__
	__argv[i]
#endif
#ifdef __BORLANDC__
	_argv[i]
#endif
		,ParmStrings))
		{
		case 0:
			nomain = true;
			break;
		case 1:
			noems = true;
			break;
		case 2:
			noxms = true;
			break;
		}
	}

	//0000+=+=PML_OpenPageFile(gvar);

	if (!noems)
		PML_StartupEMS(gvar);
	if (!noxms)
		PML_StartupXMS(gvar);

	if (!nomain && !gvar->pm.emm.EMSPresent)
	{
		printf("PM_Startup: No main or EMS\n");
		exit(-5);
		//return;
	}
	PML_StartupMainMem(gvar);

	PM_Reset(gvar);

	gvar->pm.PMStarted = true;
}

//
//	PM_Shutdown() - Shut down the Page Mgr
//
void
PM_Shutdown(global_game_variables_t *gvar)
{
	PML_ShutdownXMS(gvar);
	PML_ShutdownEMS(gvar);

	if (!gvar->pm.PMStarted)
		return;

	PML_ClosePageFile(gvar);

	PML_ShutdownMainMem(gvar);
}
