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

#ifndef _UNOX_H_
#define _UNOX_H_

#include "src/lib/types.h"

/*static const word ModeX_320x200regs[75] =
{
	0x3c2, 0x00, 0x63,
	0x3d4, 0x00, 0x5f,
	0x3d4, 0x01, 0x4f,
	0x3d4, 0x02, 0x50,
	0x3d4, 0x03, 0x82,
	0x3d4, 0x04, 0x54,
	0x3d4, 0x05, 0x80,
	0x3d4, 0x06, 0xbf,
	0x3d4, 0x07, 0x1f,
	0x3d4, 0x08, 0x00,
	0x3d4, 0x09, 0x41,
	0x3d4, 0x10, 0x9c,
	0x3d4, 0x11, 0x8e,
	0x3d4, 0x12, 0x8f,
	0x3d4, 0x13, 0x28,
	0x3d4, 0x14, 0x00,
	0x3d4, 0x15, 0x96,
	0x3d4, 0x16, 0xb9,
	0x3d4, 0x17, 0xe3,
	0x3c4, 0x01, 0x01,
	0x3c4, 0x04, 0x06,
	0x3ce, 0x05, 0x40,
	0x3ce, 0x06, 0x05,
	0x3c0, 0x10, 0x41,
	0x3c0, 0x13, 0x00
};*/

static const word ModeX_320x200regs[] = {
	0x5f00,		/* Horizontal total */
	0x4f01,		/* horizontal display enable end */
	0x5002,		/* Start horizontal blanking */
	0x8203,		/* End horizontal blanking */
	0x5404,		/* Start horizontal retrace */
	0x8005,		/* End horizontal retrace */
	0x0d06,		 /* vertical total */
	0x3e07,		 /* overflow (bit 8 of vertical counts) */
	0x4109,		 /* cell height (2 to double-scan */
	0xea10,		 /* v sync start */
	0xac11,		 /* v sync end and protect cr0-cr7 */
	0xdf12,		 /* vertical displayed */
	0x2813,		/* offset/logical width */
	0x0014,		 /* turn off dword mode */
	0x9615,		 /* v blank start */
	0xb916,		 /* v blank end */
	0xe317		  /* turn on byte mode */
};

#endif /*_UNOX_H_*/
