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

#include "src/lib/modex16.h"

void
main(int argc, char *argv[])
{
	byte *pal;
	char *bakapee;

	modexEnter();

	pal = modexNewPal();
	bakapee = malloc(64);

	if(argv[1]) bakapee = argv[1];
	else bakapee = "data/default.pal";
//	modexPalSave(pal);
	modexPalBlack();

	modexLoadPalFile(bakapee, &pal);
	modexFadeOn(1, pal);

	modexLeave();
}
