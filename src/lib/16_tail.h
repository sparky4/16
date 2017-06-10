/* Project 16 Source Code~
 * Copyright (C) 2012-2017 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

#ifndef	__16_TAIL__
#define	__16_TAIL__

#include "src/lib/16_head.h"
#include "src/lib/16text.h"
#include "src/lib/16_pm.h"
#include "src/lib/16_mm.h"
#include "src/lib/16_ca.h"
#include "src/lib/16_in.h"
#include "src/lib/16_dbg.h"
#include "src/lib/16_vl.h"
#include "src/lib/testpatt.h"

#ifdef __WATCOMC__
#include <hw/cpu/cpu.h>
#include <hw/dos/dos.h>
#include <hw/vga/vga.h>
#include <hw/vga/vrl.h>
#include <hw/vga/vrs.h>
#endif

//gvar.video.ofs.bufferofs,gvar.video.ofs.displayofs,
#define FIZZLEFADEFUNCTION \
	if(gvar.in.inst->Keyboard[sc_F]){ FizzleFade(\
	atoi(gvar.video.page[0].data),atoi(gvar.video.page[1].data),\
320,240,true, &gvar);	IN_UserInput(1, &gvar); } \

#define TAIL_FUNCTIONKEYFUNCTIONS \
	if(gvar.in.inst->Keyboard[88]){ panswitch=!panswitch;							IN_UserInput(1, &gvar); } \
	TAIL_FUNCTIONKEYFUNCTIONS0EXE

#define RFDEBUGFUNCTIONS
#ifdef __DEBUG_RF__
#undef RFDEBUGFUNCTIONS
#define RFDEBUGFUNCTIONS \
	if(gvar.in.inst->Keyboard[sc_F11]){ dbg_pagedelayrendermap=!dbg_pagedelayrendermap;		IN_UserInput(1, &gvar); } \
	if(gvar.in.inst->Keyboard[sc_F9]){ dbg_pagenorendermap=!dbg_pagenorendermap;			IN_UserInput(1, &gvar); }
#endif

/*#define MAPDEBUGFUNCTIONS
#ifdef __DEBUG_MAP__
#undef MAPDEBUGFUNCTIONS
#define MAPDEBUGFUNCTIONS \
	if(gvar.in.inst->Keyboard[sc_F12]){ dbg_maptext!=dbg_maptext;						IN_UserInput(1, &gvar); }
#endif*/

/*#define SPRIDEBUGFUNCTIONS
#ifdef __DEBUG_SPRI__
#undef SPRIDEBUGFUNCTIONS
#define SPRIDEBUGFUNCTIONS \
	if(gvar.in.inst->Keyboard[sc_Y]){ dbg_delayanimation=!dbg_delayanimation;				IN_UserInput(1, &gvar); }
//	if(gvar.in.inst->Keyboard[sc_F9]){ dbg_pagenorendermap=!dbg_pagenorendermap;			IN_UserInput(1, &gvar); }
#endif*/

#define TAIL_FUNCTIONKEYFUNCTIONS0EXE \
	if(gvar.in.inst->Keyboard[sc_F4]){ turboXT(12);									IN_UserInput(1, &gvar); } \
	if(gvar.in.inst->Keyboard[68/*sc_F10*/]){ gvar.kurokku.fpscap=!gvar.kurokku.fpscap;			IN_UserInput(1, &gvar); } \
	if(gvar.in.inst->Keyboard[sc_F8]){ gvar.video.vga_state.bgps=!gvar.video.vga_state.bgps;		IN_UserInput(1, &gvar); } \
	if(gvar.in.inst->Keyboard[sc_F7]){ VL_ShowPage(&gvar.video.page, 0, 1);						IN_UserInput(1, &gvar); } \
	if(gvar.in.inst->Keyboard[sc_F6]){ VL_ShowPage(&gvar.video.page, 0, 0);						IN_UserInput(1, &gvar); } \
	if(gvar.in.inst->Keyboard[sc_T]){ gvar.video.vga_state.rss=!gvar.video.vga_state.rss;			IN_UserInput(1, &gvar); } \
	if(gvar.in.inst->Keyboard[sc_P]){ modexpdump(0, &gvar);							IN_UserInput(1, &gvar); } \
	if(gvar.in.inst->Keyboard[sc_Y]){ dbg_delayanimation=!dbg_delayanimation;				IN_UserInput(1, &gvar); } \
	if(gvar.in.inst->Keyboard[sc_Q]){ VL_modexPrintTextBox(&gvar);						IN_UserInput(1, &gvar); } \
	if(gvar.in.inst->Keyboard[sc_W]){ VL_MemToScreen((byte __far *)&gvar, 64, 64, 16, 16, &gvar);	IN_UserInput(1, &gvar); } \
	if(gvar.in.inst->Keyboard[sc_LBrkt]){ ShowPalVal (&gvar);							IN_UserInput(1, &gvar); } \
	if(gvar.in.inst->Keyboard[sc_M]){ MM_ShowMemory(&gvar);	MM_DumpData (&gvar);			IN_UserInput(1, &gvar); } \
	RFDEBUGFUNCTIONS
