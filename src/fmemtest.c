/* Project 16 Source Code~
 * Copyright (C) 2012-2022 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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
#include <stdio.h>
#include "src/lib/16_head.h"
#include "src/lib/16_ca.h"
#include "src/lib/16_mm.h"
#include "src/lib/16_t.h"

void
main(int argc, char *argv[])
{
	int x=420;
	int huge *ptr=&x;
	int *ptr0=&x;
	//void __based(__self) *pee;
	memptr pee;
	void __near *npee;
	void __far *fpee;
	void *dpee;
	__segment segu;
	//word csw=0,dsw=0,esw=0,ssw=0,ipw=0;

	_nheapgrow();
	_fheapgrow();
	_heapgrow();
	//ptr=&x;
	printf("&main()=%Fp\n", *argv[0]);
	printf("ptr0=%Fp\n", ptr0);
	//printf("&ptr0=%Fp\n", &ptr0);
	printf("*ptr0=%d\n", *ptr0);
	printf("ptr=%Fp\n", ptr);
	//printf("&ptr=%Fp\n", &ptr);
	printf("*ptr=%d\n", *ptr);
	printf("address of *ptr=%Fp\n", *ptr);
	printf("&x=%Fp\n", &x);
	printf("x=%d\n", x);
	printf("\n");
	/*__asm
	{
		//lea csw,cs
		//lds DS,dsw
		//les ES,esw
//		mov ss,ssw
//		mov ip,ipw
	}*/
	//printf("ip=%u\n", ipw);
	//printf("ss=%u\n", ssw);
	//printf("cs=%u\n", csw);
	//printf("ds=%u\n", dsw);
	//printf("es=%u\n", esw);
	printf("memavl=%u\n", _memavl());
	printf("size of based pee~=%u	%FP\n", _bmsize(segu, pee), pee);
	printf("size of default pee~=%u	%FP\n", _msize(dpee), dpee);
	printf("size of near pee~=%u	%FP\n", _nmsize(npee), npee);
	printf("size of far pee~=%u	%FP\n", _fmsize(fpee), fpee);
	printf("pee=%Fp\n", pee);
	printf("npee=%Fp\n", npee);
	printf("&main()=%Fp\n", *argv[0]);
}
