/* Project 16 Source Code~
 * Copyright (C) 2012-2022 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

#ifndef _TIMER_H_
#define _TIMER_H_
#include "src/lib/16_head.h"
#include "src/lib/wcpu/16_wcpu.h"

#define FPSVALUE 30

clock_t start_timer(global_game_variables_t *gv);
double elapsed_timer(global_game_variables_t *gv);
double ticktock(global_game_variables_t *gv);
double time_in_seconds(global_game_variables_t *gv);
//double time_in_seconds(time_t in_t);
//void shinkutxt(global_game_variables_t *gv);
//void WaitPee(void);

#endif
