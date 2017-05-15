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

#include "src/lib/bakapee.h"

//static word far* clockw= (word far*) 0x046C; /* 18.2hz clock */
//char global_temp_status_text[512];
//char global_temp_status_text2[512];

//==========================================================================

/*
===================
=
= FizzleFade
=
= returns true if aborted
=
===================
*/

//extern	ControlInfo	c;
#define PIXPOSX			gvar->video.page[0].sw/2
#define PIXPOSY			gvar->video.page[0].sh/2

/*
===================
=
= FizzleFade
=
= returns true if aborted
=
===================
*/

//extern	ControlInfo	c;
//#define PIXPOSX			gvar->video.page[0].sw/2
//#define PIXPOSY			gvar->video.page[0].sh/2
#ifdef BAKAFIZZUNSIGNED
boolean baka_FizzleFade (unsigned source, unsigned dest, unsigned width, unsigned height, unsigned frames, boolean abortable, global_game_variables_t *gvar)
{
	word		p,pixperframe;
	unsigned	drawofs,pagedelta;
	byte 		mask,maskb[8] = {1,2,4,8};
	unsigned	x,y,frame;
	long		rndval;
	word		screenseg;
#ifdef __WATCOMC__
	unsigned	esorig;//,q;
#endif

	pagedelta = dest-source;
	rndval = 1;
#ifdef __WATCOMC__
	esorig = 0;// q = 16;
#endif
	x = y = 0;
	pixperframe = (dword)(gvar->video.page[0].width*gvar->video.page[0].height)/frames;//64000/(dword)frames;
	screenseg = SCREENSEG;

//	IN_StartAck (gvar);

//	modexClearRegion(&(gvar->video.page[0]), 0, 0, gvar->video.page[0].width, gvar->video.page[0].height, 0);
//	modexClearRegion(&(gvar->video.page[1]), 0, 0, gvar->video.page[0].width, gvar->video.page[0].height, 15);

#ifdef __WATCOMC__
	__asm {
		mov	[esorig],es
	}
#endif
//	TimeCount=
	frame=0;
	do	// while (1)
	{
		if (abortable && kbhit())//IN_CheckAck (gvar) )
			return true;

		__asm {
			mov	es,[screenseg]
		}

		for (p=0;p<pixperframe;p++)
		{
			__asm {
				//
				// seperate random value into x/y pair
				//
				mov	ax,[WORD PTR rndval]
				mov	dx,[WORD PTR rndval+2]
				mov	bx,ax
				dec	bl
				mov	[BYTE PTR y],bl			// low 8 bits - 1 = y xoordinate
				mov	bx,ax
				mov	cx,dx
				mov	[BYTE PTR x],ah			// next 9 bits = x xoordinate
				mov	[BYTE PTR x+1],dl
				//
				// advance to next random element
				//
				shr	dx,1
				rcr	ax,1
				jnc	noxor
				xor	dx,0x0001
				xor	ax,0x2000
#ifdef __BORLANDC__
			}
#endif
noxor:
#ifdef __BORLANDC__
			__asm {
#endif
				mov	[WORD PTR rndval],ax
				mov	[WORD PTR rndval+2],dx
			}

			if (x>width || y>height)
//			if ((x>width || y>height) && (x<width*2 && y<height*2))
				continue;
//			drawofs = source+(gvar->video.ofs.ylookup[y]) + (x>>2);
			drawofs = source+(y*gvar->video.page[0].stridew) + (x>>2);
/*
sprintf(global_temp_status_text, "draw - %Fp", drawofs);
modexprint(&(gvar->video.page[0]), PIXPOSX, q+PIXPOSY, 1, 0, 47, 0, 1, global_temp_status_text); q+=8;
sprintf(global_temp_status_text, "pdet - %Fp", pagedelta);
modexprint(&(gvar->video.page[0]), PIXPOSX, q+PIXPOSY, 1, 0, 47, 0, 1, global_temp_status_text); q+=8;
sprintf(global_temp_status_text, "srce - %Fp", source);
modexprint(&(gvar->video.page[0]), PIXPOSX, q+PIXPOSY, 1, 0, 47, 0, 1, global_temp_status_text); q+=8;
sprintf(global_temp_status_text, "dest - %Fp", dest);
modexprint(&(gvar->video.page[0]), PIXPOSX, q+PIXPOSY, 1, 0, 47, 0, 1, global_temp_status_text); q=16;
*/

			//
			// copy one pixel
			//
			mask = x&3;
			VGAREADMAP(mask);
			mask = maskb[mask];
			VGAMAPMASK(mask);

			__asm {
				mov	di,[drawofs]
				mov	al,[es:di]
				add	di,[pagedelta]
				mov	[es:di],al
			}

			if (rndval == 1)		// entire sequence has been completed
				return false;
		}
		frame++;
//--		while (TimeCount<frame){}//;		// don't go too fast
		delay(1);
	} while (1);
#ifdef __WATCOMC__
	__asm {
		mov	es,[esorig]
	}
	return false;
#endif
}
#endif
#if 0
boolean baka_FizzleFade (page_t *sourcepage, page_t *destpage, unsigned width, unsigned height, unsigned frames, boolean abortable, global_game_variables_t *gvar)
{
	word		p,pixperframe;
	unsigned	drawofs,pagedelta;
//	byte 		mask,maskb[8] = {1,2,4,8};
	unsigned	x,y,frame		,esorig,q;
	dword		rndval;
	unsigned	source,dest;
	word screenseg = SCREENSEG;
	source = ((word)sourcepage->data); dest = ((word)destpage->data);

	pagedelta = dest-source;
	rndval = 1;	esorig = 0; q = 16;
	x = y = 0;//destpage->dx;
	pixperframe = 76800/(dword)frames;

//	IN_StartAck ();

//	VL_ShowPage(&(gvar->video.page[0]), 1, 0);
//	VL_ShowPage(&(gvar->video.page[1]), 1, 0);

//	modexClearRegion(&(gvar->video.page[0]), 0, 0, gvar->video.page[0].width, gvar->video.page[0].height, 0);
//	modexClearRegion(&(gvar->video.page[1]), 0, 0, gvar->video.page[0].width, gvar->video.page[0].height, 15);
//	VL_SetLineWidth(80, gvar);

	__asm {
		mov	[esorig],es
	}
//	TimeCount=
	frame=0;
	do	// while (1)
	{
sprintf(global_temp_status_text, "%u", frame);
modexprint(destpage, PIXPOSX, PIXPOSY, 1, 0, 47, 0, 1, global_temp_status_text);
		if (abortable && kbhit())//IN_CheckAck () )
			return true;

		__asm {
			mov	es,[screenseg]
		}

		for (p=0;p<pixperframe;p++)
		{
			__asm {
				//
				// seperate random value into x/y pair
				//
				mov	ax,[WORD PTR rndval]
				mov	dx,[WORD PTR rndval+2]
				mov	bx,ax
				dec	bl
				mov	[BYTE PTR y],bl			// low 8 bits - 1 = y xoordinate
				mov	bx,ax
				mov	cx,dx
				mov	[BYTE PTR x],ah			// next 9 bits = x xoordinate
				mov	[BYTE PTR x+1],dl
				//
				// advance to next random element
				//
				shr	dx,1
				rcr	ax,1
				jnc	noxor
				xor	dx,0x0001
				xor	ax,0x2000
#ifdef __BORLANDC__
			}
#endif
noxor:
#ifdef __BORLANDC__
			__asm {
#endif
				mov	[WORD PTR rndval],ax
				mov	[WORD PTR rndval+2],dx
			}

//			if (x>destpage->width || y>destpage->height)
//			if (x<destpage->sw || y<destpage->sh)
			if ((x>width || y>height) && (x<width*2 && y<height*2))
				continue;
			drawofs = source+(gvar->video.ofs.ylookup[y]) + (x>>2);
sprintf(global_temp_status_text, "draw - %Fp", drawofs);
modexprint(destpage, PIXPOSX, q+PIXPOSY, 1, 0, 47, 0, 1, global_temp_status_text); q+=8;
sprintf(global_temp_status_text, "pdet - %Fp", pagedelta);
modexprint(destpage, PIXPOSX, q+PIXPOSY, 1, 0, 47, 0, 1, global_temp_status_text); q+=8;
sprintf(global_temp_status_text, "srcp - %Fp", sourcepage->data);
modexprint(destpage, PIXPOSX, q+PIXPOSY, 1, 0, 47, 0, 1, global_temp_status_text); q+=8;
sprintf(global_temp_status_text, "desp - %Fp", destpage->data);
modexprint(destpage, PIXPOSX, q+PIXPOSY, 1, 0, 47, 0, 1, global_temp_status_text); q+=8;
sprintf(global_temp_status_text, "srce - %Fp", source);
modexprint(destpage, PIXPOSX, q+PIXPOSY, 1, 0, 47, 0, 1, global_temp_status_text); q+=8;
sprintf(global_temp_status_text, "dest - %Fp", dest);
modexprint(destpage, PIXPOSX, q+PIXPOSY, 1, 0, 47, 0, 1, global_temp_status_text); q=16;

			//
			// copy one pixel
			//
/*
			mask = x&3;
			VGAREADMAP(mask);
			mask = maskb[mask];
			VGAMAPMASK(mask);
/*/
//			modexputPixel(&(gvar->video.page[0]), x, y, rand());
//			VL_Plot (x, y, 15, &(gvar->video.ofs));
			modexCopyPageRegion(destpage, sourcepage, x, y, x, y, 4, 4);

			__asm {
				mov	di,[drawofs]
				mov	al,[es:di]
				add	di,[pagedelta]
				mov	[es:di],al
			}

			if (rndval == 1)		// entire sequence has been completed
				return false;
		}
		frame++;
//--		while (TimeCount<frame){}//;		// don't go too fast
	} while (1);
	__asm {
		mov	es,[esorig]
	}
	return false;
}
#endif

