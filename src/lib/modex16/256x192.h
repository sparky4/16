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

#ifndef _DIX_H_
#define _DIX_H_

#include "src/lib/types.h"

static const word ModeX_256x224regs[75] =
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
};

#endif /*_DIX_H_*/
