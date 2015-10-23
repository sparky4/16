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
/*
 * Functions and types for a planar image buffer.
 * This is meant to be able to load into video memory faster.
 */
#ifndef PLANAR_H
#define PLANAR_H

#include "src/lib/bitmap.h"
#include "src/lib/16_ca.h"
#include "src/lib/16_mm.h"
#include "src/lib/16_hc.h"

typedef struct {
	byte huge *plane[4];     /* 4 planes of image data */
	word width;         /* width of the image (spread across 4 planes) */
	word height;        /* height of the image (spread across 4 planes) */
	word pwidth;        /* the number of bytes in each plane */
} planar_buf_t;

/* creates a planar buffer from the bitmap data.
   The planar buffer is dynamically allocated, and should
   be destroyed with the planar_buf_free function when no longer
   needed.
 */
planar_buf_t *planar_buf_from_bitmap(bitmap_t *b);

/* allocates a planar buffer with specified dimensions */
planar_buf_t *planar_buf_alloc(word width, word height);

/* deallocates a planar buffer */
void planar_buf_free(planar_buf_t *p);
#endif
