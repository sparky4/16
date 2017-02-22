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

#define FUNCTIONKEYFUNCTIONS \
	if(IN_KeyDown(88)){ panswitch=!panswitch;							IN_UserInput(1,1); } \
	FUNCTIONKEYFUNCTIONS0EXE

#define FUNCTIONKEYFUNCTIONS0EXE \
	if(IN_KeyDown(sc_F4)){ turboXT(12);									IN_UserInput(1,1); } \
	if(IN_KeyDown(87/*sc_F11*/)){ pagedelayrendermap=!pagedelayrendermap;		IN_UserInput(1,1); } \
	if(IN_KeyDown(68/*sc_F10*/)){ gvar.kurokku.fpscap=!gvar.kurokku.fpscap;		IN_UserInput(1,1); } \
	if(IN_KeyDown(sc_F9)){ pagenorendermap=!pagenorendermap;				IN_UserInput(1,1); } \
	if(IN_KeyDown(sc_F8)){ gvar.video.bgps=!gvar.video.bgps;					IN_UserInput(1,1); } \
	if(IN_KeyDown(sc_F7)){ ZC_ShowMV(&mv, 0, 1);							IN_UserInput(1,1); } \
	if(IN_KeyDown(sc_F6)){ ZC_ShowMV(&mv, 0, 0);							IN_UserInput(1,1); } \
	if(IN_KeyDown(sc_T)){ gvar.video.rss=!gvar.video.rss;						IN_UserInput(1,1); } \
	if(IN_KeyDown(sc_P)){ modexpdump(&gvar.video.page[0]);					IN_UserInput(1,1); }

#define PANKEYFUN \
	PANKEYFUNZC \
	FUNCTIONKEYDRAWJUNK

#define PANKEYFUNZC \
	ZC_panPageManual(&mv, &player, 0);

#define FUNCTIONKEYSHOWMV \
	if(IN_KeyDown(1+1)){ gvar.video.sp=0; ZC_ShowMV(&mv, 0, 0); } \
	if(IN_KeyDown(2+1)){ gvar.video.sp=1; ZC_ShowMV(&mv, 0, 0); } \
	if(IN_KeyDown(3+1)){ gvar.video.sp=2; ZC_ShowMV(&mv, 0, 1); } \
	if(IN_KeyDown(4+1)){ gvar.video.sp=3; ZC_ShowMV(&mv, 0, 1); } \

#define FUNCTIONKEYDRAWJUNK \
	FUNCTIONKEYSHOWMV \
	FUNCTIONKEYDRAWJUNKNOMV

#define FUNCTIONKEYDRAWJUNKNOMV \
	if(IN_KeyDown(sc_A)) modexClearRegion(&gvar.video.page[2], 0, 0, gvar.video.page[2].sw, gvar.video.page[2].sh, 3); \
	if(IN_KeyDown(sc_S)) modexClearRegion(&gvar.video.page[3], 0, 0, gvar.video.page[3].sw, gvar.video.page[3].sh, 4); \
\
	if(IN_KeyDown(sc_Z)){ DRAWCORNERBOXES } \
	if(IN_KeyDown(sc_X)){ TESTBG12 } \
	if(IN_KeyDown(sc_C)){ TESTBG34 } \
	if(IN_KeyDown(sc_V)) VL_PatternDraw(&gvar.video, 0, 1, 1); \
	if(IN_KeyDown(sc_I)){ dbg_maptext=!dbg_maptext; IN_UserInput(1,1); }
/*	if(IN_KeyDown(sc_PgDn)){ \
		rotateR(gvar.video.palette, sizeof(gvar.video.palette)/sizeof(gvar.video.palette[0])); \
		VL_UpdatePaletteWrite(&gvar.video.palette, 0);		IN_UserInput(1,1); } \
	if(IN_KeyDown(sc_PgUp)){ \
		rotateL(gvar.video.palette, sizeof(gvar.video.palette)/sizeof(gvar.video.palette[0])); \
		VL_UpdatePaletteWrite(&gvar.video.palette, 0);		IN_UserInput(1,1); }*/

void DebugMemory_(global_game_variables_t *gvar, boolean q);
void Shutdown16(global_game_variables_t *gvar);
void Startup16(global_game_variables_t *gvar);
void ClearMemory (global_game_variables_t *gvar);
void Quit (global_game_variables_t *gvar, char *error);
void turboXT(byte bakapee);
void nibbletest();
void booleantest();

#endif	/*__16_TAIL__ */
