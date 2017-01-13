/* Project 16 Source Code~
 * Copyright (C) 2012-2016 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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
 * palllist experiment
 */
/*
 * An experiment where I make 2 small linked list which points to 2 arrays
 * one is the "hardware palette" array
 * the other is "image palette" array
 * and their respectable pointers point to their [i] stuff
 *
 * the palette updater stuff i want to make is this:
 *
 * VGA		IMG
 * 1-			2-
 * 2-			4-
 * 3-			0-
 * 4-			9-
 * 5-			0-(I will figure this out later if  there is dup numbs)
 * 6-
 * 7-
 * 8-
 *
 *		MERGE THEM INTO
 *
 * VGA
 * 1-
 * 2->IMG[0]
 * 3-
 * 4->IMG[1]
 * 5
 * 6
 * 7
 * 8
 * 9->IMG[3]
 * 0->IMG[2]->IMG[4]
 *
 * i am going to work on a really big area of code it will be on the refresh system, the walking/scrolling system, things that use showpage, adding disableing 8087 functions if no 8087 detected, and a bunch of other things i cannot remember off the top of my head because i am BURNT. I need a small break~ -- -- -- -- update! i am working on this i made ZC_WALK and i am going to move to VRL/VRS soon! .
 * ==== PRE SHOWPAGE TO SHOWMV CONVERSION ==== i am going to work on a really big area of code it will be on the refresh system, the walking/scrolling system, things that use showpage, adding disableing 8087 functions if no 8087 detected, and a bunch of other things i cannot remember off the top of my head because i am BURNT. I need a small break~ -- -- -- -- update! i am working on this i made ZC_WALK and i am going to move to VRL/VRS soon!
 */
#include <stdio.h>

void
main(int argc, char *argv[])
{
//TOO TIREDWWWW
}
