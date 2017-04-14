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
/*
 * BAKAPEE!
 */
#ifndef __BAKAPI_H__
#define __BAKAPI_H__

#include "src/lib/bakapee.h"
#include "src/lib/wcpu/16_wcpu.h"

//project 16 testing define switch for veiwing the contents of the video memory --sparky4
#define BOINK // what does this mean? --J.C.
//#define BDOFSCONV (unsigned __near) defed in 16_tdef
#define BOFSS	BDOFSCONV (gvar.video.BOFS)//gvar.video.ofs.bufferofs
#define DOFSS	BDOFSCONV (gvar.video.DOFS)//gvar.video.ofs.displayofs

#define SETUPPAGEBAKAPI \
gvar.video.page[0] = modexDefaultPage(&gvar.video.page[0], &gvar.video); \
gvar.video.page[1] = modexNextPage(&gvar.video.page[0]);

#ifdef BAKAFIZZUNSIGNED
#define FIZZFADEFUN baka_FizzleFade (BOFSS, DOFSS, gvar.video.page[0].width, gvar.video.page[0].height, 70, true, &gvar);
#else
#define FIZZFADEFUN baka_FizzleFade (&gvar.video.page[1], &gvar.video.page[0], vga_state.vga_width, vga_state.vga_height, 70, true, &gvar);
#endif

#define BAKAPIINITFIZZTEST \
	modexClearRegion(&(gvar.video.page[0]), 0, 0, gvar.video.page[0].width, gvar.video.page[0].height, 0); \
	modexClearRegion(&(gvar.video.page[1]), 0, 0, gvar.video.page[0].width, gvar.video.page[0].height, 14); \
	FIZZFADEFUN
//{ word w; for(w=0;w<(gvar.video.page[0].width*gvar.video.page[0].height);w++) { ding(&gvar.video.page[0], &bakapee, 2); }

#endif /*__BAKAPI_H__*/
