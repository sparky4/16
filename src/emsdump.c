/* Project 16 Source Code~
 * Copyright (C) 2012-2019 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

#include<stdio.h>
#include <conio.h>
//#include <limits.h>

#include "src/lib/exmm/memory.h"

int main(int argc, char *argv[])
{
	unsigned long i=0;
	char near w;
	char huge *p =(char huge *)(GetEMMSeg0()*0x10000);//0xD0000000;
	//char far *q =(char far *)0x53332225;
	printf("1p=%p	%ul\n", p, GetEMMSeg0());
	//while(!kbhit()){		printf("%p\n", ((GetEMMSeg0()*0x10000)+0x0fffffff));	}
	//while(!kbhit()){		printf("%p\n", ((GetEMMSeg0()*0x10000)+0xffff));	}
	if(argv[1])	{		printf("switch on!\n");		while(!kbhit()){}	}
	while(p<=(char huge *)((GetEMMSeg0()*0x10000)+0xffff)/*i<262144UL*/ && !kbhit())
	{
		if(argv[1]!=NULL) (*p) = i; else
		//else printf("%p=	%x%	%ul\n", p, *p, i);
		if((i%8)==0) printf("%p= %c %c %c %c %c %c %c %c\n", p,
*p, *(p+1), *(p+2), *(p+3), *(p+4), *(p+5), *(p+6), *(p+7));
//		(*p)++;
//		printf("				%p\n", ((GetEMMSeg0()*0x10000)+0x0fffffff));
		i++;
		p++;
	}
	w=104;
	//printf("*q=%d\n",*q);
	//printf(" q=%p\n", q);
	printf(" p=%p\n", p);
	printf(" w=%d\n", w);
	printf(" w=%p\n", w);
	printf("GetEMMSeg0()=%p\n", GetEMMSeg0()*0x10000);
	return 0;
}
