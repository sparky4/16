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

#include "src/lib/16_vlpal.h"

//color checker~
//i want to make another vesion that checks the palette when the palette is being appened~
void modexchkcolor(imgtestpal_t *bmp, word *q, word *a, word *aa, word *z, word *i/*, word *offset*/)
{
		byte *pal;
		word zz=0;
		pal = modexNewPal();
		modexPalSave(pal);
		CHKCOLDBGOUT1
		//check palette for dups
		for(; (*z)<PAL_SIZE-3; (*z)+=3)
		{
			CHKCOLDBGOUT2
			//if((*z)%3==0)
			//{
//----			  if(pal[(*z)]==pal[(*z)+3] && pal[(*z)+1]==pal[(*z)+4] && pal[(*z)+2]==pal[(*z)+5])
				if((*z)==(*i))
				{
					CHKCOLDBGOUT3
//0000				  (*z)-=3;
					break;
				}
				else for(zz=0; zz<(*q); zz+=3)
				{
					CHKCOLDBGOUT4
					if(zz%3==0)
					{
						if(pal[((*z)+(*q))]==pal[((*z)+(*q))+3] && pal[((*z)+(*q))+1]==pal[((*z)+(*q))+4] && pal[((*z)+(*q))+2]==pal[((*z)+(*q))+5])	//break if duplicate colors found in palette because it have reached the end of the current data of the palette
						{
//							  (*z)-=3;
//							  (*i)-=3;
							CHKCOLDBGOUT5
							break;
						}
						else if(pal[zz]==pal[((*z)+(*q))] && pal[zz+1]==pal[((*z)+(*q))+1] && pal[zz+2]==pal[((*z)+(*q))+2])
						{
							CHKCOLDBGOUT6
//++++						  (*i)--;
//							  (*z)--;
							//expand dong here
/*
planned features that i plan to implement~
image that has values on the pallete list!
wwww
no... wait.... no wwww
*/
							//for(zzii=0; zzii<3; zzii++)
							//{
								CHKCOLDBGOUT7
								a[(((*z)+(*q)))]=zz;
							//}
							(*aa)=(((*z)+(*q)));
							CHKCOLDBGOUT8
						}
						/*else
						{
							printf("================\n");
							printf("zq: %d  [%02d][%02d][%02d]\n", ((*z)+(*q))/3, pal[((*z)+(*q))], pal[((*z)+(*q))+1], pal[((*z)+(*q))+2]);
							printf("zz: %d  [%02d][%02d][%02d]\n", (zz)/3, pal[zz], pal[zz+1], pal[zz+2]);
							printf("z : %d  [%02d][%02d][%02d]\n", (*z)/3, pal[(*z)], pal[(*z)+1], pal[(*z)+2]);
							printf("================\n");
						}*/
						CHKCOLDBGOUT9
					}
				}
		}
		CHKCOLDBGOUT10
		//free(pal);
}

