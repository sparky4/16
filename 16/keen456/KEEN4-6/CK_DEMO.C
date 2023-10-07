/* Reconstructed Commander Keen 4-6 Source Code
 * Copyright (C) 2021 K1n9_Duk3
 *
 * This file is loosely based on:
 * Keen Dreams Source Code
 * Copyright (C) 2014 Javier M. Chavez
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "CK_DEF.H"

/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

boolean scorescreenkludge;

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

#if GRMODE == EGAGR

Uint8 starcolors[17] = STARPALETTE;
Uint16 plaquenum[4] = {IDSOFTPIC, PROGTEAMPIC, ARTISTPIC, DIRECTORPIC};
Uint8 termcolors[17] = INTROPALETTE;
Uint8 termcolors2[17] = SHRINKPALETTE;

Uint8 ortoend[8] = {0xFF, 0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01};
Uint8 andtoend[8] = {0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE};

/////////////////////////////////////////////////////////////////////////////
// uninitialized variables:
/////////////////////////////////////////////////////////////////////////////

typedef struct {
	Uint16 height;
	Uint16 width;
	Uint16 rowofs[200];
} shapehead;

typedef shapehead _seg * shapeseg;

// text crawl variables:
memptr linecode;
void far *linestarts[200];
Uint16 sourceline[200];
Uint16 masterlines;
void far *routine;
memptr sourcepic;
memptr bittables;

// terminator intro variables:
shapeseg commander;
shapeseg keen;
shapeseg both;
memptr scaletable;
memptr cmdrshifts[8];
Sint16 commanderbwide;
Uint16 lastsource;
Uint16 keenstart;
memptr basepl[5];
Uint16 baseplwidth[5];
Uint16 baseplheight[5];
memptr plaqueseg;
Uint16 plaquewidth;
Uint16 plaquewidthwords;
Uint16 plaqueheight;
Uint16 plaqueplane;
Uint16 plaquedelta;
Uint16 *shiftptr;
Uint16 planeon;
Sint16 drawheight;
Uint16 source2;
static Uint16 t_dest;
static Sint16 plaque;
static Sint16 plaquephase;
static Sint16 plaquey;
static Sint16 lastframe;
static Sint16 pageon;
static Sint16 prevbottom[2];
Uint16 pageofs;
Uint16 byteadjust;

#endif	// if GRMODE == EGAGR

//===========================================================================

/*
============================
=
= CheckLastScan
=
============================
*/

void CheckLastScan(void)
{
	if (LastScan)
	{
		if (storedemo)
		{
			playstate = ex_resetgame;
			restartgame = gd_Normal;
			IN_ClearKeysDown();
			NewGame();
		}
#ifndef KEEN6
		else if (LastScan == sc_F1)
		{
			HelpScreens();
		}
#endif
		else
		{
			US_ControlPanel();
			if (restartgame)
			{
				playstate = ex_resetgame;
			}
			else if (loadedgame)
			{
				playstate = ex_loadedgame;
			}
		}
	}
}

#if GRMODE == EGAGR
/*
=============================================================================

							TERMINATOR INTRO

=============================================================================
*/

/*
============================
=
= LoadPlaque
=
============================
*/

void LoadPlaque(Sint16 index)
{
	Sint16 LocatePlaque(Sint16 elapsed);

	Uint16 chunk, picnum, width, height, planesize, i;
	Uint8 far *source;
	Uint16 far *dest;

	//
	// cache the pic and get pic size
	//
	chunk = plaquenum[index];
	CA_CacheGrChunk(chunk);
	picnum = chunk - STARTPICS;
	baseplwidth[index] = width = pictable[picnum].width;
	baseplheight[index] = height = pictable[picnum].height;
	planesize = width * height * 2;

	//
	// allocate buffer and convert pic into to our format
	// (convert bytes to word indices for faster shift-drawing)
	//
	MM_GetPtr(&basepl[index], planesize*2);	// 2 planes
	source = grsegs[chunk];
	dest = basepl[index];
	for (i=0; i<planesize; i++)
	{
		*dest++ = *source++ << 1;
	}

	//
	// pic in original format is no longer needed
	//
	MM_FreePtr(&grsegs[chunk]);
}

/*
============================
=
= DrawPlaque
=
============================
*/

void DrawPlaque(Sint16 elapsed, Uint16 x)
{
	Uint16 shift, xb;
	Sint16 y, bottom, oldbottom;
	Uint16 eraseheight, skip, screenoff;

	shift = x & 7;
	xb = (pageofs + (x / 8)) + (20 - (plaquewidth >> 1));

	EGAMAPMASK(12);	// write to "red" and "intensity" plane (for erasing old pic)

	//
	// update position (and pic number)
	//
	y = LocatePlaque(elapsed);

	//
	// erase leftovers of the previous frame
	//
	bottom = y + plaqueheight;
	if (bottom < 0)
		bottom = 0;
	oldbottom = prevbottom[pageon];
	if (bottom < 200 && oldbottom > bottom)
	{
		eraseheight = oldbottom - bottom;
		screenoff = xb + ylookup[bottom];
		asm {
			mov	es, screenseg;
			mov	bx, linewidth;
			sub	bx, plaquewidthwords;
			sub	bx, plaquewidthwords;
			mov	di, screenoff;
			mov	dx, eraseheight;
			mov	si, plaquewidthwords;
			xor	ax, ax;
		}
eraseloop:
		asm {
			mov	cx, si;
			rep stosw;
			add	di, bx;
			dec	dx;
			jnz	eraseloop;
		}
	}
	if (bottom > 200)
		bottom = 200;
	prevbottom[pageon] = bottom;

	//
	// draw the (new) pic at the new position
	//
	drawheight = plaqueheight;
	skip = 0;
	if (y < 0)
	{
		skip = -y * (plaquewidth << 1);
		drawheight += y;
		y = 0;
	}
	else if (y + plaqueheight > 200)
	{
		drawheight = 200 - y;
	}
	source2 = skip + plaqueplane;
	if (drawheight > 0)
	{
		shiftptr = shifttabletable[shift];
		t_dest = xb + ylookup[y];
		asm {
			mov	bx, skip;
			push	bp;
			mov	bp, shiftptr;
			mov	es, screenseg;
			mov	ds, plaqueseg;
			mov	ah, 4;
			mov	BYTE PTR ss:planeon, ah;
		}
planeloop:
		asm {
			mov	dx, SC_INDEX;
			mov	al, SC_MAPMASK;
			out	dx, ax;
			mov	dx, ss:drawheight;
			mov	di, ss:t_dest;
		}
yloop:
		asm {
			mov	cx, ss:plaquewidth;
			xor	al, al;
		}
xloop:
		asm {
			mov	si, [bx];
			add	bx, 2;
			xor	ah, ah;
			or 	ax, [bp+si];
			stosb;
			mov	al, ah;
			loop	xloop;
			stosb;
			mov	WORD PTR es:[di], 0;
			add	di, ss:plaquedelta;
			dec	dx;
			jnz	yloop;
			mov	bx, ss:source2;
			mov	ah, BYTE PTR ss:planeon;
			shl	ah, 1;
			mov	BYTE PTR ss:planeon, ah;
			cmp	ah, 16;
			jnz	planeloop;
			pop	bp;
			mov	ax, ss;
			mov	ds, ax;
		}
	}
}