#if 0
boolean baka_FizzleFade (page_t *sourcepage, page_t *destpage, unsigned width, unsigned height, unsigned frames, boolean abortable, global_game_variables_t *gvar)
{
	word		p,pixperframe;
	unsigned	drawofs,pagedelta;
	byte 		mask,maskb[8] = {1,2,4,8};
	unsigned	x,y,frame		,esorig,q;
	dword		rndval;
	unsigned	source,dest;
//	word TimeCount = *clockw;
	word screenseg = SCREENSEG;
	source = ((word)sourcepage->data); dest = ((word)destpage->data);

	pagedelta = dest-source;//(word)(source->data - dest->data);//(dest->data - source->data)
	rndval = 1;	esorig = 0; q = 16;
	x = y = destpage->dx;
	pixperframe = /*76800*/64000/(dword)frames;

//	IN_StartAck ();

//	VL_ShowPage(destpage, 1, 0);
//	VL_ShowPage(sourcepage, 1, 0);

//	modexClearRegion(destpage, 0, 0, (destpage->width), (destpage->height), 12);
	modexClearRegion(sourcepage, 0, 0, (sourcepage->width), (sourcepage->height), 64);

	__asm {
		mov	[esorig],es
	}
//	TimeCount=
	frame=0;
	do	// while (1)
	{
sprintf(global_temp_status_text, "%u", frame);
modexprint(destpage, PIXPOSX, PIXPOSY, 1, 0, 47, 0, 1, global_temp_status_text);
		if (abortable && kbhit())//IN_CheckAck () )
			return true;

		__asm {
			mov	es,[screenseg]
		}

		for (p=0;p<pixperframe;p++)
		{
			__asm {
				//
				// seperate random value into x/y pair
				//
				mov	ax,[WORD PTR rndval]
				mov	dx,[WORD PTR rndval+2]
				mov	bx,ax
				dec	bl
				mov	[BYTE PTR y],bl			// low 8 bits - 1 = y xoordinate
				mov	bx,ax
				mov	cx,dx
				mov	[BYTE PTR x],ah			// next 9 bits = x xoordinate
				mov	[BYTE PTR x+1],dl
				//
				// advance to next random element
				//
				shr	dx,1
				rcr	ax,1
				jnc	noxor
				xor	dx,0x0001
				xor	ax,0x2000
#ifdef __BORLANDC__
			}
#endif
noxor:
#ifdef __BORLANDC__
			__asm {
#endif
				mov	[WORD PTR rndval],ax
				mov	[WORD PTR rndval+2],dx
			}

//			if (x>destpage->width || y>destpage->height)
//			if (x<destpage->sw || y<destpage->sh)
			if (x>width || y>height)
				continue;
			drawofs = source+(gvar->video.ofs.ylookup[y]) + (x>>2);
sprintf(global_temp_status_text, "draw - %Fp", drawofs);
modexprint(destpage, PIXPOSX, q+PIXPOSY, 1, 0, 47, 0, 1, global_temp_status_text); q+=8;
sprintf(global_temp_status_text, "pdet - %Fp", pagedelta);
modexprint(destpage, PIXPOSX, q+PIXPOSY, 1, 0, 47, 0, 1, global_temp_status_text); q+=8;
sprintf(global_temp_status_text, "srcp - %Fp", sourcepage->data);
modexprint(destpage, PIXPOSX, q+PIXPOSY, 1, 0, 47, 0, 1, global_temp_status_text); q+=8;
sprintf(global_temp_status_text, "desp - %Fp", destpage->data);
modexprint(destpage, PIXPOSX, q+PIXPOSY, 1, 0, 47, 0, 1, global_temp_status_text); q+=8;
sprintf(global_temp_status_text, "srce - %Fp", source);
modexprint(destpage, PIXPOSX, q+PIXPOSY, 1, 0, 47, 0, 1, global_temp_status_text); q+=8;
sprintf(global_temp_status_text, "dest - %Fp", dest);
modexprint(destpage, PIXPOSX, q+PIXPOSY, 1, 0, 47, 0, 1, global_temp_status_text); q=16;

			//
			// copy one pixel
			//
			mask = x&3;
			VGAREADMAP(mask);
			mask = maskb[mask];
			VGAMAPMASK(mask);

			__asm {
				mov	di,[drawofs]
				mov	al,[es:di]
				add	di,[pagedelta]
				mov	[es:di],al
			}

			if (rndval == 1)		// entire sequence has been completed
				return false;
		}
		frame++;
//--		while (TimeCount<frame){}//;		// don't go too fast
	} while (1);
	__asm {
		mov	es,[esorig]
	}
	return false;
}
#endif
// clrstdin() clear any leftover chars tha may be in stdin stream //
void clrstdin()
{
   int ch = 0;
   while( ( ch = getchar() ) != '\n' && ch != EOF );
}

