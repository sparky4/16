/* Project 16 Source Code~
 * Copyright (C) 2012-2017 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
 *
 * This file is part of Project 16.
 *
 * Project 16 is free software; you can redistribute it and/or	modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Project 16 is distributed in	the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without	even the implied warranty of
 * MERCHANTABILITY or	FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for	more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 * write to the Free Software Foundation, Inc., 51 Franklin	Street,
 * Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include "src/lib/16_vl.h"

static word far* clockw= (word far*) 0x046C; /* 18.2hz clock */
#define VGASTRIDEVARIABLE	gvar->video.page[0].stridew
//vga_state.vga_stride

//===========================================================================

//==============
//
// VL_SetScreen
//
//==============

void	VL_SetScreen (unsigned int crtc, int pelpan)
{
// PROC	VL_SetScreen  crtc:WORD, pel:WORD
// PUBLIC	VL_SetScreen
	word TimeCount = *clockw;
	__asm {
		mov	cx,[TimeCount]		// if TimeCount goes up by two, the retrace
		add	cx,2				// period was missed (an interrupt covered it)

		mov	dx,STATUS_REGISTER_1

	// wait for	a display signal to make sure the raster isn't in	the middle
	// of a sync

#ifdef __BORLANDC__
	}
#endif
SetScreen_waitdisplay:
#ifdef __BORLANDC__
	__asm {
#endif
		in	al,dx
		test	al,1	//1 = display is disabled (HBL / VBL)
		jnz	SetScreen_waitdisplay

#ifdef __BORLANDC__
	}
#endif
SetScreen_loop:
#ifdef __BORLANDC__
	__asm {
#endif
		sti
		jmp	SetScreen_skip1
		cli
#ifdef __BORLANDC__
	}
#endif
SetScreen_skip1:
#ifdef __BORLANDC__
	__asm {
#endif
		cmp	[TimeCount],cx		// will only happen if an interrupt is
		jae	SetScreen_setcrtc			// straddling the entire retrace period

	// when several succesive display not enableds occur,
	// the bottom of the screen has been hit

		in	al,dx
		test	al,8
		jnz	SetScreen_waitdisplay
		test	al,1
		jz	SetScreen_loop

		in	al,dx
		test	al,8
		jnz	SetScreen_waitdisplay
		test	al,1
		jz	SetScreen_loop

		in	al,dx
		test	al,8
		jnz	SetScreen_waitdisplay
		test	al,1
		jz	SetScreen_loop

		in	al,dx
		test	al,8
		jnz	SetScreen_waitdisplay
		test	al,1
		jz	SetScreen_loop

		in	al,dx
		test	al,8
		jnz	SetScreen_waitdisplay
		test	al,1
		jz	SetScreen_loop

#ifdef __BORLANDC__
	}
#endif
SetScreen_setcrtc:
#ifdef __BORLANDC__
	__asm {
#endif
	// set CRTC start
	// for	some reason, my XT's EGA card doesn't like word outs to the CRTC index...

		mov	cx,[crtc]
		mov	dx,CRTC_INDEX
		mov	al,0ch		//start address high register
		out	dx,al
		inc	dx
		mov	al,ch
		out	dx,al
		dec	dx
		mov	al,0dh		//start address low register
		out	dx,al
		mov	al,cl
		inc	dx
		out	dx,al


	// set horizontal panning

		mov	dx,ATR_INDEX
//		mov	al,ATR_PELPAN or	20h
		out	dx,al
		jmp	SetScreen_done
		mov	al,[BYTE PTR pelpan]		//pel pan value
		out	dx,al
#ifdef __BORLANDC__
	}
#endif
SetScreen_done:
#ifdef __BORLANDC__
	__asm {
#endif
//		sti

//		ret
	}
}

