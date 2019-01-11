/* Project 16 Source Code~
 * Copyright (C) 2012-2019 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

#ifndef __TESTPATT_H__
#define __TESTPATT_H__

////corner markers
#define GVPO gvar.video.page[0]
#define GVPI gvar.video.page[1]
#define OW	GVPO.width-16
#define OH	GVPO.height-16
#define OE	GVPO.sw
#define OJ	GVPO.sh
#define IW	GVPI.width-16
#define IH	GVPI.height-16
#define IE	GVPI.sw
#define IJ	GVPI.sh
#define DCBO modexClearRegion(&GVPO
#define DCBI modexClearRegion(&GVPI

#define DRAWOTHERCORNERBOX_TOPLEFT \
DCBO,	0,	0,	16, 16, 0); \
DCBO,	16,	16,	16, 16, 1); \
DCBI,		0,	0,	16, 16, 8); \
DCBI,		16,	16,	16, 16, 9);

#define DRAWOTHERCORNERBOX_TOPRIGHT \
DCBO,	OW,	0,	16, 16, 2); \
DCBO,	OE,	16,	16, 16, 3); \
DCBI,		IW,	0,	16, 16, 10); \
DCBI,		IE,	16,	16, 16, 11);

#define DRAWOTHERCORNERBOX_BOTTOMLEFT \
DCBO,	0,	OH,	16, 16, 4); \
DCBO,	16,	OJ,	16, 16, 5); \
DCBI,		0,	IH,	16, 16, 12); \
DCBI,		16,	IJ,	16, 16, 13);

#define DRAWOTHERCORNERBOX_BOTTOMRIGHT \
DCBO,	OE,	OJ,	16, 16, 6); \
DCBO,	OW,	OH,	16, 16, 7); \
DCBI,		IE,	IJ,	16, 16, 14);\
DCBI,		IW,	IH,	16, 16, 15);

#define DRAWCORNERBOXES \
DRAWOTHERCORNERBOX_TOPLEFT; \
DRAWOTHERCORNERBOX_TOPRIGHT; \
DRAWOTHERCORNERBOX_BOTTOMLEFT; \
DRAWOTHERCORNERBOX_BOTTOMRIGHT;

#define TESTBG1 \
	modexClearRegion(&gvar.video.page[0], 0, 0, gvar.video.page[0].width, gvar.video.page[0].height, 1); \
	modexClearRegion(&gvar.video.page[0], 16, 16, gvar.video.page[0].sw, gvar.video.page[0].sh, 2); \
	modexClearRegion(&gvar.video.page[0], 32, 32, gvar.video.page[0].sw-32, gvar.video.page[0].sh-32, 3); \
	modexClearRegion(&gvar.video.page[0], 48, 48, gvar.video.page[0].sw-64, gvar.video.page[0].sh-64, 2);

#define TESTBG2 \
	modexClearRegion(&gvar.video.page[1], 0, 0, gvar.video.page[1].width, gvar.video.page[1].height, 5); \
	modexClearRegion(&gvar.video.page[1], 16, 16, gvar.video.page[1].sw, gvar.video.page[1].sh, 6); \
	modexClearRegion(&gvar.video.page[1], 32, 32, gvar.video.page[1].sw-32, gvar.video.page[1].sh-32, 7); \
	modexClearRegion(&gvar.video.page[1], 48, 48, gvar.video.page[1].sw-64, gvar.video.page[1].sh-64, 6);

#define TESTBG12 \
	TESTBG1 \
	TESTBG2

#define TESTBG34 \
	modexClearRegion(&gvar.video.page[2], 0, 0, gvar.video.page[2].sw, gvar.video.page[2].sh, 45); \
	modexClearRegion(&gvar.video.page[3], 0, 0, gvar.video.page[3].sw, gvar.video.page[3].sh, 47);

#define TESTBG \
	TESTBG12 \
	TESTBG34

#endif
