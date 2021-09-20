/* Project 16 Source Code~
 * Copyright (C) 2012-2021 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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
	IN_Default(0, &gvar.gvar.player[0],ctrl_Keyboard1);

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
	modexPalBlack();	//so gvar.player will not see loadings~
#endif
#endif

	/* setup camera and screen~ */
	modexHiganbanaPageSetup(&gvar.video);
	ZC_MVSetup(&gvar.mv, &map, &gvar);

	//renderswitch
	gvar.video.rss=1;

	/* set up paging */
	//TODO: LOAD map data and position the map in the middle of the screen if smaller then screen
	mapGoTo(&gvar.mv, 0, 0);

	ZC_gvar.playerXYpos(0, 0, &gvar.player, &gvar.mv, 0, 1);
	EN_initgvar.player(&gvar.player, 0, &gvar.video);
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
	modexClearRegion(gvar.mv[0].page, gvar.player[0].enti.x, gvar.player[0].enti.y-TILEWH, 16, 32, 15);
#else
	modexDrawSpriteRegion(&gvar.video.page[0], gvar.player[0].enti.x, gvar.player[0].enti.y-TILEWH, 16, 64, 16, 32,	PCXBMPPTR);
#endif

	//if(!pageflipflop)
		VL_ShowPage(gvar.mv[1].page, 0, 0);//modexShowPage(gvar.mv[1].page);
	//else			ZC_ShowMV(&gvar.mv, 0, 0);//modexShowPage(gvar.mv[0].page);//!(gvar.video.p)

	//modexDrawBmp(&gvar.video.page[0], 16, 16, PCXBMPPTR);
	/* buffer pages */
// 	modexClearRegion(gvar.mv[2].page, 0, 0, gvar.mv[2].page->width, gvar.mv[2].page->height, 47);
// 	modexClearRegion(gvar.mv[3].page, 0, 0, gvar.mv[3].page->width, gvar.mv[3].page->height, 45);
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
	modexClearRegion(gvar.mv[3].page, 0, 128, 24, 36, 15);

#ifdef MODEX
#ifdef FADE
	modexFadeOn(4, &gvar.video.palette);
#endif
#endif
	while(!IN_KeyDown(sc_Escape) && gvar.player[0].enti.hp>0)
	{
		shinku(&gvar);
	//top left corner & bottem right corner of map veiw be set as map edge trigger since maps are actually square
	//to stop scrolling and have the gvar.player position data move to the edge of the screen with respect to the direction
	//when gvar.player[0].enti.tx or gvar.player[0].enti.ty == 0 or gvar.player[0].enti.tx == 20 or gvar.player[0].enti.ty == 15 then stop because that is edge of map and you do not want to walk of the map

	//gvar.player movement
		IN_ReadControl(0, &gvar.player);
	if(!panswitch){
		ZC_walk(&gvar.mv, &gvar.player, 0);
	}else{
		PANKEYFUNZC;//panPageManual(&gvar.mv, &gvar.player, 0);
		//printf("	gvar.player[0].enti.q: %d", gvar.player[0].enti.q);	printf("	gvar.player[0].d: %d\n", gvar.player[0].d);
	}

	//the scripting stuff....
	//if(((gvar.player[0].enti.triggerx == TRIGGX && gvar.player[0].enti.triggery == TRIGGY) && IN_KeyDown(0x1C))||(gvar.player[0].enti.tx == 5 && gvar.player[0].enti.ty == 5))
	if(((gvar.mv[0].map->layerdata[0].data[(gvar.player[0].enti.triggerx-1)+(map.width*(gvar.player[0].enti.triggery-1))] == 0) && IN_KeyDown(0x1C))||(gvar.player[0].enti.tx == 5 && gvar.player[0].enti.ty == 5))
	{
		short i;
		for(i=800; i>=400; i--)
		{
			sound(i);
		}
		nosound();
	}
	if(gvar.player[0].enti.q == (TILEWH/(gvar.player[0].enti.speed))+1 && gvar.player[0].info.dir != 2 && (gvar.player[0].enti.triggerx == 5 && gvar.player[0].enti.triggery == 5)){ gvar.player[0].enti.hp--; }
	//debugging binds!
#ifdef MODEX
#ifdef FADE
	if(IN_KeyDown(24)){ modexPalUpdate0(&gvar.video.palette); paloffset=0; modexpdump(gvar.mv[0].page); modexpdump(gvar.mv[1].page);  IN_UserInput(1); } //p
	/*if(IN_KeyDown(22)){
	paloffset=0; modexPalBlack(); modexPalUpdate(PCXBMPVAR, &paloffset, 0, 0);
	printf("1paloffset	=	%d\n", paloffset/3);
	 modexPalUpdate(map.tiles->data, &paloffset, 0, 0);
	printf("2paloffset	=	%d\n", paloffset/3);
	 modexpdump(gvar.mv[0].page); modexpdump(gvar.mv[1].page);
		IN_UserInput(1);
	}*/
#endif
#endif
	//pan switch
	if(IN_KeyDown(62))	//f3
	{
		modexClearRegion(gvar.mv[1].page, 0, 0, gvar.mv[1].page->width, gvar.mv[1].page->height, 2);
		modexClearRegion(gvar.mv[2].page, 0, 0, gvar.mv[2].page->width, gvar.mv[2].page->height, 3);
		modexClearRegion(gvar.mv[3].page, 0, 0, gvar.mv[3].page->width, gvar.mv[3].page->height, 4);
		modexClearRegion(gvar.mv[3].page, 0, 0, 20, 36, 15);
		//IN_UserInput(1);
	}

	FUNCTIONKEYFUNCTIONS
	FUNCTIONKEYDRAWJUNK
	if(IN_KeyDown(sc_L)){ modexClearRegion(&gvar.video.page[0], gvar.player[0].enti.x, gvar.player[0].enti.y, 16, 16, 1); }

	//9
#ifdef FADE
		if(IN_KeyDown(10)){ modexPalOverscan(rand()%56); modexPalUpdate(gvar.video.dpal); IN_UserInput(1); }
#endif
		if(IN_KeyDown(sc_R)){ modexPalOverscan(rand()%56); } //r

	if((gvar.player[0].enti.q==1) && !(gvar.player[0].enti.x%TILEWH==0 && gvar.player[0].enti.y%TILEWH==0)) break;	//incase things go out of sync!
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