/*
====================
=
= VL_SetLineWidth
=
= Line witdh is in	WORDS, 40 words is normal width for	vgaplanegr
=
====================
*/
#if 0
void VL_SetLineWidth (unsigned width, global_game_variables_t *gvar)
{
	int i,offset;

//
// set wide virtual screen
//
	outport (CRTC_INDEX,CRTC_OFFSET+width*256);

//
// set up lookup tables
//
	gvar->video.ofs.linewidth = width*2;

	offset = 0;

	for	(i=0;i<MAXSCANLINES;i++)
	{
		gvar->video.ofs.ylookup[i]=offset;
		offset += gvar->video.ofs.linewidth;
	}
}
#endif
/*
=============================================================================

						PALETTE OPS

		To avoid snow, do a WaitVBL BEFORE calling these

=============================================================================
*/


/*
=================
=
= VL_FillPalette
=
=================
*/

void VL_FillPalette (int red, int green, int blue)
{
	int	i;

	outportb (PAL_WRITE_REG,0);
	for	(i=0;i<256;i++)
	{
		outportb (PAL_DATA_REG,red);
		outportb (PAL_DATA_REG,green);
		outportb (PAL_DATA_REG,blue);
	}
}

//===========================================================================

/*
=================
=
= VL_SetColor
=
=================
*/

void VL_SetColor	(int color, int red, int green, int blue)
{
	outportb (PAL_WRITE_REG,color);
	outportb (PAL_DATA_REG,red);
	outportb (PAL_DATA_REG,green);
	outportb (PAL_DATA_REG,blue);
}

//===========================================================================

/*
=================
=
= VL_GetColor
=
=================
*/

void VL_GetColor	(int color, int *red, int *green, int *blue)
{
	outportb (PAL_READ_REG,color);
	*red = inportb (PAL_DATA_REG);
	*green = inportb (PAL_DATA_REG);
	*blue = inportb (PAL_DATA_REG);
}

//===========================================================================

/*
=================
=
= VL_SetPalette
=
= If fast palette setting has been tested for, it is used
= (some cards don't like outsb palette setting)
=
=================
*/

void VL_SetPalette (byte far *palette, video_t *v)
{
//	int	i;
	boolean fastpalette;
	fastpalette=v->fastpalette;

//	outportb (PAL_WRITE_REG,0);
//	for	(i=0;i<768;i++)
//		outportb(PAL_DATA_REG,*palette++);

	__asm {
		mov	dx,PAL_WRITE_REG
		mov	al,0
		out	dx,al
		mov	dx,PAL_DATA_REG
		lds	si,[palette]

		test	[ss:fastpalette],1
		jz	slowset
//
// set palette fast for	cards that can take it
//
		//mov	cx,768
		//rep outsb
		//jmp	done

//
// set palette slowly for	some video cards
//
#ifdef __BORLANDC__
	}
#endif
slowset:
#ifdef __BORLANDC__
	__asm {
#endif
		mov	cx,256
#ifdef __BORLANDC__
	}
#endif
setloop:
#ifdef __BORLANDC__
	__asm {
#endif
		lodsb
		out	dx,al
		lodsb
		out	dx,al
		lodsb
		out	dx,al
		loop	setloop
#ifdef __BORLANDC__
	}
#endif
done:
#ifdef __BORLANDC__
	__asm {
#endif
		mov	ax,ss
			mov	ds,ax
	}
	v->fastpalette=fastpalette;
}


//===========================================================================

/*
=================
=
= VL_GetPalette
=
= This does not use the port string instructions,
= due to some incompatabilities
=
=================
*/

void VL_GetPalette (byte far *palette)
{
	int	i;

	outportb (PAL_READ_REG,0);
	for	(i=0;i<768;i++)
		*palette++ = inportb(PAL_DATA_REG);
}


//===========================================================================

/*
=================
=
= VL_FadeOut
=
= Fades the current palette to the given color	in	the given number of steps
=
=================
*/