/*
============================
=
= LocatePlaque
=
============================
*/

Sint16 LocatePlaque(Sint16 elapsed)
{
	switch (plaquephase)
	{
	case -1:
		//
		// pic starts to appear
		//
		plaqueseg = basepl[plaque];
		plaquewidth = baseplwidth[plaque];
		plaquewidthwords = (plaquewidth + 3) >> 1;
		plaqueheight = baseplheight[plaque];
		plaquedelta = linewidth - (plaquewidth + 1);
		plaqueplane = (plaquewidth * plaqueheight) << 1;
		plaquephase++;
		lastframe = elapsed;
		plaquey = 240;
		// no break or return here!
	case 0:
		//
		// pic is moving from the bottom to the center of the screen
		//
		plaquey -= (elapsed - lastframe) << 1;
		if (plaquey < 100)
		{
			plaquey = 100;
			plaquephase++;
		}
		lastframe = elapsed;
		return plaquey - (plaqueheight >> 1);

	case 1:
		//
		// pic is staying at the center position
		//
		if (elapsed - lastframe > 200)
		{
			plaquephase++;
			lastframe = elapsed;
		}
		return 100 - (plaqueheight >> 1);

	case 2:
		//
		// pic is moving up from the center to the top of the screen
		//
		plaquey -= (elapsed - lastframe) << 1;
		if (plaquey < -40)
		{
			plaquey = -40;
			if (++plaque < 4)
			{
				plaquephase = -1;
			}
			else
			{
				plaquephase = 3;
			}
		}
		lastframe = elapsed;
		return plaquey - (plaqueheight >> 1);
	}

	return -40;
}

/*
============================
=
= SlideLetters
=
============================
*/

void SlideLetters(void)
{
	Sint16 x, cPosX, screenxb;
	Uint16 elapsed, totaltics, dstofs;
	Sint16 cStart, cEnd, cTotalMove;
	Uint16 shift, srcseg, srcofs;
	Sint16 clearleft, copywidth, clearright;
	Uint16 srcdif, dstdif;
	Sint32 now;

	//
	// set up characteristics of the animation
	//
	EGAWRITEMODE(0);
	EGAREADMAP(0);	// useless...

	keenstart = keen->width + 200;
	EGAREADMAP(1);	// also useless ... I think...

	cEnd = 120 - commander->width;
	cStart = 320;
	cTotalMove = cEnd - cStart;
	totaltics = abs(cTotalMove);

	pageofs = pageon = 0;
	lasttimecount = TimeCount;
	while (TimeCount == lasttimecount);
	lasttimecount = TimeCount;

	for (elapsed=0; elapsed <= totaltics; elapsed += tics)
	{
		//
		// draw the credits pic
		//
		x = ((Sint32)keenstart * (Sint32)(totaltics-elapsed)) / (Sint32)totaltics;
		DrawPlaque(elapsed, x);

		//
		// get ready to draw draw the "COMMANDER" pic
		//
		cPosX = cStart + ((Sint32)cTotalMove * (Sint32)elapsed) / (Sint32)totaltics;
		cPosX += x & 7;
		screenxb = (cPosX + 0x800) / 8 + -0x100;
		shift = (cPosX + 0x800) & 7;
		srcseg = FP_SEG(cmdrshifts[shift]);
		srcofs = 0;
		dstofs = pageofs + x / 8;
		if (screenxb > 0)
		{
			clearleft = (screenxb + 1) / 2;
			if (screenxb & 1)
				dstofs--;
			copywidth = 21 - clearleft;
			clearright = 0;
		}
		else if (-commanderbwide + 40 < screenxb)
		{
			clearleft = 0;
			copywidth = 21;
			clearright = 0;
			srcofs -= screenxb;
		}
		else
		{
			clearleft = 0;
			copywidth = (commanderbwide + screenxb) / 2;
			clearright = 21 - copywidth;
			srcofs -= screenxb;
		}
		srcdif = commanderbwide - copywidth*2;
		dstdif = 248 - (clearleft + copywidth + clearright)*2;

		//
		// draw "COMMANDER" pic
		//
		EGAMAPMASK(2);

		asm {
			mov	di, dstofs;
			mov	es, screenseg;
			mov	si, srcofs;
			mov	lastsource, si;
			mov	ds, srcseg;
			mov	dx, 200;
		}
yloop:
		asm {
			xor	ax, ax;
			mov	cx, clearleft;
			rep stosw;
			mov	cx, copywidth;
			rep movsw;
			xor	ax, ax;
			mov	cx, clearright;
			rep stosw;
			test	dx, 1;
			jnz	oddline;
			mov	si, ss:lastsource;
			jmp	nextline;
		}
oddline:
		asm {
			add	si, srcdif;
			mov	ss:lastsource, si;
		}
nextline:
		asm {
			add	di, dstdif;
			dec	dx;
			jnz	yloop;
			mov	ax, ss;
			mov	ds, ax;
		}

		//
		// page flip
		//
		VW_SetScreen(pageofs + x / 8, x & 7);
		pageon ^= 1;
		if (pageon)
		{
			pageofs = 124;
		}
		else
		{
			pageofs = 0;
		}

		//
		// handle timing
		//
		do
		{
			now = TimeCount;
			tics = now - lasttimecount;
		} while (tics < 2);
		lasttimecount = now;

		//
		// handle input
		//
		if (IN_IsUserInput() && LastScan != sc_F1)
		{
			LastScan = sc_Space;
		}
		if (LastScan)
			return;
	}

	byteadjust = x / 8;
}