//===========================================================================

/*
====================
=
= TL_DosLibStartup
=
====================
*/

void TL_DosLibStartup(global_game_variables_t *gvar)
{
	if(gvar->DLStarted)
		return;

	// DOSLIB: check our environment
	probe_dos();

	// DOSLIB: what CPU are we using?
	// NTS: I can see from the makefile Sparky4 intends this to run on 8088 by the -0 switch in CFLAGS.
	//	  So this code by itself shouldn't care too much what CPU it's running on. Except that other
	//	  parts of this project (DOSLIB itself) rely on CPU detection to know what is appropriate for
	//	  the CPU to carry out tasks. --J.C.
	cpu_probe();

	// DOSLIB: check for VGA
	if (!probe_vga()) {
		printf("VGA probe failed\n");
		return;
	}
	// hardware must be VGA or higher!
	if (!(vga_state.vga_flags & VGA_IS_VGA)) {
		printf("This program requires VGA or higher graphics hardware\n");
		return;
	}

	textInit();
	gvar->DLStarted = true;
}

//color ‚Ä‚·‚Æ
void colortest(page_t *page, bakapee_t *pee)
{
	//if(pee->coor < 256)
	//{
//		modexcls(page, pee->coor, VGA);
		VL_ClearVideo (pee->coor);
		pee->coor++;
	//}else pee->coor = 0;
}