void VL_FadeOut (int start, int end, int red, int green, int blue, int steps, video_t *v)
{
	int		i,j,orig,delta;
	byte	far *origptr, far *newptr;

	VL_WaitVBL(1);
	VL_GetPalette (&v->palette1[0][0]);
	_fmemcpy (v->palette2,v->palette1,PALSIZE);

//
// fade through intermediate frames
//
	for	(i=0;i<steps;i++)
	{
		origptr = &v->palette1[start][0];
		newptr = &v->palette2[start][0];
		for	(j=start;j<=end;j++)
		{
			orig = *origptr++;
			delta = red-orig;
			*newptr++ = orig + delta * i / steps;
			orig = *origptr++;
			delta = green-orig;
			*newptr++ = orig + delta * i / steps;
			orig = *origptr++;
			delta = blue-orig;
			*newptr++ = orig + delta * i / steps;
		}

		VL_WaitVBL(1);
		VL_SetPalette (&v->palette2[0][0], v);
	}

//
// final color
//
	VL_FillPalette (red,green,blue);

	v->screenfaded = true;
}


/*
=================
=
= VL_FadeIn
=
=================
*/

void VL_FadeIn (int start, int end, byte far *palette, int steps, video_t *v)
{
	int		i,j,delta;

	VL_WaitVBL(1);
	VL_GetPalette (&v->palette1[0][0]);
	_fmemcpy (&v->palette2[0][0],&v->palette1[0][0],sizeof(v->palette1));

	start *= 3;
	end = end*3+2;

//
// fade through intermediate frames
//
	for	(i=0;i<steps;i++)
	{
		for	(j=start;j<=end;j++)
		{
			delta = palette[j]-v->palette1[0][j];
			v->palette2[0][j] = v->palette1[0][j] + delta * i / steps;
		}

		VL_WaitVBL(1);
		VL_SetPalette (&v->palette2[0][0], v);
	}

//
// final color
//
	VL_SetPalette (palette, v);
	v->screenfaded = false;
}



/*
=================
=
= VL_TestPaletteSet
=
= Sets the palette with outsb, then reads it in	and	compares
= If it compares ok, fastpalette is set to true.
=
=================
*/

void VL_TestPaletteSet (video_t *v)
{
	int	i;

	for	(i=0;i<768;i++)
		v->palette1[0][i] = i;

	v->fastpalette = true;
	VL_SetPalette (&v->palette1[0][0], v);
	VL_GetPalette (&v->palette2[0][0]);
	if (_fmemcmp (&v->palette1[0][0],&v->palette2[0][0],768))
		v->fastpalette = false;
}

/*
==================
=
= VL_ColorBorder
=
==================
*/

void VL_ColorBorder (int color, video_t *v)
{
	union REGS in, out;

	in.h.ah = 0x10;
	in.h.al = 0x01;
	in.h.bh = color;
	int86(0x10, &in, &out);
	v->bordercolor = color;
}


/*
=============================================================================

							PIXEL OPS

=============================================================================
*/

//byte	rightmasks[4] = {1,3,7,15};

/*
=================
=
= VL_Plot
=
=================
*/

void VL_Plot (int x, int y, int color, global_game_variables_t *gvar)
{
	byte mask;
	VCLIPDEF

	if(!gvar->video.VL_Started) return;

	mask = pclip[x&3];
	VGAMAPMASK(mask);
	//*(byte far *)MK_FP(SCREENSEG,gvar->video.ofs.bufferofs+(gvar->video.ofs.ylookup[y]+(x>>2))) = color;
	//*(byte far *)MK_FP(SCREENSEG,gvar->video.ofs.bufferofs+((y*VGASTRIDEVARIABLE)+(x>>2))) = color;
	*(byte far *)MK_FP(SCREENSEG,BDOFSCONV gvar->video.BOFS+((y*VGASTRIDEVARIABLE)+(x>>2))) = color;
	VGAMAPMASK(15);
}


