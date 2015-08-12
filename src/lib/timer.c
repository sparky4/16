/* Project 16 Source Code~
 * Copyright (C) 2012-2015 sparky4 & pngwen & andrius4669
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

#include "src/lib/timer.h"

clock_t start_timer(global_game_variables_t *gv)
{
	//time(&(this->t));
	gv->t = clock();
	gv->tiku = 0;
	gv->clock = (word far*) 0x046C; /* 18.2hz clock */
	return gv->t;
}

double elapsed_timer(global_game_variables_t *gv)
{
	return (clock() - gv->t) / CLOCKS_PER_SEC;
}

/*double time_in_seconds(global_game_variables_t *gv)
{
	return (gv->t) / CLOCKS_PER_SEC;
}*/

double time_in_seconds(time_t in_t)
{
	return (in_t) / CLOCKS_PER_SEC;
}
