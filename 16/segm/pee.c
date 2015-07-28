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
	Segment experiment!
*/
#include <stdio.h>
#ifdef __WATCOMC__
typedef void __based(__self) * memptr;
#endif
#ifdef __BORLANDC__
typedef void _seg * memptr;
#endif

void
main(/*int argc, char *argv[]*/)
{
	unsigned bee=0;
	memptr pee;
	printf("local variable bee=		%Fp\n", &bee);
	printf("far pointer value of pee=	%Fp\n", pee);
	printf("pointer value of pee=		%p\n", pee);
}