//color ‚Ä‚·‚Æ
void colorz(page_t *page, bakapee_t *pee)
{
	if(pee->coor <= pee->hgq)
	{
//		modexcls(page, pee->coor, VGA);
		VL_ClearVideo (pee->coor);
		pee->coor++;
	}else pee->coor = pee->lgq;
}

//slow spectrum down
void ssd(page_t *page, bakapee_t *pee, word svq)
{
	if(pee->sy < page->sh+1)
	{
		if(pee->sx < page->sw+1)
		{
			//mxPutPixel(sx, sy, coor);
			//printf("%d %d %d %d\n", pee->sx, pee->sy, svq, pee->coor);
			dingpp(page, pee);
			pee->sx++;
		}else pee->sx = 0;
		if(pee->sx == page->sw)
		{
			pee->sy++;
			if(svq == 7) pee->coor++;
			if(pee->sy == page->sh && svq == 8) pee->coor = rand()%256;
		}
	}else pee->sy = 0;
}

//plot pixel or plot tile
void dingpp(page_t *page, bakapee_t *pee)
{
	if(pee->tile)
	{
		modexClearRegion(page, pee->xx, pee->yy, TILEWH, TILEWH, pee->coor);
	}
	else
		modexputPixel(page, pee->xx, pee->yy, pee->coor);
}

