/* Project 16 Source Code~
 * Copyright (C) 2012-2020 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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
void chkcolor(bitmap_t *bmp, word *q, word *a, word *aa, word *z)
{
		byte *pal;
		word zz=0;
		pal = modexNewPal();
		modexPalSave(pal);
		CHKCOLDBGOUT1
		//check palette for dups
		for((*z)=0; (*z)<PAL_SIZE; (*z)+=3)
		{
			CHKCOLDBGOUT2
			if((*z)%3==0)
			{
//=======
				if(pal[(*z)]==pal[(*z)+3] && pal[(*z)+1]==pal[(*z)+4] && pal[(*z)+2]==pal[(*z)+5])
				{
					CHKCOLDBGOUT3
//====
					break;
				}
				else for(zz=0; zz<(*q); zz+=3)
				{
					CHKCOLDBGOUT4
					if(zz%3==0)
					{
						if(pal[((*z)+(*q))]==pal[((*z)+(*q))+3] && pal[((*z)+(*q))+1]==pal[((*z)+(*q))+4] && pal[((*z)+(*q))+2]==pal[((*z)+(*q))+5])
						{
//====
//====
							CHKCOLDBGOUT5
							break;
						}
						else if(pal[zz]==pal[((*z)+(*q))] && pal[zz+1]==pal[((*z)+(*q))+1] && pal[zz+2]==pal[((*z)+(*q))+2])
						{
							CHKCOLDBGOUT6
							CHKCOLDBGOUT7
							a[((*z)+(*q))/3]=zz/3;
							(*aa)=((*z)+(*q))/3;
							CHKCOLDBGOUT8
						}
						/*else
						{
							printf("================\n");
							printf("zq: %d	[%02d][%02d][%02d]\n", ((*z)+(*q))/3, pal[((*z)+(*q))], pal[((*z)+(*q))+1], pal[((*z)+(*q))+2]);
							printf("zz: %d	[%02d][%02d][%02d]\n", (zz)/3, pal[zz], pal[zz+1], pal[zz+2]);
							printf("z : %d	[%02d][%02d][%02d]\n", (*z)/3, pal[(*z)], pal[(*z)+1], pal[(*z)+2]);
							printf("================\n");
						}*/
						CHKCOLDBGOUT9
					}
				}
			}
		}
		CHKCOLDBGOUT10
		//free(pal);
}

