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
	gv->kurokku.t = clock();
	gv->kurokku.tiku = 0;
	gv->kurokku.clock_start = *clockw;
	gv->kurokku.clock = clockw;
	gv->kurokku.frames_per_second = 60;
	gv->pee = _nmalloc(sizeof(byte)*2);
	//turn this off if XT
	switch(detectcpu())
	{
		case 0:
			gv->kurokku.fpscap=0;
		break;
		default:
			gv->kurokku.fpscap=1;
		break;
	}
	return gv->kurokku.t;
}

double elapsed_timer(global_game_variables_t *gv)
{
	return (clock() - gv->kurokku.t) / CLOCKS_PER_SEC;
}

double ticktock(global_game_variables_t *gv)
{
	double clocku;
	clocku = (clock() - gv->kurokku.t) / CLOCKS_PER_SEC;
	gv->kurokku.t = clock();
	return clocku;
}

double time_in_seconds(global_game_variables_t *gv)
{
	return (gv->kurokku.t) / CLOCKS_PER_SEC;
}

/*double time_in_seconds(time_t in_t)
{
	return (in_t) / CLOCKS_PER_SEC;
}*/