/*
=================
=
= VL_Hlin
=
=================
*/

void VL_Hlin	(unsigned x, unsigned y, unsigned width, unsigned color, global_game_variables_t *gvar)
{
	unsigned		xbyte;
	byte			far *dest;
	byte			leftmask,rightmask;
	int				midbytes;

	LRCLIPDEF

	if(!gvar->video.VL_Started) return;

	xbyte = x>>2;
	leftmask = lclip[x&3];
	rightmask = rclip[(x+width-1)&3];
	midbytes = ((x+width+3)>>2) - xbyte - 2;

	//dest = MK_FP(SCREENSEG,gvar->video.ofs.bufferofs+gvar->video.ofs.ylookup[y]+xbyte);
	dest = MK_FP(SCREENSEG,BDOFSCONV gvar->video.BOFS+(y*VGASTRIDEVARIABLE)+xbyte);

	if (midbytes<0)
	{
	// all in	one byte
		VGAMAPMASK(leftmask&rightmask);
		*dest = color;
		VGAMAPMASK(15);
		return;
	}

	VGAMAPMASK(leftmask);
	*dest++ = color;

	VGAMAPMASK(15);
	_fmemset (dest,color,midbytes);
	dest+=midbytes;

	VGAMAPMASK(rightmask);
	*dest = color;

	VGAMAPMASK(15);
}


/*
=================
=
= VL_Vlin
=
=================
*/

void VL_Vlin	(int x, int y, int height, int color, global_game_variables_t *gvar)
{
	byte	far *dest,mask;
	VCLIPDEF

	if(!gvar->video.VL_Started) return;

	mask = pclip[x&3];
	VGAMAPMASK(mask);

	//dest = MK_FP(SCREENSEG,gvar->video.ofs.bufferofs+gvar->video.ofs.ylookup[y]+(x>>2));
	dest = MK_FP(SCREENSEG,BDOFSCONV gvar->video.BOFS+(y*VGASTRIDEVARIABLE)+(x>>2));

	while (height--)
	{
		*dest = color;
		//dest += gvar->video.ofs.linewidth;
		dest += VGASTRIDEVARIABLE;
	}

	VGAMAPMASK(15);
}


/*
=================
=
= VL_Bar
=
=================
*/

void VL_Bar (int x, int y, int width, int height, int color, global_game_variables_t *gvar)
{
	byte	far *dest;
	byte	leftmask,rightmask;
	int		midbytes,linedelta;

	LRCLIPDEF

	if(!gvar->video.VL_Started) return;

	leftmask = lclip[x&3];
	rightmask = rclip[(x+width-1)&3];
	midbytes = ((x+width+3)>>2) - (x>>2) - 2;
	//linedelta = gvar->video.ofs.linewidth-(midbytes+1);
	linedelta = VGASTRIDEVARIABLE-(midbytes+1);

	//dest = MK_FP(SCREENSEG,gvar->video.ofs.bufferofs+gvar->video.ofs.ylookup[y]+(x>>2));
	dest = MK_FP(SCREENSEG,BDOFSCONV gvar->video.BOFS+(y*VGASTRIDEVARIABLE)+(x>>2));

	if (midbytes<0)
	{
	// all in	one byte
		VGAMAPMASK(leftmask&rightmask);
		while (height--)
		{
			*dest = color;
			//dest += gvar->video.ofs.linewidth;
			dest += VGASTRIDEVARIABLE;
		}
		VGAMAPMASK(15);
		return;
	}

	while (height--)
	{
		VGAMAPMASK(leftmask);
		*dest++ = color;

		VGAMAPMASK(15);
		_fmemset (dest,color,midbytes);
		dest+=midbytes;

		VGAMAPMASK(rightmask);
		*dest = color;

		dest+=linedelta;
	}

	VGAMAPMASK(15);
}

//==========================================================================

/*
=================
=
= VL_MemToScreen
=
= Draws a block of data to the screen.
=
=================
*/

