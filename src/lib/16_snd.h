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

#ifndef __16_SND_H_
#define __16_SND_H_

#include "src/lib/16_head.h"

#define MIN_REGISTER			0x01
#define MAX_REGISTER			0xF5
#define ADLIB_FM_ADDRESS	0x388	/* adlib address/status register */
#define ADLIB_FM_DATA		0x389	/* adlib data register           */

void opl2out(word reg, word data);
void opl3out(word reg, word data);
void opl3exp(word data);
void FMReset(void/*int percusiveMode*/);

#endif /*__16_SND_H_*/
