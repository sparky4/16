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

#ifndef __RENDER_H__
#define __RENDER_H__

#include "src/lib/modex16.h"

void modexDrawPBufRegion(page_t *page, int x, int y, int rx, int ry, int rw, int rh, planar_buf_t *p, boolean sprite);
void modexDrawPBuf(page_t *page, int x, int y, planar_buf_t *p, boolean sprite); /*temp*/
void oldDrawBmp(byte far* page, int x, int y, bitmap_t *bmp, byte sprite);
void modexDrawBmp(page_t *page, int x, int y, bitmap_t *bmp);
void modexDrawBmpRegion(page_t *page, int x, int y, int rx, int ry, int rw, int rh, bitmap_t *bmp);
void modexDrawSprite(page_t *page, int x, int y, bitmap_t *bmp);
void modexDrawSpriteRegion(page_t *page, int x, int y, int rx, int ry, int rw, int rh, bitmap_t *bmp);
void modexDrawBmpPBuf(page_t *page, int x, int y, planar_buf_t *bmp); /*pbuf version*/
void modexDrawBmpPBufRegion(page_t *page, int x, int y, int rx, int ry, int rw, int rh, planar_buf_t *bmp);
void modexDrawSpritePBuf(page_t *page, int x, int y, planar_buf_t *bmp);
void modexDrawSpritePBufRegion(page_t *page, int x, int y, int rx, int ry, int rw, int rh, planar_buf_t *bmp);

#endif
