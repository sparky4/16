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
/*
 * soundtest
 */

#include "src/lib/vgmsnd/vgmSnd.h"
#include "src/lib/16_snd.h"
//#include "src/lib/doslib/adlib.h"
#include "src/lib/16_tail.h"
#include "src/lib/16_pm.h"
#include "src/lib/16_ca.h"
#include "src/lib/16_mm.h"
#include "src/lib/16_hc.h"
#include "src/lib/16_dbg.h"

void OPL2_Write(UINT8 reg, UINT8 data);
UINT8 OPL2_ReadStatus(void);

void OPL2_Write(UINT8 reg, UINT8 data)
{
	//ym3812_w(0, 0, reg);
	//ym3812_w(0, 1, data);
	opl2out((word)reg, (word)data);
	return;
}

UINT8 OPL2_ReadStatus(void)
{
	return(inp(ADLIB_FM_ADDRESS));
	//return ym3812_r(0, 0);
}

void
main(int argc, char *argv[])
{
	static global_game_variables_t gvar;
	VGM_FILE pee[9];
	char bakapee[64] = "data/02.vgm";

	if(argv[1]) strcpy(bakapee, argv[1]);

//	MM_Startup(&gvar);
//	PM_Startup(&gvar); PM_UnlockMainMem(&gvar);
//	CA_Startup(&gvar);
	StartupCAMMPM(&gvar);
	printf("%x\n", OpenVGMFile(bakapee, &pee[0], &gvar));
	InitEngine();
	PlayMusic(&pee[0]);
	while(!kbhit())
	{
		UpdateSoundEngine();
	}
	StopMusic();
	FreeVGMFile(&pee[0], &gvar); printf("ok\n");
	DeinitEngine();
//	PM_Shutdown(&gvar);
//	CA_Shutdown(&gvar);
//	MM_Shutdown(&gvar);
	ShutdownCAMMPM(&gvar);
}