void
VL_modexPalUpdate(bitmap_t *bmp, word *i, word qp, word aqpp)
{
	byte *p = bmp->palette;
	word w=0;
	word q=0;
	word qq=0;
	word ii;
	static word a[256] = { 0 };
	word z=0,aq=0,aa=0;
	word pp=0,spee=0,ppee=0;
	sword aqpw;

//	if(qp>0) printf("(*i)=%02d\n", (*i));
	modexWaitBorder();
	if((*i)==0) outp(PAL_WRITE_REG, 0);  /* start at the beginning of palette */
	else if(qp==0)
	{
		q=(*i);
	}
	else
	{
		q=(*i);
		qq=(*i)/3;
//		printf("q: %02d\n", (q));
//		printf("qq: %02d\n", (qq));
		//printf("	(*i)-q=%02d\n", (*i)-q);
//		printf("================\n");
		outp(PAL_WRITE_REG, qq);  /* start at the beginning of palette */
	}
	if((*i)<PAL_SIZE && w==0)
	{
		for(; (*i)<PAL_SIZE; (*i)++)
		{
			//if(i%3==0 && (p[i+5]==p[i+4] && p[i+4]==p[i+3] && p[i+3]==p[i+2] && p[i+2]==p[i+1] && p[i+1]==p[i] && p[i+5]==p[i]))
//____			if((qp>0)&&((*i)-q)%3==0 && (p[((*i)-q)]==p[((*i)-q)+3] && p[((*i)-q)+1]==p[((*i)-q)+4] && p[((*i)-q)+2]==p[((*i)-q)+5])) outp(PAL_DATA_REG, p[(*i)-q]); else
			if(((((*i)-q)%3==0) || ((qp>0)&&((*i)-(bmp->offset*3))%3==0)) && (p[((*i)-q)]==p[((*i)-q)+3] && p[((*i)-q)+1]==p[((*i)-q)+4] && p[((*i)-q)+2]==p[((*i)-q)+5]))
			{
				if(qp>0)
				{
					(*i)-=(aqpp*3);
					aqpw=aqpp-1;
					outp(PAL_WRITE_REG, qq+(((*i)+(aqpw*3)-(bmp->offset*3))/3));
					for(ii=aqpp; ii>0; ii--)
					{
						outp(PAL_DATA_REG, p[((((*i)+((aqpp-ii)*3))+((aqpp+ii)*3))-(bmp->offset*3))]);
						outp(PAL_DATA_REG, p[((((*i)+((aqpp-ii)*3))+((aqpp+ii)*3)+1)-(bmp->offset*3))]);
						outp(PAL_DATA_REG, p[((((*i)+((aqpp-ii)*3))+((aqpp+ii)*3)+2)-(bmp->offset*3))]);
//						printf("position	=	%d\n", qq+(((*i)+(aqpw*3)-(bmp->offset*3))/3));
/*if(qp>0){ //printf("[%d]", p[((*i)-q)]);	printf("[%d]", p[((*i)-q)+1]);	printf("[%d]", p[((*i)-q)+2]);	printf("[%d]", p[((*i)-q)+3]);			printf("[%d]", p[((*i)-q)+4]);			printf("[%d]", p[((*i)-q)+5]);			printf("	%d [%d]\n", (*i), p[((*i)-q)]); }
printf("[%d]", p[((((*i)+((aqpp-ii)*3)))-(bmp->offset*3))]);
printf("[%d]", p[((((*i)+((aqpp-ii)*3))+1)-(bmp->offset*3))]);
printf("[%d] | ", p[((((*i)+((aqpp-ii)*3))+2)-(bmp->offset*3))]);
printf("[%d]", p[((((*i)+((aqpp-ii)*3))+3)-(bmp->offset*3))]);
printf("[%d]", p[((((*i)+((aqpp-ii)*3))+4)-(bmp->offset*3))]);
printf("[%d]", p[((((*i)+((aqpp-ii)*3))+5)-(bmp->offset*3))]);
printf("	%d [%d]\n",((((*i)+((aqpp-ii)*3))+((aqpp+ii)*3))-(bmp->offset*3))/3, p[((((*i)+((aqpp-ii)*3))+((aqpp+ii)*3))-(bmp->offset*3))]); }*/
						//printf("%d\n", ((*i)+((ii)*3))/3);
						//printf("ii=%d\n", ii);
						//printf("aqpp=%d\n", aqpp);
						//printf("			%d\n", ((*i)+((aqpp-ii)*3))/3);
					}
					//printf("	%d\n",((((*i)+((aqpp-ii)*3)))-(bmp->offset*3)));
					//printf("	%d\n",((((*i)+((aqpp-ii)*3))+1)-(bmp->offset*3)));
					//printf("	%d\n",((((*i)+((aqpp-ii)*3))+2)-(bmp->offset*3)));
					//printf("(*i)=%d\n", (*i));
				}
				//printf("[%d]", p[((*i)-q)]);	printf("[%d]", p[((*i)-q)+1]);	printf("[%d]", p[((*i)-q)+2]);	printf("[%d]", p[((*i)-q)+3]);			printf("[%d]", p[((*i)-q)+4]);			printf("[%d]", p[((*i)-q)+5]);			printf("	%d [%d]\n", (*i), p[((*i)-q)]);
				w++;
				break;
			}
			else
			{
				if(bmp->offset==0 && (*i)<3 && q==0) outp(PAL_DATA_REG, 0);
				else
				if(qp==0) outp(PAL_DATA_REG, p[(*i)-q]);
				else outp(PAL_DATA_REG, p[((*i)-(bmp->offset*3))]);
			}
		}
	}

	//palette checker~
	if(q>0 && qp==0)
	{
		long lq;
		long bufSize = (bmp->width * bmp->height);
		chkcolor(bmp, &q, &a, &aa, &z);

		/*printf("z=%d\n", z/3);
		printf("q+z=%d\n", (q+z)/3);
		printf("z-ppee=%d\n", (z-ppee)/3);
		printf("q=%d\n", q/3);
		printf("aa=%d\n", aa);*/

		aq=0; pp = q; ppee=q;
aqpee:
		while(aq<=aa)
		{
			//printf("a[%02d]=(%d)", aq, a[aq]);
			if(a[aq]==0) aq++;
			else{ aqpp++; break; }
		}

/*		printf("aq=%02d\n", aq);
		printf("z=%02d\n", z/3);
		printf("(z/3)-aqpp=%02d\n", (z/3)-aqpp);
		printf("aqpp=%02d\n", aqpp);*/

	for(lq=0; lq<bufSize; lq++)
	{
		if(bmp->data[lq]+bmp->offset==aq)
		{
			//printf("\n%02d\n", bmp->data[lq]);
			//printf("\n%02d\n", bmp->offset);
			//printf("\naq=	%02d\n", aq);
			//printf("a[aq]=	%02d\n", a[aq]);
			//bmp->data[lq]=((bmp->data[lq]+bmp->offset)-a[aq]);
			bmp->data[lq]=a[aq];
			//printf("_%d \n", bmp->data[lq]);
		}
		else if(bmp->data[lq]+bmp->offset < (z/3)-aqpp)
		{
			if(bmp->data[lq]+bmp->offset >= aq) bmp->data[lq]=(bmp->data[lq]+bmp->offset)-aqpp;
			else bmp->data[lq]+=(bmp->offset);
		}

		//printf("%02d ", bmp->data[lq]);
		//if(lq > 0 && lq%bmp->width==0) printf("\n");
	}

	while(pp<=(aq*3))
	{
		if(((pp/3)==aq || spee>0))
		{
			/*printf("spee=%d\n", spee);
			printf("		pp=%02d	", pp/3);
			printf("old	bmp: [%d]", bmp->palette[(pp-ppee)]);
			printf("[%d]", bmp->palette[(pp-ppee)+1]);
			printf("[%d]\n", bmp->palette[(pp-ppee)+2]);*/
			//if(spee==0) printf("\npp=%02d\n\n", pp/3);
			bmp->palette[(pp-ppee)]=		bmp->palette[(pp-ppee)+3];
			bmp->palette[(pp-ppee)+1]=	bmp->palette[(pp-ppee)+4];
			bmp->palette[(pp-ppee)+2]=	bmp->palette[(pp-ppee)+5];
			if(spee==0) spee++;
		}
		/*printf("		pp=%02d	", pp/3);
		printf("	bmp: [%d]", bmp->palette[(pp-ppee)]);
		printf("[%d]", bmp->palette[(pp-ppee)+1]);
		printf("[%d]\n", bmp->palette[(pp-ppee)+2]);*/
		pp+=3;
	}

	//update the palette~
	//printf("	aqpp=		%d\n", aqpp);
	VL_modexPalUpdate(bmp, &ppee, 1, aqpp);
	(*i)=ppee;
	//printf("	aqpp=	%d\n", aqpp);
	//printf("	ppee=	%d\n", ppee);

	/*printf(".\n");
	printf("aqpp=	%02d\n", aqpp/3);
	printf("aq=	%02d\n", aq);
	printf("aa=	%02d\n", aa);
	printf("		ppee=	%02d\n", ppee);*/

	if(aq<aa){ /*printf("~~~~\n"); */ppee=q; aq++; goto aqpee; }
	/*printf("ppee=%d\n", ppee);
	printf("pp=%d\n", pp);
	printf("q=%d\n", q);
	printf("(*i)=%d\n", (*i));*/

	}
}