/*
============================
=
= DrawScan
=
============================
*/

void DrawScan(Sint16 far *source, Uint8 far *dest)
{
	register Uint16 x;
	register Sint16 w;
	register Uint16 val;
	register Uint16 i;

	val = x = 0;
	for (;;)
	{
		//
		// first part: puts black pixels (<width> pixels wide)
		//
		w = *source++;
		if (w == -1)
		{
			*dest++ = val;
			*dest = 0;
			return;
		}

		x += w;
		if (x > 7)
		{
			*dest++ = val;
			val = 0;
			i = (x / 8) - 1;
			while (i--)
			{
				*dest++ = 0;
			}
			x &= 7;
		}

		//
		// second part: puts white pixels (<width> pixels wide)
		//
		w = *source++;
		if (w == -1)
		{
			*dest++ = val;
			*dest = 0;
			return;
		}

		val |= ortoend[x];
		x += w;
		if (x > 7)
		{
			*dest++ = val;
			val = 0xFF;
			i = (x / 8) - 1;
			while (i--)
			{
				*dest++ = 0xFF;
			}
			x &= 7;
		}
		val &= andtoend[x];
	}
}

/*
============================
=
= BuildScaleShape
=
============================
*/

void BuildScaleShape(void)
{
	Sint16 px, w;
	Sint16 far *source;
	Sint16 far *dest;
	Sint16 y;

	MM_GetPtr((memptr*)&both, 30000);
	dest = MK_FP(FP_SEG(both), sizeof(shapehead));

	for (y=0; y<200; y++)
	{
		both->rowofs[y] = FP_OFF(dest);
		px = 0;

		EGAREADMAP(1);	// this is pretty useless, we're not reading from EGA memory here

		source = (Sint16 far *)((byte _seg *)commander + commander->rowofs[y]);
		w = *source++;
		do
		{
			*dest++ = px;
			px = px + w;
			w = *source++;
		} while (w != -1);

		//
		// insert an 80 pixel gap between "COMMANDER" and "KEEN"
		//
		// This assumes that the rightmost column(s) of the "COMMANDER"
		// shape are black. Otherwise the gap would be filled with
		// white pixels and the "KEEN" image would use inverted colors
		// as a result.
		//
		px += 80;

		EGAREADMAP(0);	// this is pretty useless, we're not reading from EGA memory here

		source = (Sint16 far *)((byte _seg *)keen + keen->rowofs[y]);
		source++;	// kludgy bit, causes errors when left egde of "KEEN" is no rectangle
		w = *source++;
		do
		{
			*dest++ = px;
			px = px + w;
			w = *source++;
		} while (w != -1);

		*dest++ = px;	// put last value
		*dest++ = -1;	// put end-of-line
	}
}

/*
============================
=
= ScalePointScan
=
============================
*/

void ScalePointScan(Sint16 far *rowptr, Sint16 y, Sint16 toleft, Sint16 far *scaletable)
{
	Uint8 far *dest;
	Sint16 left, endx;
	Uint16 w, val, x, right;
	register Sint16 px, sx;

	val = x = 0;
	endx = 320 - toleft;
	dest = MK_FP(0xA000, pageofs + byteadjust + ylookup[y]);

	if (toleft < 0)
	{
		left = -toleft;
		val = 0;
		x = 0;

		for (;;)
		{
			px = *rowptr++;
			sx = scaletable[px];
			if (sx > left)
				goto drawwhite;

			px = *rowptr++;
			sx = scaletable[px];
			if (sx > left)
				goto drawblack;
		}
	}

	//
	// regular
	//
	val = 0;
	x = toleft & 7;
	dest += (toleft >> 3);
	left = 0;
	rowptr++;	// the first value is always 0, we need the next value
drawloop:
	px = *rowptr++;
	sx = scaletable[px];

	//
	// draw/add black pixels
	//
drawblack:
	w = sx - left;
	left = sx;
	x += w;
	if (x > 7)
	{
		asm {
			les	di, dest;
			mov	al, BYTE PTR val;
			stosb;
			mov	cx, x;
			shr	cx, 1;
			shr	cx, 1;
			shr	cx, 1;
			dec	cx;
			xor	al, al;
			mov	BYTE PTR val, al;
			rep stosb;
			and	x, 7;
			mov	WORD PTR dest, di;
		}
	}

	//
	// stop if the right side of the screen is reached
	//
	if (sx > endx)
		return;

	//
	// stop if the end of the image row is reached
	// 
	// This is only checked after drawing the black part, so the
	// combined shape must not end with white pixels on the right.
	// That means the rightmost column(s) of the "KEEN" shape must
	// always be black.
	//
	px = *rowptr++;
	if (px == -1)
		goto clearright;

	sx = scaletable[px];

	//
	// draw/add white pixels
	//
drawwhite:
	w = sx - left;
	left = sx;
	val |= ortoend[x];
	x += w;
	if (x > 7)
	{
		asm {
			les	di, dest;
			mov	al, BYTE PTR val;
			stosb;
			mov	cx, x;
			shr	cx, 1;
			shr	cx, 1;
			shr	cx, 1;
			dec	cx;
			mov	al, 255;
			mov	BYTE PTR val, al;
			rep stosb;
			and	x, 7;
			mov	WORD PTR dest, di;
		}
	}

	//
	// stop if the right side of the screen is reached
	//
	if (sx > endx)
		return;

	val &= andtoend[x];
	goto drawloop;

	//
	// clear the right side of the screen
	//
clearright:
	w = 320 - left;
	x += w;
	if (x > 7)
	{
		*dest++ = val;
		val = 0;
		right = x / 8 - 1;
		while (right--)
		{
			*dest++ = 0;
		}
		x &= 7;
		return;
	}
	return;
}

/*
============================
=
= ScaleDown
=
============================
*/

