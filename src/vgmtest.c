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

#include "src/lib/vgmsnd/vgmSnd.h"

/*
 * soundtest
 */

#define VGM_SLOTS	4

VGM_FILE vgmFiles[VGM_SLOTS];

void main()
{
	InitEngine(void);
	global_game_variables_t gvar;
	VGM_FILE* tempVgmFile;
	UINT8 vgmChn;
	UINT8 vgmId;

	tempVgmFile = &vgmFiles[vgmId];

	if (vgmChn == 0x7F)
		PlayMusic(tempVgmFile);
	else
		PlaySFX(tempVgmFile, vgmChn);
	DeinitEngine(void);
}
