/* Project 16 Source Code~
 * Copyright (C) 2012-2018 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

#ifndef __16_PM__
#define __16_PM__

//
//	ID_PM.H
//	Header file for Id Engine's Page Manager
//

#include "src/lib/16_head.h"
#include "src/lib/16_hc.h"
#include "src/lib/16_mm.h"
#include "src/lib/16_ca.h"
#include <dos.h>

//#define __PM__NOHOGEMS__
/*//	NOTE! PMPageSize must be an even divisor of EMSPageSize, and >= 1024
#define	EMSPageSize		16384
#define	EMSPageSizeSeg	(EMSPageSize >> 4)
#define	EMSPageSizeKB	(EMSPageSize >> 10)
#define	EMSFrameCount	4
#define	PMPageSize		4096
#define	PMPageSizeSeg	(PMPageSize >> 4)
#define	PMPageSizeKB	(PMPageSize >> 10)
#define	PMEMSSubPage	(EMSPageSize / PMPageSize)

#define	PMMinMainMem	10			// Min acceptable # of pages from main
#define	PMMaxMainMem	100			// Max number of pages in main memory

#define	PMThrashThreshold	1	// Number of page thrashes before panic mode
#define	PMUnThrashThreshold	5	// Number of non-thrashing frames before leaving panic mode

typedef	enum
		{
			pml_Unlocked,
			pml_Locked
		} PMLockType;

typedef	enum
		{
			pmba_Unused = 0,
			pmba_Used = 1,
			pmba_Allocated = 2
		} PMBlockAttr;

typedef	struct
		{
			dword	offset;		// Offset of chunk into file
			word		length;		// Length of the chunk

			int			xmsPage;	// If in XMS, (xmsPage * PMPageSize) gives offset into XMS handle

			PMLockType	locked;		// If set, this page can't be purged
			int			emsPage;	// If in EMS, logical page/offset into page
			int			mainPage;	// If in Main, index into handle array

			dword	lastHit;	// Last frame number of hit
		} PageListStruct;

typedef	struct
		{
			int			baseEMSPage;	// Base EMS page for this phys frame
			dword	lastHit;		// Last frame number of hit
		} EMSListStruct;

extern	boolean			XMSPresent,EMSPresent;
extern	word			XMSPagesAvail,EMSPagesAvail;

extern	word			ChunksInFile,
						PMSpriteStart,PMSoundStart;
extern	PageListStruct	far *PMPages;*///moved to src/lib/typdefst.h

#define	PM_GetSoundPage(v)	PM_GetPage(gvar->pm.fi.PMSoundStart + (v), gvar)
#define	PM_GetSpritePage(v)	PM_GetPage(gvar->pm.fi.PMSpriteStart + (v), gvar)

#define	PM_LockMainMem(gvar)	PM_SetMainMemPurge(0, gvar)
#define	PM_UnlockMainMem(gvar)	PM_SetMainMemPurge(3, gvar)


extern	char	PageFileName[13];


extern	void	PM_Startup(global_game_variables_t *gvar),
				PM_Shutdown(global_game_variables_t *gvar),
				PM_Reset(global_game_variables_t *gvar),
				PM_Preload(boolean (*update)(word current,word total), global_game_variables_t *gvar),
				PM_NextFrame(global_game_variables_t *gvar),
				PM_SetPageLock(int pagenum,PMLockType lock, global_game_variables_t *gvar),
				PM_SetMainPurge(int level),
				PM_CheckMainMem(global_game_variables_t *gvar);
extern	memptr	PM_GetPageAddress(int pagenum, global_game_variables_t *gvar),
				PM_GetPage(int pagenum, global_game_variables_t *gvar);		// Use this one to cache page

void PM_SetMainMemPurge(int level, global_game_variables_t *gvar);
void PML_StartupMainMem(global_game_variables_t *gvar);
#endif