void ScaleDown(void)
{
	Uint16 i;
	Uint16 toleft, ticselapsed, ticstotal, scale, endscale, rownum, rowinc;
	Sint32 now;
	Sint16 far *rowptr;
	Uint16 scaleheight, top, bottom, lastbottom[2];
	Sint32 leftorigin;

	//
	// set our new palette
	//
	SetPalette(termcolors2);

	EGAREADMAP(1);	// this is pretty useless, we're not reading from EGA memory here

	leftorigin = 120l - commander->width;
	BuildScaleShape();
	MM_GetPtr(&scaletable, 2500*sizeof(Uint16));

	scale = 0x100;		// 100%
	endscale = 0x21;	// 13% (scale from 200px to 26px)
	endscale = 0x21;	// redundant
	lastbottom[0] = lastbottom[1] = 200;
	ticselapsed = 1;
	ticstotal = 30;	// time for the whole shrinking animation

	while (ticselapsed <= ticstotal)
	{
		//
		// get current scaling
		//
		if (ticselapsed == ticstotal)
		{
			scale = endscale;
			toleft = 0;
			top = 4;
		}
		else
		{
			scale = 0x100 - ((0x100-endscale) * ticselapsed) / ticstotal;
			toleft = (leftorigin * (ticstotal - ticselapsed)) / ticstotal;
			top = (ticselapsed * 4) / ticstotal;
		}

		//
		// build scale table:		scaletable[i] = (i*scale) / 0x100;
		//
		asm {
			xor	ax, ax;
			xor	dx, dx;
			mov	cx, 2500;
			mov	bx, scale;
			mov	es, scaletable;
			xor	di, di;
		}
l1:
		asm {
			mov	es:[di], ah;
			inc	di;
			mov	es:[di], dl;
			inc	di;
			add	ax, bx;
			adc	dx, 0;
			loop	l1;
		}

		//
		// wait... didn't we already do this?
		//
		if (ticselapsed == ticstotal)
		{
			toleft = 0;
		}
		else
		{
			toleft = (leftorigin * (ticstotal - ticselapsed)) / ticstotal;
		}

		//
		// prepare scaled drawing process
		//
		scaleheight = ((Sint16 _seg *)scaletable)[200];
		rownum = 0;
		rowinc = 0x10000l / scale;
		bufferofs = pageofs + byteadjust;

		//
		// erase stuff at the top
		//
		if (top > 0)
		{
			VW_Bar(0, 0, 320, top, BLACK);
		}

		//
		// draw the scaled shape
		//
		EGAWRITEMODE(0);
		EGAMAPMASK(15);

		for (i=0; i<scaleheight; i++)
		{
			rowptr = (Sint16 far *)((byte _seg *)both + both->rowofs[rownum >> 8]);
			ScalePointScan(rowptr, i+top, toleft, scaletable);

			rownum += rowinc;
		}

		//
		// erase leftovers at the bottom of the screen
		//
		bufferofs = pageofs + byteadjust;
		bottom = scaleheight + top;
		if (lastbottom[pageon] > bottom)
		{
			VW_Bar(0, bottom, 320, lastbottom[pageon] - bottom, BLACK);
			lastbottom[pageon] = bottom;
		}

		//
		// page flip
		//
		VW_SetScreen(pageofs+byteadjust, 0);
		pageon ^= 1;
		if (pageon)
		{
			pageofs = 124;
		}
		else
		{
			pageofs = 0;
		}

		//
		// handle timing
		//
		now = TimeCount;
		tics = now - lasttimecount;
		lasttimecount = now;
		if (tics > 8)
			tics = 8;	// don't skip too many frames on slow systems

		if (ticselapsed == ticstotal)
			break;

		ticselapsed += tics;
		if (ticselapsed > ticstotal)
			ticselapsed = ticstotal;

		//
		// handle input
		//
		if (IN_IsUserInput() && LastScan != sc_F1)
		{
			LastScan = sc_Space;
		}
		if (LastScan)
			return;	// BUG: buffers aren't freed!
	}

	//
	// free the buffers
	//
	MM_FreePtr(&scaletable);
	MM_FreePtr((memptr*)&both);
}

/*
============================
=
= FinishPage
=
============================
*/

