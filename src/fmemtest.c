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

#include <stdio.h>

void
main(int argc, char *argv[])
{
	int x=420;
	int huge *ptr=&x;
	int *ptr0=&x;

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
}
