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

#include "src/lib/16_sd.h"
#include "src/lib/16_tail.h"

void main(int argc,char **argv)
{
	ScanCode scancode;
	static global_game_variables_t gvar;
	StartupCAMMPM(&gvar);
	if(!CA_LoadFile(argv[1], MEMPTRCONV gvar.ca.audiosegs[0], &gvar))
	{
		printf("Failed to load IMF Music\n");
		ShutdownCAMMPM(&gvar);
		return;
	}
	IN_Startup(&gvar);
	printf("start\n");
	SD_Startup(&gvar);
	PL_StartMusic (&gvar);
	while (!(scancode = gvar.in.inst->LastScan)){}
	printf("shutdown\n");
	PL_StopMusic(&gvar);
	SD_Shutdown(&gvar);
	IN_Shutdown(&gvar);
	ShutdownCAMMPM(&gvar);
}
