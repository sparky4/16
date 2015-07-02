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
	input test
*/
#include "src/lib/16_ca.h"
#include "src/lib/16_mm.h"

void
main(int argc, char *argv[])
{
	mminfo_t mm; mminfotype mmi;
	memptr	bigbuffer;
	__segment segu;
	mm.mmstarted=0;
	printf("start!\n");
	MM_Startup(&mm, &mmi);
	printf("done!\n");
	printf("&main()=%Fp\n", *argv[0]);
	if(CA_LoadFile("data/chikyuu.pcx", &bigbuffer, &mm, &mmi)) printf("\nyay!\n");
	//MM_GetPtr(&bigbuffer, mmi.nearheap, &mm, &mmi);
	//hmm functions in cache system use the buffered stuff
	printf("size of big buffer~=%ul\n", _bmsize(segu, bigbuffer));
	MM_ShowMemory(&mm);
	MM_Report(&mm, &mmi);
	printf("stop!\n");
	MM_FreePtr(&bigbuffer, &mm);
	MM_Shutdown(&mm);
	printf("done!\n");
}