//FIZZLEFADEFUNCTION

#define TAIL_PANKEYFUN \
	TAIL_PANKEYFUNZC \
	TAIL_FUNCTIONKEYDRAWJUNK

#define TAIL_PANKEYFUNZC \
	ZC_panPageManual(&gvar.mv, &gvar.player, 0);

#define FUNCTIONKEYSHOWMV \
	if(gvar.in.inst->Keyboard[1+1]){ gvar.video.sp=0; VL_ShowPage(&gvar.video.page, 0, 0); } \
	if(gvar.in.inst->Keyboard[2+1]){ gvar.video.sp=1; VL_ShowPage(&gvar.video.page, 0, 0); } \
	if(gvar.in.inst->Keyboard[3+1]){ gvar.video.sp=2; VL_ShowPage(&gvar.video.page, 0, 1); } \
	if(gvar.in.inst->Keyboard[4+1]){ gvar.video.sp=3; VL_ShowPage(&gvar.video.page, 0, 1); }

#define TAIL_FUNCTIONKEYDRAWJUNK \
	FUNCTIONKEYSHOWMV \
	TAIL_FUNCTIONKEYDRAWJUNKNOMV

#define TAIL_FUNCTIONKEYDRAWJUNKNOMV \
	if(gvar.in.inst->Keyboard[sc_A]){ modexClearRegion(&gvar.video.page[2], 0, 0, gvar.video.page[2].sw, gvar.video.page[2].sh, 3); } \
	if(gvar.in.inst->Keyboard[sc_S]){ modexClearRegion(&gvar.video.page[3], 0, 0, gvar.video.page[3].sw, gvar.video.page[3].sh, 4); } \
\
	if(gvar.in.inst->Keyboard[sc_Z]){ DRAWCORNERBOXES } \
	if(gvar.in.inst->Keyboard[sc_X]){ TESTBG12 } \
	if(gvar.in.inst->Keyboard[sc_C]){ TESTBG34 } \
	if(gvar.in.inst->Keyboard[sc_V]){ VL_PatternDraw(&gvar.video, 0, 1, 1); } \
	if(gvar.in.inst->Keyboard[sc_I]){ dbg_maptext=!dbg_maptext; IN_UserInput(1, &gvar); } \
	if(gvar.in.inst->Keyboard[sc_PgDn]){ \
		rotateR(gvar.video.palette, sizeof(gvar.video.palette)/sizeof(gvar.video.palette[0])); \
		VL_UpdatePaletteWrite(&gvar.video.palette, 0, 255, &gvar);		IN_UserInput(1, &gvar); } \
	if(gvar.in.inst->Keyboard[sc_PgUp]){ \
		rotateL(gvar.video.palette, sizeof(gvar.video.palette)/sizeof(gvar.video.palette[0])); \
		VL_UpdatePaletteWrite(&gvar.video.palette, 0, 255, &gvar);		IN_UserInput(1, &gvar); }
#ifdef __BORLANDC__
#define PAL_WRITE_REG		   0x03C8   /* Color register, write address */
#define PAL_DATA_REG			0x03C9   /* Color register, data port */
#endif

void	Shutdown16(global_game_variables_t *gvar),
	Startup16(global_game_variables_t *gvar),
	StartupCAMMPM (global_game_variables_t *gvar),
	ShutdownCAMMPM (global_game_variables_t *gvar);
void TL_DosLibStartup(global_game_variables_t *gvar);
void TL_VidInit(global_game_variables_t *gvar);
boolean FizzleFade (unsigned source, unsigned dest, unsigned width, unsigned height, unsigned frames, boolean abortable, global_game_variables_t *gvar);
void DebugMemory_(global_game_variables_t *gvar, boolean q);
void ClearMemory (global_game_variables_t *gvar);
void Quit (global_game_variables_t *gvar, char *error);
void turboXT(byte bakapee);
void nibbletest();
void booleantest();

#endif	/*__16_TAIL__ */