void
VL_palette(imgtestpal_t *bmp, byte *p, word *i, word qp, word aqoffset)
{
	//byte *p = bmp->palette;
	word w=0;
	word q=0;
	word qq=0;
	static word a[PAL_SIZE-3];	//palette array of change values!
	word z=0, aq=0, aa=0, pp=0;

	//modexWaitBorder();
	vga_wait_for_vsync();
	if((*i)==0)
	{
		memset(a, -1, sizeof(a));
		outp(PAL_WRITE_REG, 1);  /* start at the beginning of palette */
	}
	else if(qp==0)
	{
		q=(*i);
	}
	else
	{
		q=(*i);
		qq=(*i)/3;
#ifdef BEVERBOSEPALCHECK
		printf("q: %02d\n", (q));//
		printf("qq: %02d\n", (qq));//
		printf("	  (*i)-q=%02d\n", (*i)-q);//
#endif
		outp(PAL_WRITE_REG, qq);
	}
	if((*i)<PAL_SIZE-3 && w==0)
	{
		for(; (*i)<PAL_SIZE-3; (*i)++)
		{
			//if(i%3==0 && (p[i+5]==p[i+4] && p[i+4]==p[i+3] && p[i+3]==p[i+2] && p[i+2]==p[i+1] && p[i+1]==p[i] && p[i+5]==p[i]))
//____		  if((qp>0)&&((*i)-q)%3==0 && (p[((*i)-q)]==p[((*i)-q)+3] && p[((*i)-q)+1]==p[((*i)-q)+4] && p[((*i)-q)+2]==p[((*i)-q)+5])) outp(PAL_DATA_REG, p[(*i)-q]); else
			if(((((*i)-q)%3==0)) && (p[((*i)-q)]==p[((*i)-q)+3] && p[((*i)-q)+1]==p[((*i)-q)+4] && p[((*i)-q)+2]==p[((*i)-q)+5]))
			{
				w++;
				break;
			}
			else if(qp>0 && (*i)>=(qp) && (*i)<((qp)+3))
			{
#ifdef BEVERBOSEPALCHECK
				printf("qp=%d\n", qp);//
				printf("		  (*i)=%d a[%d]=%d\n", (*i), qp, a[qp]);//
				printf("		%d's color=%d\n", (*i), (a[qp])-(bmp->offset*3)+qp);//
#endif
				outp(PAL_DATA_REG, p[((a[qp])-(bmp->offset*3)+qp)]);// fix this shit!
				if((*i)+1==(qp)+3){ w++; /*(*i)++;*/ break; }
			}
			else
			{
				if(bmp->offset==0 && (*i)<3 && q==0) outp(PAL_DATA_REG, 0);
				else
				if(qp==0) outp(PAL_DATA_REG, p[(*i)-q]);
				else
				{ outp(PAL_DATA_REG, p[((*i)-(bmp->offset*3)+qp)]);
#ifdef BEVERBOSEPALCHECK
				printf("p[]=%d  qp=%d   p[]-qp=%d\n", ((*i)-(bmp->offset*3)), qp, ((*i)-(bmp->offset*3))+qp);
#endif
				}
			}
		}
#ifdef BEVERBOSEPALCHECK
		if(qp>0) printf("qp=%d\n", qp);//
		if(qp>0) printf("						 (*i)=%d\n", (*i)/3);//
		printf("						  (*i)=%d\n", (*i)/3);
#endif
	}

#ifdef BEVERBOSEPALCHECK
	printf("\nqqqqqqqq\n\n");
#endif

	//palette checker~
	if(q>0 && qp==0)
	{
		long lq;
		long bufSize = (bmp->width * bmp->height);
		pp = q;
#ifdef BEVERBOSEPALCHECK
		printf("1(*i)=%02d\n", (*i)/3);//
		printf("1z=%02d\n", z/3);//
#endif
		modexchkcolor(bmp, &q, &a, &aa, &z, i);
#ifdef BEVERBOSEPALCHECK
		printf("2(*i)=%02d\n", (*i)/3);
		printf("2z=%02d\n", z/3);
#endif
		aq=0;
aqpee:
		while(aq<=aa)
		{
#ifdef BEVERBOSEPALCHECK
			printf("a[%02d]=(%d)\n", aq, a[aq]);//
#endif
			if(a[aq]==-1) aq++;
			else { aqoffset++; break; }
		}
//update the image data here!
	for(lq=0; lq<bufSize; lq++)
	{
				/*
									note to self
									use a[qp] instead of bmp->offset for this spot!
									NO! wwww
				*/

				/*
				Facking bloody point the values of the changed palette to correct values.... major confusion! wwww
				*/

		//(offset/bmp->offset)*bmp->offset

#ifdef BEVERBOSEPALCHECK
		//printf("%02d ",bmp->data[lq]+bmp->offset);//
		//if(lq > 0 && lq%bmp->width==0) printf("\n");//
		//printf("%02d_", bmp->data[lq]+bmp->offset);//
#endif
		/*if(bmp->data[lq]+bmp->offset==aq)
		{
#ifdef BEVERBOSEPALCHECK
			printf("%02d", bmp->data[lq]);//
			printf("\n%02d\n", bmp->offset);//
			printf("aq=%02d ", aq);
			printf("a[aq]=%02d	  ", a[aq]);
			printf("a[aq]+aqpp=%02d ", a[aq]+aqpp);
			printf("a[aq]-aqpp=%02d\n", a[aq]-aqpp);
#endif
			//bmp->data[lq]=((bmp->data[lq]+bmp->offset)-a[aq]);
//++++		  bmp->data[lq]=a[aq]-aqpp;
#ifdef BEVERBOSEPALCHECK
			  printf("_%d ", bmp->data[lq]);//
			if(lq > 0 && lq%bmp->width==0) printf("\n");//
#endif
		}
		else if(bmp->data[lq]+bmp->offset < ((*i)/3)-aqpp)
		{
			if(bmp->data[lq]+bmp->offset >= aq)
			{
				bmp->data[lq]=(bmp->data[lq]+bmp->offset)-aqpp;//-((z-(*i))/3);
#ifdef BEVERBOSEPALCHECK
				printf("_%d ", bmp->data[lq]+bmp->offset)-aqpp-((z-(*i))/3);//
#endif
			}
			else bmp->data[lq]+=(bmp->offset-aqpp);
		}*/
#ifdef BEVERBOSEPALCHECK
		//printf("%02d`", bmp->data[lq]);//
		//if(lq > 0 && lq%bmp->width==0) printf("\n");//
#endif
	}

#ifdef BEVERBOSEPALCHECK
printf("		  aq=%02d\n", aq);//
printf("		  aa=%02d\n", aa);//
#endif
	//update the palette~
	VL_palette(bmp, p, &pp, aq, aqoffset);
	(*i)=pp;

	if(aq<aa){ pp=q; aq++; goto aqpee; }
	}
}
