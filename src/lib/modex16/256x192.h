/* Project 16 Source Code~
 * Copyright (C) 2012-2016 sparky4 & pngwen & andrius4669 & joncampbell123
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

#ifndef _DIX_H_
#define _DIX_H_

#include "src/lib/types.h"

/*static const word ModeX_256x224regs[75] =
{
	0x3c2, 0x00, 0xe3,
	0x3d4, 0x00, 0x5f,
	0x3d4, 0x01, 0x3f,
	0x3d4, 0x02, 0x40,
	0x3d4, 0x03, 0x82,
	0x3d4, 0x04, 0x4a,
	0x3d4, 0x05, 0x9a,
	0x3d4, 0x06, 0x0b,
	0x3d4, 0x07, 0x3e,
	0x3d4, 0x08, 0x00,
	0x3d4, 0x09, 0x41,
	0x3d4, 0x10, 0xda,
	0x3d4, 0x11, 0x9c,
	0x3d4, 0x12, 0xbf,
	0x3d4, 0x13, 0x20,
	0x3d4, 0x14, 0x00,
	0x3d4, 0x15, 0xc7,
	0x3d4, 0x16, 0x04,
	0x3d4, 0x17, 0xe3,
	0x3c4, 0x01, 0x01,
	0x3c4, 0x04, 0x06,
	0x3ce, 0x05, 0x40,
	0x3ce, 0x06, 0x05,
	0x3c0, 0x10, 0x41,
	0x3c0, 0x13, 0x00
};*/

static const word ModeX_256x192regs[] = {
	0x3200,		/* Horizontal total */
	0x2701,		/* horizontal display enable end */
	0x2802,		/* Start horizontal blanking */
	0x2003,		/* End horizontal blanking */
	0x2b04,		/* Start horizontal retrace */
	0x7005,		/* End horizontal retrace */
	0x0d06,		 /* vertical total */
	0x3e07,		 /* overflow (bit 8 of vertical counts) */
	0x4309,		 /* cell height (2 to double-scan */
	0xea10,		 /* v sync start */
	0xac11,		 /* v sync end and protect cr0-cr7 */
	0xdf12,		 /* vertical displayed */
	0x1413,		/* offset/logical width */
	0x0014,		 /* turn off dword mode */
	0xe715,		 /* v blank start */
	0x0616,		 /* v blank end */
	0xe317		  /* turn on byte mode */
};

#endif /*_DIX_H_*/
/*
voidtg::mode160x120(){
	int crtc11;	outp(CRTC_INDEX, 0x11); // unlock crtc
	crtc11 = inp(CRTC_DATA) & 0x7f;
	outp(CRTC_INDEX, 0x11);
	outp(CRTC_DATA, crtc11);
	width   = 160;
	height  = 120;
	maxx	= 159;
	maxy	= 119;
	pages   = 13;
	lineSize = 40;
	pageSize = 19200;
	modeName = "160x120";
	outp(MISC_OUTPUT, 0xe3);   // mor

	outp(CRTC_INDEX, 0x00); outp(CRTC_DATA, 0x32);
	outp(CRTC_INDEX, 0x01); outp(CRTC_DATA, 0x27);
	outp(CRTC_INDEX, 0x02); outp(CRTC_DATA, 0x28);
	outp(CRTC_INDEX, 0x03); outp(CRTC_DATA, 0x20);
	outp(CRTC_INDEX, 0x04); outp(CRTC_DATA, 0x2b);
	outp(CRTC_INDEX, 0x05); outp(CRTC_DATA, 0x70);
	outp(CRTC_INDEX, 0x06); outp(CRTC_DATA, 0x0d);
	outp(CRTC_INDEX, 0x07); outp(CRTC_DATA, 0x3e);
	outp(CRTC_INDEX, 0x08); outp(CRTC_DATA, 0x00);
	outp(CRTC_INDEX, 0x09); outp(CRTC_DATA, 0x43);
	outp(CRTC_INDEX, 0x10); outp(CRTC_DATA, 0xea);
	outp(CRTC_INDEX, 0x11); outp(CRTC_DATA, 0xac);
	outp(CRTC_INDEX, 0x12); outp(CRTC_DATA, 0xdf);
	outp(CRTC_INDEX, 0x13); outp(CRTC_DATA, 0x14);
	outp(CRTC_INDEX, 0x14); outp(CRTC_DATA, 0x00);
	outp(CRTC_INDEX, 0x15); outp(CRTC_DATA, 0xe7);
	outp(CRTC_INDEX, 0x16); outp(CRTC_DATA, 0x06);
	outp(CRTC_INDEX, 0x17); outp(CRTC_DATA, 0xe3);

	outp(0xSC_INDEX, 0x01); // seq
	outp(0xSC_DATA, 0x01);
	outp(0xSC_INDEX, 0x03); // seq
	outp(0xSC_DATA, 0x00);
	outp(0xSC_INDEX, 0x04); // seq
	outp(0xSC_DATA, 0x06);

	outp(GC_INDEX, 0x05); // gcr
	outp(0x3cf, 0x40);
	outp(GC_INDEX, 0x06); // gcr
	outp(0x3cf, 0x05);

	inp(0x3da);		  // acr
	outp(0x3c0, 0x10 | 0x20);
	outp(0x3c0, 0x41);
	inp(0x3da);		  // acr
	outp(0x3c0, 0x11 | 0x20);
	outp(0x3c0, 0x00);
	inp(0x3da);		  // acr
	outp(0x3c0, 0x12 | 0x20);
	outp(0x3c0, 0x0f);
	inp(0x3da);		  // acr
	outp(0x3c0, 0x13 | 0x20);
	outp(0x3c0, 0x00);
	inp(0x3da);		  // acr
	outp(0x3c0, 0x14 | 0x20);
	outp(0x3c0, 0x00);

	outp(CRTC_INDEX, 0x11); // lock crtc
	crtc11 = inp(CRTC_DATA) | 0x80;
	outp(CRTC_INDEX, 0x11);
	outp(CRTC_DATA, crtc11);
}
*/
