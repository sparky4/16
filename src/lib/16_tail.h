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

#ifndef	__16_TAIL__
#define	__16_TAIL__

#include "src/lib/16_head.h"
#include "src/lib/16text.h"
#include "src/lib/16_pm.h"
#include "src/lib/16_mm.h"
#include "src/lib/16_ca.h"
#include "src/lib/16_in.h"

#define FUNCTIONKEYFUNCTIONS \
	if(IN_KeyDown(sc_F6)){ modexShowPage(&gvar.video.page[0]); IN_UserInput(1,1); } \
	if(IN_KeyDown(sc_F7)){ modexShowPage_(&gvar.video.page[0]); IN_UserInput(1,1); }

void DebugMemory_(global_game_variables_t *gvar, boolean q);
void Shutdown16(global_game_variables_t *gvar);
void Startup16(global_game_variables_t *gvar);
void ClearMemory (global_game_variables_t *gvar);
void Quit (global_game_variables_t *gvar, char *error);
void nibbletest();
void booleantest();

#endif	/*__16_TAIL__ */
