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
/* tab size = 8 */

#include "src/lib/wcpu/16_wcpu.h"

void main(int argc, char **argv)
{
	/*const char *cpus, *fpus;
	unsigned char cput, fput;

	cput = WCPU_detectcpu();
	fput = WCPU_detectfpu();
	switch(cput)
	{
		case 0: cpus = "8086/8088 or 186/88"; break;
		case 1: cpus = "286"; break;
		case 2: cpus = "386 or newer"; break;
		default: cpus = "internal error"; break;
	}

	switch(fput)
	{
		case 0: fpus = "none"; break;
		case 1: fpus = "8087"; break;
		default: fpus = "internal error"; break;
	}
	printf("detected CPU type: %s\n", cpus);
	printf("detected FPU type: %s\n",	fpus);*/
	WCPU_cpufpumesg();
	//if (cpu_flags & CPU_FLAG_FPU) printf(" - With FPU\n");
}
