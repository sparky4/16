/* Project 16 Source Code~
 * Copyright (C) 2012-2020 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

#ifndef _SRC_LIB_16_DBG
#define _SRC_LIB_16_DBG

//#include "src/lib/16_head.h"
#include "src/lib/16_pm.h"
//#include "src/lib/16_tdef.h"

#define __DEBUG__

#ifdef __DEBUG__
#define __DEBUG_InputMgr__
#define __DEBUG_MAP__
//#define __DEBUG_2__
//#define __DEBUG_CA__
//#define __DEBUG_PM__
//#define __DEBUG_MM__
#define __DEBUG_RF__
#define __DEBUG_SPRI__
#define __DEBUG_VL__
#endif


#ifdef __DEBUG__
#ifdef __DEBUG_MM__
extern boolean dbg_debugmm;
#endif
#ifdef __DEBUG_PM__
//#define __DEBUG_PM_MAIN__
extern boolean dbg_debugpm;
#endif
#ifdef __DEBUG_CA__
extern boolean dbg_debugca;
#endif
#ifdef __DEBUG_InputMgr__
extern boolean dbg_testkeyin,dbg_testcontrolnoisy,dbg_nointest,dbg_joymousedelta;
#endif
#ifdef __DEBUG_MAP__
extern boolean dbg_maptext;
extern byte *dbg_mapdata;
#endif
#ifdef __DEBUG_RF__
extern boolean dbg_pagenorendermap,dbg_pagedelayrendermap;
#endif
#ifdef __DEBUG_SPRI__
extern boolean dbg_delayanimation;
#endif
#endif

#ifdef __WATCOMC__
typedef struct
{
	word	palq,mult;
	word	palx, paly;
	byte	i, oi;
} spv_t;
# ifdef DEBUGSERIAL
#  include <hw/8250/8250.h>

void _DEBUGF(const char *fmt,...);
void _DEBUG(const char *msg);
int _DEBUG_INIT();
# else
static inline void _DEBUGF(const char *fmt,...) {
	// NOTHING
}
static inline void _DEBUG(const char *msg) {
	// NOTHING
}

static inline int _DEBUG_INIT() {
	// NOTHING
	return -1;
}
# endif
#endif //watcomc

void ShapeTest_(global_game_variables_t *gvar);
#ifdef __WATCOMC__
#ifdef __DEBUG_VL__
spv_t ShowPalVarSync (spv_t *spv);
void ShowPalVal (global_game_variables_t *gvar);
#endif
#endif

#endif // _SRC_LIB_16_DBG
