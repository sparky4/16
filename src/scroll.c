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

#include "src/lib/scroll16.h"
#include "src/lib/16_timer.h"
#include "src/lib/16_dbg.h"

#define FADE
#define MODEX	//this is for mode x initiating

static map_t map;
map_view_t mv[4];
float t;
sword bakapee;
//debugswitches
boolean panswitch=0;//1
//extern boolean pageflipflop=1;
extern boolean pagenorendermap;
unsigned int i;
//#ifdef FADE
	static word paloffset=0;
	byte *gvar.video.dpal;
//#endif
//	byte *ptr;
	byte *mappalptr;

void main(int argc, char *argv[])
{
	static global_game_variables_t gvar;
	static player_t player[MaxPlayers];
	if(argv[1]) bakapee = atoi(argv[1]);
	else bakapee = 1;

	Startup16(&gvar);

	/* create the map */
//	fprintf(stderr, "testing map load~	");
	CA_loadmap("data/test.map", &map, &gvar);
	chkmap(&map, 0);
//	printf("chkmap ok	");
//	fprintf(stderr, "yay map loaded~~\n");

	/* draw the tiles */
#ifdef MODEX
//	ptr = map.data;
//	mappalptr = map.tiles->btdata->palette;

	/* data */
	PCXBMP = bitmapLoadPcx("data/chikyuu.pcx", &gvar); // load sprite

#endif
	/*	input!	*/
	IN_Default(0, &player,ctrl_Keyboard1);

	/* save the palette */
#ifdef MODEX
#ifdef FADE
	gvar.video.dpal = modexNewPal();
	modexPalSave(gvar.video.dpal);
	modexFadeOff(4, gvar.video.dpal);
#endif

	VGAmodeX(bakapee, 1, &gvar);

	/* fix up the palette and everything */
#ifdef FADE
	modexPalBlack();	//reset the palette~
#endif
#endif
//++++	PCXBMPVAR.offset=(paloffset/3);
//++++	modexPalUpdate(&PCXBMPVAR, &paloffset, 0, 0);
	modexPalUpdate(&PCXBMP->palette);
//++++0000		modexPalUpdate(map.tiles->btdata->palette);
	//printf("	%d\n", sizeof(ptmp->data));
	//printf("1:	%d\n", paloffset);
//++++	map.tiles->data->offset=(paloffset/3);
	//XTmodexPalUpdate(map.tiles->data, &paloffset, 0, 0);
//	printf("\n====\n");
//	printf("0	paloffset=	%d\n", paloffset/3);
//	printf("====\n\n");
#ifdef MODEX
#ifdef FADE
	&gvar.video.palette = modexNewPal();
	modexPalSave(&gvar.video.palette);
	modexSavePalFile("data/g.pal", &gvar.video.palette);
	modexPalBlack();	//so player will not see loadings~
#endif
#endif

	/* setup camera and screen~ */
	modexHiganbanaPageSetup(&gvar.video);
	ZC_MVSetup(&mv, &map, &gvar);

	//renderswitch
	gvar.video.rss=1;

	/* set up paging */
	//TODO: LOAD map data and position the map in the middle of the screen if smaller then screen
	mapGoTo(&mv, 0, 0);

	ZC_playerXYpos(0, 0, &player, &mv, 0, 1);
	EN_initplayer(&player, 0, &gvar.video);
	if (gvar.video.sprifilei == -1)
	{
#ifdef FADE
		modexFadeOff(4, &gvar.video.palette);
#endif
		Quit(&gvar, "Wrong");
#ifdef FADE
		modexFadeOn(4, gvar.video.dpal);
#endif
	}

#ifndef	SPRITE
	modexClearRegion(mv[0].page, player[0].enti.x, player[0].enti.y-TILEWH, 16, 32, 15);
#else
	modexDrawSpriteRegion(&gvar.video.page[0], player[0].enti.x, player[0].enti.y-TILEWH, 16, 64, 16, 32,	PCXBMPPTR);
#endif

	//if(!pageflipflop)
		VL_ShowPage(mv[1].page, 0, 0);//modexShowPage(mv[1].page);
	//else			ZC_ShowMV(&mv, 0, 0);//modexShowPage(mv[0].page);//!(gvar.video.p)

	//modexDrawBmp(&gvar.video.page[0], 16, 16, PCXBMPPTR);
	/* buffer pages */
// 	modexClearRegion(mv[2].page, 0, 0, mv[2].page->width, mv[2].page->height, 47);
// 	modexClearRegion(mv[3].page, 0, 0, mv[3].page->width, mv[3].page->height, 45);
// 	{
// 		unsigned int k,j,o;
// 		/* fill screen with a distinctive pattern */
// 		for (k=0;k < vga_state.vga_width;k++) {
// 			o = k >> 2;
// 			vga_write_sequencer(0x02/*map mask*/,1 << (k&3));
// 				for (j=0;j < vga_state.vga_height;j++,o += vga_state.vga_stride)
// 					vga_state.vga_graphics_ram[o] = (k^j)&15; // VRL samples put all colors in first 15!
// 		}
// 	}
	modexClearRegion(mv[3].page, 0, 128, 24, 36, 15);

#ifdef MODEX
#ifdef FADE
	modexFadeOn(4, &gvar.video.palette);
#endif
#endif
	while(!IN_KeyDown(sc_Escape) && player[0].enti.hp>0)
	{
		shinku(&gvar);
	//top left corner & bottem right corner of map veiw be set as map edge trigger since maps are actually square
	//to stop scrolling and have the player position data move to the edge of the screen with respect to the direction
	//when player[0].enti.tx or player[0].enti.ty == 0 or player[0].enti.tx == 20 or player[0].enti.ty == 15 then stop because that is edge of map and you do not want to walk of the map

	//player movement
		IN_ReadControl(0, &player);
	if(!panswitch){
		ZC_walk(&mv, &player, 0);
	}else{
		PANKEYFUNZC;//panPageManual(&mv, &player, 0);
		//printf("	player[0].enti.q: %d", player[0].enti.q);	printf("	player[0].d: %d\n", player[0].d);
	}

	//the scripting stuff....
	//if(((player[0].enti.triggerx == TRIGGX && player[0].enti.triggery == TRIGGY) && IN_KeyDown(0x1C))||(player[0].enti.tx == 5 && player[0].enti.ty == 5))
	if(((mv[0].map->layerdata[0].data[(player[0].enti.triggerx-1)+(map.width*(player[0].enti.triggery-1))] == 0) && IN_KeyDown(0x1C))||(player[0].enti.tx == 5 && player[0].enti.ty == 5))
	{
		short i;
		for(i=800; i>=400; i--)
		{
			sound(i);
		}
		nosound();
	}
	if(player[0].enti.q == (TILEWH/(player[0].enti.speed))+1 && player[0].info.dir != 2 && (player[0].enti.triggerx == 5 && player[0].enti.triggery == 5)){ player[0].enti.hp--; }
	//debugging binds!
#ifdef MODEX
#ifdef FADE
	if(IN_KeyDown(24)){ modexPalUpdate0(&gvar.video.palette); paloffset=0; modexpdump(mv[0].page); modexpdump(mv[1].page);  IN_UserInput(1,1); } //p
	/*if(IN_KeyDown(22)){
	paloffset=0; modexPalBlack(); modexPalUpdate(PCXBMPVAR, &paloffset, 0, 0);
	printf("1paloffset	=	%d\n", paloffset/3);
	 modexPalUpdate(map.tiles->data, &paloffset, 0, 0);
	printf("2paloffset	=	%d\n", paloffset/3);
	 modexpdump(mv[0].page); modexpdump(mv[1].page);
		IN_UserInput(1,1);
	}*/
#endif
#endif
	//pan switch
	if(IN_KeyDown(62))	//f3
	{
		modexClearRegion(mv[1].page, 0, 0, mv[1].page->width, mv[1].page->height, 2);
		modexClearRegion(mv[2].page, 0, 0, mv[2].page->width, mv[2].page->height, 3);
		modexClearRegion(mv[3].page, 0, 0, mv[3].page->width, mv[3].page->height, 4);
		modexClearRegion(mv[3].page, 0, 0, 20, 36, 15);
		//IN_UserInput(1,1);
	}

	FUNCTIONKEYFUNCTIONS
	FUNCTIONKEYDRAWJUNK
	if(IN_KeyDown(sc_L)){ modexClearRegion(&gvar.video.page[0], player[0].enti.x, player[0].enti.y, 16, 16, 1); }

	//9
#ifdef FADE
		if(IN_KeyDown(10)){ modexPalOverscan(rand()%56); modexPalUpdate(gvar.video.dpal); IN_UserInput(1,1); }
#endif
		if(IN_KeyDown(sc_R)){ modexPalOverscan(rand()%56); } //r

	if((player[0].enti.q==1) && !(player[0].enti.x%TILEWH==0 && player[0].enti.y%TILEWH==0)) break;	//incase things go out of sync!
	}

	/* fade back to text mode */
	/* but 1st lets save the game palette~ */
#ifdef MODEX
#ifdef FADE
	modexPalSave(&gvar.video.palette);
	modexSavePalFile("data/g.pal", &gvar.video.palette);
	modexFadeOff(4, &gvar.video.palette);
#endif
	VGAmodeX(0, 1, &gvar);
#endif
	Shutdown16(&gvar);
	printf("\nProject 16 scroll.exe. This is just a test file!\n");
	printf("version %s\n", VERSION);
	SCROLLEXITMESG;
	WCPU_cpufpumesg();
#ifdef MODEX
#ifdef FADE
	modexFadeOn(4, gvar.video.dpal);
#endif
#endif
}
