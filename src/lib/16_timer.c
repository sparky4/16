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

#include "src/lib/16_timer.h"

clock_t start_timer(global_game_variables_t *gv)
{
	gv->kurokku.t = clock();
	gv->kurokku.tiku = 0;
	gv->kurokku.clock_start = *clockw;
	gv->kurokku.clock = clockw;
	//gv->kurokku.frames_per_second = 60;

    //turn this off if XT
	switch(detectcpu())
	{
		case 0:
			gv->kurokku.fpscap=0;
			gv->kurokku.frames_per_second=1;
		break;
		default:
			gv->kurokku.fpscap=1;
			gv->kurokku.frames_per_second=60;
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

// big global status text buffer
char global_temp_status_text[512];

/*double time_in_seconds(time_t in_t)
{
	return (in_t) / CLOCKS_PER_SEC;
}*/

/*	sync	*/
void shinkutxt(global_game_variables_t *gv)
{
	//float t;
	if(elapsed_timer(gv) >= (1.0 / gv->kurokku.frames_per_second))
	{
		//t=(((*(gv->clock))-gv->clock_start) /18.2);
		sprintf(global_temp_status_text, "%.0f fps", (double)gv->kurokku.tiku/ticktock(gv));
		fprintf(stderr, "%s\n", global_temp_status_text);
		//(gv->clock_start)=*(gv->clock);
		gv->kurokku.tiku=0;
	}
	gv->kurokku.tiku++;
// 	switch(gv->kurokku.fpscap)
// 	{
// 		case 0:
// 			gv->kurokku.frames_per_second=1;
// 		break;
// 		case 1:
// 			//turn this off if XT
// 			WaitPee();
// 			gv->kurokku.frames_per_second=60;
// 		break;
// 	}
}

void WaitPee()
{
    while(inp(INPUT_STATUS_1)  & 8)  {
	/* spin */
    }

    while(!(inp(INPUT_STATUS_1)  & 8))  {
	/* spin */
    }
}