void VL_MemToScreen (byte far *source, int width, int height, int x, int y, global_game_variables_t *gvar)
{
	byte    far *screen,far *dest,mask;
	int		plane;

	width>>=2;
	//dest = MK_FP(SCREENSEG,gvar->video.ofs.bufferofs+gvar->video.ofs.ylookup[y]+(x>>2));
	dest = MK_FP(SCREENSEG,BDOFSCONV gvar->video.BOFS+(y*VGASTRIDEVARIABLE)+(x>>2));
	mask = 1 << (x&3);

	for	(plane = 0; plane<4; plane++)
	{
		VGAMAPMASK(mask);
		mask <<= 1;
		if (mask == 16)
			mask = 1;

		screen = dest;
		//for	(y=0;y<height;y++,screen+=gvar->video.ofs.linewidth,source+=width)
		for	(y=0;y<height;y++,screen+=VGASTRIDEVARIABLE,source+=width)
			_fmemcpy (screen,source,width);
	}
}

//==========================================================================

/*
==============

 VL_WaitVBL			******** NEW *********

 Wait for	the vertical retrace (returns before the actual vertical sync)

==============
*/

void VL_WaitVBL(word num)
{
//PROC	VL_WaitVBL  num:WORD
//PUBLIC	VL_WaitVBL
#ifdef __WATCOMC__
	__asm {
#endif
	wait:
#ifdef __BORLANDC__
	__asm {
#endif

		mov	dx,STATUS_REGISTER_1

		mov	cx,[num]
	//
	// wait for	a display signal to make sure the raster isn't in	the middle
	// of a sync
	//
#ifdef __BORLANDC__
	}
#endif
	waitnosync:
#ifdef __BORLANDC__
	__asm {
#endif
		in	al,dx
		test	al,8
		jnz	waitnosync


#ifdef __BORLANDC__
	}
#endif
	waitsync:
#ifdef __BORLANDC__
	__asm {
#endif
		in	al,dx
		test	al,8
		jz	waitsync

		loop	waitnosync

		ret
	}
}

//===========================================================================
#if 0
#define VGAWRITEMODE(x) asm{
		cli
		mov	dx,GC_INDEX
		mov	al,GC_MODE
		out	dx,al
		inc	dx
		in	al,dx
		and	al,252
		or	al,x
		out	dx,al
		sti
}

#define VGAMAPMASK(x) asm{
		cli
		mov	dx,SC_INDEX
		mov	al,SC_MAPMASK
		mov	ah,x
		out	dx,ax
		sti
}

#define VGAREADMAP(x) asm{
		cli
		mov	dx,GC_INDEX
		mov	al,GC_READMAP
		mov	ah,x
		out	dx,ax
		sti
}

#define EGABITMASK(x) asm{
		mov	dx,GC_INDEX
		mov	ax,GC_BITMASK+256*x
		out	dx,ax
		sti
}
#endif
void VGAWRITEMODE(byte x)
{
	__asm {
		cli
		mov	dx,GC_INDEX
		mov	al,GC_MODE
		out	dx,al
		inc	dx
		in	al,dx
		and	al,252
		or	al,x
		out	dx,al
		sti
	}
}

void VGAMAPMASK(byte x)
{
	__asm {
		cli
		mov	dx,SC_INDEX
		mov	al,SC_MAPMASK
		mov	ah,x
		out	dx,ax
		sti
	}
}

void VGAREADMAP(byte x)
{
	__asm {
		cli
		mov	dx,GC_INDEX
		mov	al,GC_READMAP
		mov	ah,x
		out	dx,ax
		sti
	}
}

void VGABITMASK(byte x)
{
	word q = 256*x;
	__asm {
		mov	dx,GC_INDEX
		mov	ax,GC_BITMASK+q
		out	dx,ax
		sti
	}
}

//===========================================================================
