/* Project 16 Source Code~
 * Copyright (C) 2012-2015 sparky4 & pngwen & andrius4669
 *
 * This file is part of Project 16.
 *
 * Project 16 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as publipage->shed by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Project 16 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You page->should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "src/lib/bakapee.h"

//color ‚Ä‚·‚Æ
void colortest(page_t *page, bakapee_t *pee)
{
	//if(pee->coor < 256)
	//{
		cls(page, pee->coor, VGA);
		pee->coor++;
	//}else pee->coor = 0;
}

//color ‚Ä‚·‚Æ
void colorz(page_t *page, bakapee_t *pee)
{
	if(pee->coor <= HGQ)
	{
		cls(page, pee->coor, VGA);
		pee->coor++;
	}else pee->coor = LGQ;
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
#ifdef TILE
	//fill_block(pee->xx, pee->yy, pee->xx+TILEWH, pee->yy+TILEWH, pee->coor);
	//mxFillBox(pee->xx, pee->yy, TILEWH, TILEWH, pee->coor, OP_SET);
	modexClearRegion(page, pee->xx, pee->yy, TILEWH, TILEWH, pee->coor);
#else
	modexputPixel(page, pee->xx, pee->yy, pee->coor);
#endif
}

void dingo(page_t *page, bakapee_t *pee)
{
	#ifdef TILE
	if(pee->xx<0) pee->xx=(page->sw-TILEWH);
	if(pee->yy<0) pee->yy=(page->sh-TILEWH);
	if(pee->xx>(page->sw-TILEWH)) pee->xx=0;
	if(pee->yy>(page->sh-TILEWH)/*+(TILEWH*BUFFMX)*/) pee->yy=0;
	#else
	if(pee->xx<0) pee->xx=page->sw;
	if(pee->yy<0) pee->yy=page->sh;
	if(pee->xx>page->sw) pee->xx=0;
	if(pee->yy>page->sh) pee->yy=0;
	#endif
}

//assigning values from randomizer
void dingas(bakapee_t *pee)
{
	if(pee->gq == BONK) dingu(pee);
	if(!pee->bakax)
	{
		#ifdef TILE
		pee->xx-=TILEWH;
		#else
		pee->xx--;
		#endif
	}
	else if(pee->bakax>1)
	{
		#ifdef TILE
		pee->xx+=TILEWH;
		#else
		pee->xx++;
		#endif
	}
	if(!pee->bakay)
	{
		#ifdef TILE
		pee->yy-=TILEWH;
		#else
		pee->yy--;
		#endif
	}
	else if(pee->bakay>1)
	{
		#ifdef TILE
		pee->yy+=TILEWH;
		#else
		pee->yy++;
		#endif
	}
}

void dingu(bakapee_t *pee)
{
	if(pee->coor < HGQ && pee->coor < LGQ) pee->coor = LGQ;
	if(pee->coor < HGQ)
	{
		pee->coor++;
	}else{
		pee->coor = LGQ;
	}
}

//randomizer
void dingq(bakapee_t *pee)
{
	if(pee->gq<BONK)
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

/*-----------ding-------------*/
void ding(page_t *page, bakapee_t *pee, word q)
{
	word d3y, tx=0,ty=0;

//++++  if(q <= 4 && q!=2 && gq == BONK-1) coor = rand()%HGQ;
	switch(q)
	{
		case 1:
			dingq(pee);
			if(pee->xx==page->sw){pee->bakax=0;}
			if(pee->xx==0){pee->bakax=1;}
			if(pee->yy==page->sh){pee->bakay=0;}
			if(pee->yy==0){pee->bakay=1;}
		break;
		case 2:
			dingq(pee);
			dingas(pee);
			dingo(page, pee);
			dingpp(page, pee);	//plot the pixel/tile
#ifdef TILE
			modexClearRegion(page, (rand()*TILEWH)%page->width, (rand()*TILEWH)%(page->height), TILEWH, TILEWH, 0);
#else
			modexputPixel(page, rand()%page->width, rand()%page->height, 0);
#endif
		break;
		case 3:
			dingq(pee);
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
			pee->coor = rand()%256;
			cls(page, pee->coor, VGA);
		break;
		case 7:
			if(pee->coor <= HGQ)
			{
				ssd(page, pee, q);
				pee->coor++;
			}else pee->coor = LGQ;
		break;
		case 8:
			colorz(page, pee);
			modexprint(page, page->sw/2, page->sh/2, 1, 47, 0, "bakapi", 1);
		break;
		case 9:
			if(pee->coor <= HGQ)
			{
				ssd(page, pee, q);
				pee->coor++;
			}else pee->coor = LGQ;
		break;
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
#ifdef TILE
			modexClearRegion(page, (rand()*TILEWH)%page->width, (rand()*TILEWH)%(page->height), TILEWH, TILEWH, 0);
#else
			modexputPixel(page, rand()%page->width, rand()%(page->height), 0);
#endif
			//printf("%d %d %d %d %d %d\n", pee->xx, pee->yy, tx, ty, TILEWH);
		break;
		default:
		break;
	}
	//pee->coor++;
}
