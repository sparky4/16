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
unsigned int i;
//#ifdef FADE
	static word paloffset=0;
	byte *dpal;
//#endif
	byte *gpal;
//	byte *ptr;
	byte *mappalptr;

void main(int argc, char *argv[])
{
	static global_game_variables_t gvar;
	static player_t player[MaxPlayers];
	if(argv[1]) bakapee = atoi(argv[1]);
	else bakapee = 1;

	Startup16(&gvar);

	gvar.video.panp=0;

	/* create the map */
//	fprintf(stderr, "testing map load~	");
	loadmap("data/test.map", &map);
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
	dpal = modexNewPal();
	modexPalSave(dpal);
	modexFadeOff(4, dpal);
#endif

	VGAmodeX(bakapee, 1, &gvar);

	/* fix up the palette and everything */
#ifdef FADE
	modexPalBlack();	//reset the palette~
#endif
#endif
//++++	PCXBMPVAR.offset=(paloffset/3);
//++++	modexPalUpdate1(&PCXBMPVAR, &paloffset, 0, 0);
	modexPalUpdate1(&PCXBMP->palette);
//++++0000		modexPalUpdate1(map.tiles->btdata->palette);
	//printf("	%d\n", sizeof(ptmp->data));
	//printf("1:	%d\n", paloffset);
//++++	map.tiles->data->offset=(paloffset/3);
	//XTmodexPalUpdate(map.tiles->data, &paloffset, 0, 0);
//	printf("\n====\n");
//	printf("0	paloffset=	%d\n", paloffset/3);
//	printf("====\n\n");
#ifdef MODEX
#ifdef FADE
	gpal = modexNewPal();
	modexPalSave(gpal);
	modexSavePalFile("data/g.pal", gpal);
	modexPalBlack();	//so player will not see loadings~
#endif
#endif

	/* setup camera and screen~ */
	modexHiganbanaPageSetup(&gvar.video);
	ZC_MVSetup(&mv, &map, &gvar);


	/* set up paging */
	//TODO: LOAD map data and position the map in the middle of the screen if smaller then screen
	mapGoTo(&mv, 0, 0);

	playerXYpos(0, 0, &player, &mv, 0);
	IN_initplayer(&player, 0);

#ifndef	SPRITE
	modexClearRegion(mv[0].page, player[0].x, player[0].y-TILEWH, 16, 32, 15);
#else
	PBUFSFUN(&gvar.video.page[0], player[0].x, player[0].y-TILEWH, 16, 64, 16, 32,	PCXBMPPTR);
#endif

	if(!pageflipflop)	VL_ShowPage(mv[1].page, 0, 0);//modexShowPage(mv[1].page);
	else			ZC_ShowMV(&mv, 0, 0);//modexShowPage(mv[0].page);//!(gvar.video.p)


	modexDrawBmp(&gvar.video.page[0], 16, 16, PCXBMPPTR);
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
	modexFadeOn(4, gpal);
#endif
#endif
	while(!IN_KeyDown(sc_Escape) && player[0].hp>0)
	{
		shinku(&gvar);
	//top left corner & bottem right corner of map veiw be set as map edge trigger since maps are actually square
	//to stop scrolling and have the player position data move to the edge of the screen with respect to the direction
	//when player[0].tx or player[0].ty == 0 or player[0].tx == 20 or player[0].ty == 15 then stop because that is edge of map and you do not want to walk of the map

	//player movement
		IN_ReadControl(0, &player);
	if(!panswitch){
		ZC_walk(&mv, &player, 0);
	}else{
		PANKEYFUN;//panPageManual(&mv, &player, 0);
		//printf("	player[0].q: %d", player[0].q);	printf("	player[0].d: %d\n", player[0].d);
	}

	//the scripting stuff....
	//if(((player[0].triggerx == TRIGGX && player[0].triggery == TRIGGY) && IN_KeyDown(0x1C))||(player[0].tx == 5 && player[0].ty == 5))
	if(((mv[0].map->data[(player[0].triggerx-1)+(map.width*(player[0].triggery-1))] == 0) && IN_KeyDown(0x1C))||(player[0].tx == 5 && player[0].ty == 5))
	{
		short i;
		for(i=800; i>=400; i--)
		{
			sound(i);
		}
		nosound();
	}
	if(player[0].q == (TILEWH/(player[0].speed))+1 && player[0].info.dir != 2 && (player[0].triggerx == 5 && player[0].triggery == 5)){ player[0].hp--; }
	//debugging binds!
#ifdef MODEX
#ifdef FADE
	if(IN_KeyDown(24)){ modexPalUpdate0(gpal); paloffset=0; modexpdump(mv[0].page); modexpdump(mv[1].page);  IN_UserInput(1,1); } //p
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
	if(IN_KeyDown(88)){panswitch=!panswitch; IN_UserInput(1,1);}	//f12
	if(IN_KeyDown(87))	//f11
	{
		pageflipflop=!pageflipflop;
		IN_UserInput(1,1);
// 		VGAmodeX(0, 0, &gvar);
// 		IN_Shutdown();
// 		__asm
// 		{
// 			mov ah,31h
// 			int 21h
// 		}
	}
	if(IN_KeyDown(68))	//f10
	{
		gvar.kurokku.fpscap=!gvar.kurokku.fpscap;
		IN_UserInput(1,1);
	}
	if(IN_KeyDown(67))	//f9
	{
		modexClearRegion(mv[1].page, 0, 0, mv[1].page->width, mv[1].page->height, 2);
		modexClearRegion(mv[2].page, 0, 0, mv[2].page->width, mv[2].page->height, 3);
		modexClearRegion(mv[3].page, 0, 0, mv[3].page->width, mv[3].page->height, 4);
		modexClearRegion(mv[3].page, 0, 0, 20, 36, 15);
		//IN_UserInput(1,1);
	}
	if(IN_KeyDown(66))	//f8
	{
//		modexDrawSprite(mv[0].page, 16, 16, &p);
		modexDrawSprite(mv[0].page, 32+72, 16, (PCXBMPVAR));
	}
	FUNCTIONKEYFUNCTIONS;

	//9
#ifdef FADE
		if(IN_KeyDown(10)){ modexPalOverscan(rand()%56); modexPalUpdate1(dpal); IN_UserInput(1,1); }
#endif
		if(IN_KeyDown(sc_R)){ modexPalOverscan(rand()%56); } //r

	if((player[0].q==1) && !(player[0].x%TILEWH==0 && player[0].y%TILEWH==0)) break;	//incase things go out of sync!
	}

	/* fade back to text mode */
	/* but 1st lets save the game palette~ */
#ifdef MODEX
#ifdef FADE
	modexPalSave(gpal);
	modexSavePalFile("data/g.pal", gpal);
	modexFadeOff(4, gpal);
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
	modexFadeOn(4, dpal);
#endif
#endif
}