void dingo(page_t *page, bakapee_t *pee)
{
	if(pee->tile)
	{
		if(pee->xx<0) pee->xx=(page->sw-TILEWH);
		if(pee->yy<0) pee->yy=(page->sh-TILEWH);
		if(pee->xx>(page->sw-TILEWH)) pee->xx=0;
		if(pee->yy>(page->sh-TILEWH)/*+(TILEWH*BUFFMX)*/) pee->yy=0;
	}
		else
	{
		if(pee->xx<0) pee->xx=page->sw;
		if(pee->yy<0) pee->yy=page->sh;
		if(pee->xx>page->sw) pee->xx=0;
		if(pee->yy>page->sh) pee->yy=0;
	}
}

//assigning values from randomizer
void dingas(bakapee_t *pee)
{
	if(pee->gq == pee->bonk) dingu(pee);
	if(!pee->bakax)
	{
		if(pee->tile)
		pee->xx-=TILEWH;
		else pee->xx--;
	}
	else if(pee->bakax>1)
	{
		if(pee->tile)
		pee->xx+=TILEWH;
		else pee->xx++;
	}
	if(!pee->bakay)
	{
		if(pee->tile)
		pee->yy-=TILEWH;
		else pee->yy--;
	}
	else if(pee->bakay>1)
	{
		if(pee->tile)
		pee->yy+=TILEWH;
		else pee->yy++;
	}
}

void dingaso(bakapee_t *pee)
{
	if(pee->gq == pee->bonk) dingu(pee);
	if(!pee->bakax)
	{
		if(pee->tile)
		pee->xx-=TILEWH;
		else pee->xx--;
	}
	else
	{
		if(pee->tile)
		pee->xx+=TILEWH;
		else pee->xx++;
	}
	if(!pee->bakay)
	{
		if(pee->tile)
		pee->yy-=TILEWH;
		else pee->yy--;
	}
	else
	{
		if(pee->tile)
		pee->yy+=TILEWH;
		else pee->yy++;
	}
}

void dingu(bakapee_t *pee)
{
	if(pee->coor < pee->hgq && pee->coor < pee->lgq) pee->coor = pee->lgq;
	if(pee->coor < pee->hgq)
	{
		pee->coor++;
	}else{
		pee->coor = pee->lgq;
	}
}

//randomizer
void dingq(bakapee_t *pee)
{
	if(pee->gq<pee->bonk)
	{
		pee->gq++;
	}
	else
	{
		dingu(pee);
		pee->gq = 0;
	}
	pee->bakax = rand()%3; pee->bakay = rand()%3;
}

void dingqo(bakapee_t *pee)
{
	if(pee->gq<pee->bonk)
	{
		pee->gq++;
		pee->bakax = rand()%3; pee->bakay = rand()%3;
	}
	else
	{
		dingu(pee);
		pee->gq = 0;
	}
	//either one will do wwww --4
	pee->bakax = rand()&0x1; pee->bakay = rand()&0x1;
	//pee->bakax = rand()%2; pee->bakay = rand()%2;
}