void FinishPage(void)
{
	Sint16 swap, temp, i, n, x, y;
	Uint16 ofs;
	Sint16 top, bottom, delta;
	Uint8 bitmask[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
	Sint16 xtable[320], ytable[200];

	//
	// build lookup tables
	//
	for (i=0; i<320; i++)
	{
		xtable[i] = i;
	}
	for (i=0; i<320; i++)
	{
		swap = random(320);
		temp = xtable[swap];
		xtable[swap] = xtable[i];
		xtable[i] = temp;
	}
	for (i=0; i<200; i++)
	{
		ytable[i] = xtable[i];
	}

	//
	// set up display
	//
	VW_SetDefaultColors();
	if (pageon)
	{
		bufferofs = byteadjust + 124;
		displayofs = byteadjust;
	}
	else
	{
		bufferofs = byteadjust;
		displayofs = byteadjust + 124;
	}
	VW_SetScreen(displayofs, 0);

	//
	// draw title pic to the non-displayed buffer
	//
	VW_DrawPic(0, 0, TITLEPICPIC);

	//
	// copy "random" pixels from the non-displayed area
	// into the displayed area to create the "fizzle" effect
	//
	delta = displayofs - bufferofs;

	//
	// set ES register for the pixel copying code in the loops
	//
	// This is faster than setting the ES register in the loops,
	// but you need to make sure nothing in the loops overwrites
	// the ES register, otherwise the code won't work correctly.
	//
	asm	mov	es, screenseg;

	for (i = 0; i< 360; i++)
	{
		top = i - 160;
		if (top < 0)
			top = 0;

		bottom = i;
		if (bottom >= 200)
			bottom = 199;

		for (y = top; y <= bottom; y++)
		{
			ofs = bufferofs + ylookup[y];
			for (n=0; n<2; n++)
			{
				x = xtable[ytable[y]];
				if (++ytable[y] == 320)
				{
					ytable[y] = 0;
				}

				//
				// set bitmask for our x value
				//
				asm	mov	cx, x;
				asm	mov	si, cx;
				asm	and	si, 7;
				asm	cli;
				asm	mov	dx, GC_INDEX;
				asm	mov	al, GC_BITMASK;
				asm	mov	ah, BYTE PTR bitmask[si];
				asm	out	dx, ax;
				asm	sti;

				//
				// set up source and dest index registers
				//
				asm	mov	si, ofs;
				asm	shr	cx, 1;
				asm	shr	cx, 1;
				asm	shr	cx, 1;
				asm	add	si, cx;
				asm	mov	di, si;
				asm	add	di, delta;

				//
				// copy the pixel data (all 4 planes)
				//
				// "blue" plane:
				asm	mov	dx, SC_INDEX;
				asm	mov	ax, SC_MAPMASK + 1*256;
				asm	out	dx, ax;
				asm	mov	dx, GC_INDEX;
				asm	mov	ax, GC_READMAP + 0*256;
				asm	out	dx, ax;
				asm	mov	bl, es:[si];
				asm	xchg	bl, es:[di];
				// "green" plane:
				asm	mov	dx, SC_INDEX;
				asm	mov	ax, SC_MAPMASK + 2*256;
				asm	out	dx, ax;
				asm	mov	dx, GC_INDEX;
				asm	mov	ax, GC_READMAP + 1*256;
				asm	out	dx, ax;
				asm	mov	bl, es:[si];
				asm	xchg	bl, es:[di];
				// "red" plane:
				asm	mov	dx, SC_INDEX;
				asm	mov	ax, SC_MAPMASK + 4*256;
				asm	out	dx, ax;
				asm	mov	dx, GC_INDEX;
				asm	mov	ax, GC_READMAP + 2*256;
				asm	out	dx, ax;
				asm	mov	bl, es:[si];
				asm	xchg	bl, es:[di];
				// "intensity" plane:
				asm	mov	dx, SC_INDEX;
				asm	mov	ax, SC_MAPMASK + 8*256;
				asm	out	dx, ax;
				asm	mov	dx, GC_INDEX;
				asm	mov	ax, GC_READMAP + 3*256;
				asm	out	dx, ax;
				asm	mov	bl, es:[si];
				asm	xchg	bl, es:[di];
			}
		}

		VW_WaitVBL(1);	// so the fizzle animation won't go super fast

		if (IN_IsUserInput() && LastScan != sc_F1)
		{
			LastScan = sc_Space;
		}
		if (LastScan)
		{
			EGABITMASK(0xFF);
			EGAMAPMASK(15);
			return;
		}
	}

	//
	// clean up EGA registers
	//
	EGABITMASK(0xFF);
	EGAMAPMASK(15);

	//
	// pause for 6 seconds
	//
	IN_UserInput(6 * TickBase, false);
}

/*
============================
=
= Terminator
=
============================
*/

void Terminator(void)
{
	Uint16 i, shift, bufsize;
	Sint16 far *source;
	Uint8 far *dest;
	Uint16 srcseg, destseg;
	boolean pagefinished;
	Uint16 rowofs[200];

	pagefinished = false;
	CA_SetAllPurge();
	SetPaletteEx(colors[0]);	// all black
	VW_ClearVideo(BLACK);
	VW_SetLineWidth(248);	// 1984 pixels total, we're using 992 per "page"

	CA_CacheGrChunk(TITLEPICPIC);
	CA_CacheGrChunk(BIGCOMMANDER);
	CA_CacheGrChunk(BIGKEEN);
	keen = grsegs[BIGKEEN];
	commander = grsegs[BIGCOMMANDER];

	EGAMAPMASK(1);

	keenstart = keen->width + 200;
	VW_SetScreen((keenstart/8)+1, 0);

	//
	// draw the "KEEN" pic (to first "page")
	//
	for (i=0; i<200; i++)
	{
		source = (Sint16 far *)((byte _seg *)keen + keen->rowofs[i]);
		dest = MK_FP(0xA000, ylookup[i]);
		dest += 25;	// 25 bytes -> 200 pixels
		DrawScan(source, dest);
	}
	//
	// copy pic from first "page" to second "page"
	//
	VW_ScreenToScreen(0, 124, 109, 200);

	//
	// create pre-shifted image buffers for the "COMMANDER" pic
	// (only 100 pixels high instead of 200 pixels to save memory)
	//
	commanderbwide = (commander->width + 7) / 8;
	commanderbwide = (commanderbwide + 3) & ~1;
	bufsize = commanderbwide * 100;	// half height
	for (shift = 0; shift < 8; shift++)
	{
		MM_GetPtr(&cmdrshifts[shift], bufsize);
	}

	//
	// re-assign shape pointers (memory manager might have moved the buffers)
	//
	keen = grsegs[BIGKEEN];
	commander = grsegs[BIGCOMMANDER];

	//
	// draw the first (unshifted) version of the "COMMANDER" pic to the buffer
	//
	for (i=0; i<100; i++)
	{
		rowofs[i*2] = rowofs[i*2+1] = i * commanderbwide;
		source = (Sint16 far *)((byte _seg *)commander + commander->rowofs[i*2]);
		dest = (Uint8 _seg *)cmdrshifts[0] + rowofs[i*2];
		DrawScan(source, dest);
	}

	//
	// create the shifted versions of the "COMMANDER" pic
	//
	for (shift = 1; shift < 8; shift++)
	{
		srcseg = FP_SEG(cmdrshifts[shift-1]);
		destseg = FP_SEG(cmdrshifts[shift]);
		asm {
			mov	ds, srcseg;
			mov	es, destseg;
			mov	cx, bufsize;
			clc;
			xor	si, si;
			xor	di, di;
		}
l1:
		asm {
			lodsb;
			rcr	al, 1;
			stosb;
			loop	l1;
			mov	ax, ss;
			mov	ds, ax;
		}
	}

	//
	// prepare (and set) the palettes
	//
	termcolors[16] = termcolors2[16] = termcolors[16] = bordercolor;
	SetPalette(termcolors);

	//
	// cache the credits pics (they are converted into a special
	// format to make shifted drawing easier during the animation)
	//
	for (i=0; i<4; i++)
	{
		LoadPlaque(i);
	}

	//
	// play the animation
	//
	plaque = lastframe = 0;
	plaquephase = -1;
	SlideLetters();

	//
	// free some of the buffers
	// (shrink animation needs additional memory)
	//
	for (i=0; i<4; i++)
	{
		MM_FreePtr(&basepl[i]);
	}
	for (shift=0; shift<8; shift++)
	{
		MM_FreePtr(&cmdrshifts[shift]);
	}

	//
	// do the shrinking and fizzle fade animations
	// (if intro wasn't aborted)
	//
	if (!LastScan)
	{
		ScaleDown();
	}

	if (!LastScan)
	{
		FinishPage();
		pagefinished = true;
	}

	//
	// free the remaining buffers
	//
	MM_SetPurge(&grsegs[BIGCOMMANDER], 3);
	MM_SetPurge(&grsegs[BIGKEEN], 3);

	//
	// switch back to default graphics settings
	//
	VW_ClearVideo(BLACK);
	VW_SetLineWidth(SCREENWIDTH);
	VW_SetDefaultColors();
	RF_FixOfs();
	CA_ClearMarks();

	//
	// handle input and main menu stuff
	//
	if (LastScan == sc_None)
	{
		return;
	}
#ifndef KEEN6
	if (LastScan == sc_F1)
	{
		HelpScreens();
		return;
	}
#endif
	if (!pagefinished)
	{
		RF_FixOfs();	//redundant
		CA_CacheGrChunk(TITLEPICPIC);
		VW_DrawPic(0, 0, TITLEPICPIC);
		VW_SetScreen(bufferofs, 0);
		IN_Ack();
		CA_ClearMarks();
		if (storedemo)
		{
			playstate = ex_resetgame;
			restartgame = gd_Normal;
			IN_ClearKeysDown();
			NewGame();
			return;
		}
	}

	US_ControlPanel();
	if (restartgame)
	{
		playstate = ex_resetgame;
	}
	else if (loadedgame)
	{
		playstate = ex_loadedgame;
	}
}

/*
=============================================================================

							STAR WARS TEXT CRAWL

=============================================================================
*/

/*
============================
=
= BuildBitTables
=
============================
*/

void BuildBitTables(void)
{
	Uint16 bit1, bit2, i;
	Uint8 far *buffer;

	MM_GetPtr(&bittables, 0x4000);
	buffer = bittables;

	//
	// generate a lookup table that maps the bits of the "texture" (bit1)
	// to the appropriate bit for the screen position (bit2) to make the
	// scaler code faster and smaller
	//
	// table[((7-b1)*8+(7-b2))*256+i] = (i & (1 << (7-b1))) ? (1 << (7-b2)) : 0;
	//
	for (bit1 = 1; bit1 < 0x100; bit1 <<= 1)
	{
		for (bit2 = 1; bit2 < 0x100; bit2 <<= 1)
		{
			for (i = 0; i < 0x100; i++, buffer++)
			{
				if (i & bit1)
				{
					*buffer = bit2;
				}
				else
				{
					*buffer = 0;
				}
			}
		}
	}
}

/*
============================
=
= CompileSWUpdate
=
============================
*/

void CompileSWUpdate(void)
{
	Sint16 y;
	Uint16 i, width, scalestep, step;
	Sint32 scale, rowof, xpos, size;
	void far *buffer;
	Uint8 srcoff, srcbit, bitpos;
	Uint16 destoff, srcx, left, orindex, lastoff;

	BuildBitTables();
	size = 190000;
	MM_GetPtr(&linecode, size);
	buffer = linecode;
	//
	// Note: You should really lock the pointer to prevent the memmory manager
	// from moving the buffer around. This code stores a bunch of pointers to
	// this memory block in the linestarts array. Those pointers will not be
	// updated when the memory manager moves the buffer around and the game
	// might end up crashing (or worse) when trying to run the "code" at the
	// memory location after the data was moved. The game starts playing music
	// after this function is done, which may or may not cause the memory
	// manager to move memory blocks around.
	//

	//
	// move the buffer address into ES:DI (and keep it there)
	//
	asm	mov	es, WORD PTR buffer+2;
	asm	mov	di, WORD PTR buffer;
	//
	// Since the address is kept in ES:DI, we must save and restore
	// the ES register when calling other functions (push es / pop es).
	// The Borland C compiler always saves and restores the DI register
	// when a function modifies it, so we don't need to worry about
	// that register. This is a bit of an ugly hack, but it makes this
	// code a little faster and smaller.
	//

	scale = 320l << 11;
	scalestep = (((Uint32)(320-40) << 11) / 200);	// roughly 1.4 pixels per step, going from 320 pixels to 40 pixels in 200 steps
	rowof = 0;

	for (y=199; y >= 0; y--)
	{
		//
		// draw a blue line for the current row
		//
		asm	push	es;
		VW_Hlin(0, 320, y, BLUE);
		asm	pop	es;

		//
		// update the buffer variable with the current (normalized) ES:DI address
		//
		asm	mov	WORD PTR buffer, di;
		asm	mov	WORD PTR buffer+2, es;

		//
		// store the address in the scaler lookup table
		//
		linestarts[y] = buffer;

		//
		// get current scaling factors
		//
		width = ((Uint16)((scale/2) >> 11)) << 1;	// some trickery to make sure width is even
		sourceline[y] = (rowof >> 11);
		step = (336l << 11) / width;
		xpos = 0;
		rowof += step;
		left = 160 - (width >> 1);
		destoff = ylookup[y] + left / 8;
		bitpos = left & 7;

		//
		// generate the machine code
		//
		//		MOV	CX, SS
		//		MOV	SS, AX
		//		ADD	DI, <destoff>
		//		XOR	AL, AL
		//
		asm	mov	ax, 0D18Ch;
		asm	stosw;
		asm	mov	ax, 0D08Eh;
		asm	stosw;
		asm	mov	ax, 0C781h;
		asm	stosw;
		asm	mov	ax, destoff;
		asm	stosw;
		asm	mov	ax, 0C030h;
		asm	stosw;

		lastoff = -1;
		for (i=0; i<width; i++)
		{
			srcx = (xpos >> 11);
			srcoff = (srcx / 8);
			srcbit = srcx & 7;

			orindex = ((7-srcbit)*8 + 7-bitpos) << 8;
			if (srcoff != lastoff)
			{
				//
				//		MOV	BL, [BP + <srcoff>]
				//
				asm	mov	ax, 5E8Ah;
				asm	stosw;
				asm	mov	al, srcoff;
				asm	stosb;

				lastoff = srcoff;
			}

			//
			//		OR		AL, [BX + <orindex>]
			//
			asm	mov	ax, 870Ah;
			asm	stosw;
			asm	mov	ax, orindex;
			asm	stosw;

			bitpos++;
			if (bitpos == 8)
			{
				bitpos = 0;

				//
				//		STOSB
				//		XOR	AL, AL
				//
				asm	mov	ax, 30AAh;
				asm	stosw;
				asm	mov	al, 0C0h;
				asm	stosb;
			}

			xpos += step;
		}

		if (bitpos)
		{
			//
			//		STOSB
			//
			asm	mov	al, 0AAh;
			asm	stosb;
		}
		//
		// generate end of subroutine
		//
		//		MOV	SS, CX
		//		RETF
		//
		asm	mov	ax, 0D18Eh;
		asm	stosw;
		asm	mov	al, 0CBh;
		asm	stosb;

		//
		// normalize ES:DI
		//
		asm	mov	ax, di;
		asm	shr	ax, 1;
		asm	shr	ax, 1;
		asm	shr	ax, 1;
		asm	shr	ax, 1;
		asm	mov	bx, es;
		asm	add	ax, bx;
		asm	mov	es, ax;
		asm	and	di, 0Fh;

		//
		// update scale value for next row
		//
		scale -= scalestep;

		//
		// replace the blue line with the row from the background image
		//
		asm	push	es;
		VW_ScreenToScreen(ylookup[y] + 0x8000, ylookup[y], 40, 1);
		asm	pop	es;

		if (LastScan)
			return;
	}
}

/*
============================
=
= TranslateString
=
============================
*/

void TranslateString(char *text)
{
	char c;

	while (*text)
	{
		c = *text;

		if (c >= 'A' && c <= 'Z')
		{
			c = c + -33;
		}
		else if (c >= 'a' && c <= 'z')
		{
			c = c + -39;
		}
		else if (c == '.')
		{
			c = 84;
		}
		else if (c == ',')
		{
			c = 85;
		}
		else if (c == '-')
		{
			c = 86;
		}
		else if (c == '"')
		{
			c = 87;
		}
		else if (c == ' ')
		{
			c = 88;
		}
		else if (c == '!')
		{
			c = 89;
		}
		else if (c == '\'')
		{
			c = 90;
		}
		else if (c != '\n')
		{
			c = 84;	// any unhandled character is drawn as '.'
		}

		*text++ = c;
	}
}

/*
============================
=
= DrawSWText
=
============================
*/

void DrawSWText(void)
{
	char far *text;
	char *ptr;
	char c;
	char strbuf[80];

	WindowX = 0;
	WindowW = 336;
	PrintY = 1;		// always leave the first line blank
	bufferofs = 0;
	panadjust = 0;
	text = swtext;
	masterlines = 0;

	//
	// draw the entire text to video memory
	//
	while (*text)
	{
		ptr = strbuf;
		do
		{
			c = *text++;
			*ptr++ = c;
		} while (c != '\n' && c != '\0');
		*ptr = '\0';

		TranslateString(strbuf);

		US_CPrint(strbuf);

		bufferofs += ylookup[PrintY];
		masterlines += PrintY;
		PrintY = 0;
	}

	//
	// allocate a buffer large enough to hold the entire text image
	// and move the image data from video memory into that buffer
	//
	MM_GetPtr(&sourcepic, bufferofs);
	EGAREADMAP(1);	// read from "green" plane (doesn't really matter from which plane we read)
	movedata(screenseg, 0, FP_SEG(sourcepic), 0, bufferofs);

	//
	// erase the (first screen of the) text from video memory.
	// we're going to display this area and copy the backgound pic
	// here line-by-line as the scalers are generated and we don't
	// want to have parts of the text still visible at that point.
	//
	bufferofs = 0;
	VW_Bar(0, 0, 320, 200, BLACK);
}

/*
============================
=
= ScrollSWText
=
============================
*/

void ScrollSWText(void)
{
	Sint32 now;
	Uint16 pos;
	Sint16 i, rowof;

	tics = TimeCount = lasttimecount = 0;

	EGAWRITEMODE(0);
	EGAMAPMASK(8);	// only draw to the "intensity" plane (so we don't erase the backgound pic)

	pos = 0;
	while (masterlines + 400 >= pos)
	{
		for (i = 199; i >= 0; i--)
		{
			rowof = pos - sourceline[i];
			if (rowof < 0 || rowof >= masterlines)
			{
				masterofs = 0;	// draw the first (blank) line of the buffer
			}
			else
			{
				masterofs = rowof * 42;
			}
			routine = linestarts[i];
			asm {
				mov	es, screenseg;
				mov	di, pageofs;
				mov	ds, bittables;
				push	bp;
				mov	bp, ss:masterofs;
				mov	ax, ss:sourcepic;
				xor	bh, bh;
				cli;	// disable interrupts (scaler changes register SS, so interrupts would be fatal!)
				call	ss:routine;
				sti;	// enable interrupts again
				pop	bp;
				mov	ax, ss;
				mov	ds, ax;
			}
		}

		VW_SetScreen(pageofs, 0);
		pageon ^= 1;
		pageofs = pageon << 15;

		now = TimeCount;
		tics = tics + (now - lasttimecount);
		lasttimecount = now;
		if (tics > 20)
			tics = 20;

		pos = pos + tics / 4;
		tics &= 3;

		if (IN_IsUserInput() && LastScan != sc_F1)
			LastScan = sc_Space;

		if (LastScan)
			break;
	}
}

/*
============================
=
= StarWars
=
============================
*/

void StarWars(void)
{
	SetPaletteEx(colors[0]);	// all black
	VW_ClearVideo(BLACK);
	VW_SetLineWidth(42);	// 336 pixels
	VW_SetScreen(0, 0);
	pageon = pageofs = 0;
	CA_SetAllPurge();
	CA_CacheGrChunk(STARTFONT+2);
	fontnumber = 2;
	DrawSWText();
	fontnumber = 0;

	CA_CacheGrChunk(SW_BACKGROUNDPIC);
	bufferofs = 0x8000;
	VW_DrawPic(0, 0, SW_BACKGROUNDPIC);
	CA_SetAllPurge();
	SetPaletteEx(starcolors);
	bufferofs = 0;
	CompileSWUpdate();

	if (!LastScan)
	{
		StartMusic(STARWARSMUSIC);
		ScrollSWText();
		StopMusic();
	}

	MM_FreePtr(&linecode);
	MM_FreePtr(&bittables);
	MM_FreePtr(&sourcepic);

	VW_ClearVideo(BLACK);
	VW_SetLineWidth(SCREENWIDTH);
	VW_SetDefaultColors();
	RF_FixOfs();
	CA_ClearMarks();

	CheckLastScan();
}

#endif	// if GRMODE == EGAGR

//===========================================================================

/*
============================
=
= ShowTitle
=
============================
*/

void ShowTitle(void)
{
	panadjust = 0;
	CA_CacheGrChunk(TITLEPICPIC);
	VW_DrawPic(0, 0, TITLEPICPIC);
#if GRMODE == CGAGR
	VW_UpdateScreen();
#else
	VW_SetScreen(displayofs, 0);
	VW_ScreenToScreen(bufferofs, displayofs, 42, 224);
#endif
	IN_UserInput(6*TickBase, false);
	CA_ClearMarks();
	CheckLastScan();
}

//===========================================================================

#if GRMODE == CGAGR
/*
============================
=
= ShowCredits
=
============================
*/

void ShowCredits(void)
{
	panadjust = 0;
	CA_CacheGrChunk(SW_BACKGROUNDPIC);
	VW_DrawPic(0, 0, SW_BACKGROUNDPIC);
	VW_UpdateScreen();
	IN_UserInput(6*TickBase, false);
	CA_ClearMarks();
	CheckLastScan();
}
#endif

//===========================================================================

/*
============================
=
= RunDemo
=
============================
*/

void RunDemo(Sint16 num)
{
	Uint16 far *demodata;
	
	NewGame();
	num += DEMO0;
	CA_CacheGrChunk(num);
	demodata = grsegs[num];
	gamestate.mapon = demodata[0];
	DemoSize = demodata[1];
	MM_GetPtr(&(memptr)DemoBuffer, DemoSize);
	MM_SetLock(&(memptr)DemoBuffer, true);
	_fmemcpy(DemoBuffer, ((char _seg *)grsegs[num])+4, DemoSize);
	MM_FreePtr(&grsegs[num]);
	IN_StartDemoPlayback(DemoBuffer, DemoSize);
	SetupGameLevel(true);
	if (scorescreenkludge)
	{
		DrawHighScores();
	}
	PlayLoop();
	IN_StopDemo();
	MM_FreePtr(&(memptr)DemoBuffer);
	VW_FixRefreshBuffer();
	CA_ClearMarks();
	CheckLastScan();
}

//===========================================================================

/*
============================
=
= DrawHighScores
=
============================
*/

void DrawHighScores(void)
{
	Uint16 i, n;
	Uint16 width, height;
	HighScore *entry;
	Uint16 oldbufferofs;
	char buf[16], *bufptr;
	
	RF_NewPosition(0, 0);
	oldbufferofs = bufferofs;
	bufferofs = masterofs;
#ifdef KEEN5
#if GRMODE == CGAGR
	fontcolor = 2;
#else
	fontcolor = BLUE ^ LIGHTMAGENTA;	// blue text on light magenta background (XOR draw mode!)
#endif
#endif
	for (i=0, entry=&Scores[0]; i<MaxScores; i++, entry++)
	{
		PrintY = i*16 + HIGHSCORE_TOP;
		PrintX = HIGHSCORE_LEFT;
		US_Print(entry->name);
#ifdef KEEN4
		PrintX = 152;
		for (n=0; n<entry->completed; n++)
		{
			VWB_DrawTile8(PrintX, PrintY+1, 71);
			PrintX += 8;
		}
#endif
		ultoa(entry->score, buf, 10);
		for (bufptr=buf; *bufptr; bufptr++)
		{
			*bufptr = *bufptr + 81;
		}
		USL_MeasureString(buf, &width, &height);
		PrintX = HIGHSCORE_RIGHT - width;
		US_Print(buf);
	}
	fontcolor = WHITE;	// back to default color
	bufferofs = oldbufferofs;
}

//===========================================================================

/*
============================
=
= CheckHighScore
=
============================
*/

void CheckHighScore(Sint32 score, Sint16 completed)
{
	Uint16 i, n;
	Sint16 index;
	HighScore entry;
	
	strcpy(entry.name, "");	//Note: 'entry.name[0] = 0;' would be more efficient
	entry.score = score;
	entry.completed = completed;
	for (i=0, index=-1; i<MaxScores; i++)
	{
		if (Scores[i].score < entry.score ||
			(Scores[i].score == entry.score && Scores[i].completed < entry.completed))
		{
			n=MaxScores;
			while (--n > i)
			{
				Scores[n] = Scores[n-1];
			}
			Scores[i] = entry;
			index = i;
			HighScoresDirty = true;
			break;
		}
	}
	if (index != -1)
	{
		scorescreenkludge = true;
		gamestate.mapon = HIGHSCORE_MAP;
		SetupGameLevel(true);
		DrawHighScores();
#ifdef KEEN5
#if GRMODE == CGAGR
		fontcolor = 2;
#else
		fontcolor = BLUE ^ LIGHTMAGENTA;	// blue text on light magenta background (XOR draw mode!)
#endif
#endif
		RF_Refresh();
		RF_Refresh();
		PrintY = i*16 + HIGHSCORE_TOP;
		PrintX = HIGHSCORE_LEFT;
		US_LineInput(PrintX, PrintY, Scores[index].name, NULL, true, MaxHighName, 112);
		scorescreenkludge = false;
	}
#ifdef KEEN5
	fontcolor = 15;	// back to default color (white)
#endif
}

//===========================================================================

/*
============================
=
= ShowHighScores
=
============================
*/

void ShowHighScores(void)
{
	scorescreenkludge = true;
	IN_ClearKeysDown();
	RunDemo(4);
	scorescreenkludge = false;
}
