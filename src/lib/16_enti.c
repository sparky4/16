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

#include "src/lib/16_enti.h"

void EN_initentity(entity_t *enti)
{
	//tx and ty inited by map
	enti->x = enti->tx*TILEWH;
	enti->y = enti->ty*TILEWH;
	enti->triggerx = enti->tx;
	enti->triggery = enti->ty+1;
	enti->q = 1;
	enti->d =2;
	enti->hp=4;
	enti->speed=4;
	enti->persist_aniframe = enti->q;
	enti->spt=(TILEWH/(enti->speed));	//speed per tile wwww
	enti->invq=1;
	enti->pred = enti->d;
	enti->overdraww=0;
	enti->overdrawh=4;
}

//init player!
void EN_initplayer(player_t *player, word pn)
{
	EN_initentity(&player[pn].enti);
/*	player[0].info.x = player[0].tx;
	player[0].info.xaxis = player[0].tx*TILEWH;
	player[0].info.y = player[0].ty;
	player[0].info.yaxis = player[0].ty*TILEWH;*/
}