/*-----------ding-------------*/
void ding(page_t *page, bakapee_t *pee, word q)
{
	word tx=0,ty=0;//d3y,

//++++  if(q <= 4 && q!=2 && gq == pee->bonk-1) coor = rand()%pee->hgq;
	switch(q)
	{
		case 1:/*
			dingq(pee);
			if(pee->xx==page->sw){pee->bakax=0;}
			if(pee->xx==0){pee->bakax=1;}
			if(pee->yy==page->sh){pee->bakay=0;}
			if(pee->yy==0){pee->bakay=1;}*/
			dingqo(pee);
			dingaso(pee);
			dingo(page, pee);
			dingpp(page, pee);	//plot the pixel/tile
			if(pee->tile)
			modexClearRegion(page, (rand()*TILEWH)%page->width, (rand()*TILEWH)%(page->height), TILEWH, TILEWH, 0);
			else
			modexputPixel(page, rand()%page->width, rand()%page->height, 0);
		break;
		case 2:
			dingq(pee);
			dingas(pee);
			dingo(page, pee);
			dingpp(page, pee);	//plot the pixel/tile
			if(pee->tile)
			modexClearRegion(page, (rand()*TILEWH)%page->width, (rand()*TILEWH)%(page->height), TILEWH, TILEWH, 0);
			else
			modexputPixel(page, rand()%page->width, rand()%page->height, 0);
		break;
		case 3:
			/*dingq(pee);
			if(pee->xx!=page->sw||pee->yy!=page->sh)
			{
				if(pee->xx==0){pee->bakax=1;pee->bakay=-1;d3y=1;}
				if(pee->yy==0){pee->bakax=1;pee->bakay=0;d3y=1;}
				if(pee->xx==page->sw){pee->bakax=-1;pee->bakay=-1;d3y=1;}
				if(pee->yy==page->sh){pee->bakax=1;pee->bakay=0;d3y=1;}
			}else if(pee->xx==page->sw&&pee->yy==page->sh) pee->xx=pee->yy=0;
			if(d3y)
			{
				if(pee->bakay<0)
				{
					pee->yy--;
					d3y--;
				}else
				if(pee->bakay>0)
				{
					pee->yy++;
					d3y--;
				}
			}
			if(pee->bakax<0)
			{
				pee->xx--;
			}else
			if(pee->bakax>0)
			{
				pee->xx++;
			}
			dingpp(page, pee);	//plot the pixel/tile*/
			dingqo(pee);
			dingaso(pee);
			dingo(page, pee);
			dingpp(page, pee);	//plot the pixel/tile
		break;
		case 4:
			dingq(pee);
			dingas(pee);
			dingo(page, pee);
			dingpp(page, pee);	//plot the pixel/tile
		break;
		case 5:
			colortest(page, pee);
		break;
		case 6:
// 			pee->coor = rand()%256;
// 			modexcls(page, pee->coor, VGA);
			colorz(page, pee);
			modexprint(page, page->sw/2, page->sh/2, 1, 0, 47, 0, 1, "bakapi");
		break;
		case 7:
			if(pee->coor <= pee->hgq)
			{
				ssd(page, pee, q);
				pee->coor++;
			}else pee->coor = pee->lgq;
		break;
		case 8:
			colorz(page, pee);
			modexprint(page, page->sw/2, page->sh/2, 1, 0, 47, 0, 1, "bakapi");
		break;
/*		case 9:
			modexClearRegion(&(ggvv->video.page[0]), 0, 0, ggvv->video.page[0].width/2, ggvv->video.page[0].height/2, 15);
#ifdef BAKAFIZZUNSIGNED
//			baka_FizzleFade (ggvv->video.ofs.bufferofs, ggvv->video.ofs.displayofs, vga_state.vga_width, vga_state.vga_height, 70, true, ggvv);
			baka_FizzleFade (ggvv->video.ofs.bufferofs, ggvv->video.ofs.displayofs, ggvv->video.page[0].width, ggvv->video.page[0].height, 70, true, ggvv);
#else
			baka_FizzleFade (&ggvv->video.page[1], &ggvv->video.page[0], vga_state.vga_width, vga_state.vga_height, 70, true, ggvv);
#endif
		break;*/
		case 10:
			ssd(page, pee, q); /*printf("%d\n", pee->coor);*/
		break;
		case 11:
			colorz(page, pee); delay(100);
		break;

		case 16:	//interesting effects
			dingq(pee);
			if(!pee->bakax){ pee->xx--;}
			else if(pee->bakax>0){ pee->xx++; }
			if(!pee->bakay){ pee->yy--;}
			else if(pee->bakay>0){ pee->yy++; }
			dingas(pee);
			tx+=pee->xx+TILEWH+4;
			ty+=pee->yy+TILEWH+4;
			modexClearRegion(page, tx, ty, 4, 4, pee->coor);
			if(pee->tile)
			modexClearRegion(page, (rand()*4)%page->width, (rand()*4)%(page->height), 4, 4, 0);
			else
			modexputPixel(page, rand()%page->width, rand()%(page->height), 0);
			//printf("%d %d %d %d %d %d\n", pee->xx, pee->yy, tx, ty, TILEWH);
		break;
		default:
		break;
	}
	//printf("	%dx%d	%dx%d\n", pee->xx, pee->yy, tx, ty);
	//pee->coor++;
}
