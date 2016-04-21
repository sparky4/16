/* Project 16 Source Code~
 * Copyright (C) 2012-2016 sparky4 & pngwen & andrius4669 & joncampbell123
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
#include "src/lib/wcpu/wcpu.h"

//#define FADE
#define MODEX	//this is for mode x initiating

//word far *clock= (word far*) 0x046C; /* 18.2hz clock */

global_game_variables_t gvar;
static map_t map;
player_t player[MaxPlayers];
//page_t screen, gvar.video.page[1], gvar.video.page[2];
map_view_t mv[3];
bitmap_t p;
word pn=0;
static planar_buf_t huge *pp;
float t;
sword bakapee;

//	word panswitch=0, panq=1, pand=0;
	word panpagenum=0; //for panning!
	unsigned int i;
	const char *cpus;
	//static int persist_aniframe = 0;    /* gonna be increased to 1 before being used, so 0 is ok for default */

	//map_view_db_t pgid[4];
	word pg;
//#ifdef FADE
	static word paloffset=0;
	byte *dpal, *default_pal;
//#endif
	byte *gpal;
	byte *ptr;
	byte *mappalptr;

void main(int argc, char *argv[])
{
	byte *mesg=malloc(sizeof(dword));

	if(argv[1]) bakapee = atoi(argv[1]);
	else bakapee = 1;

	// DOSLIB: check our environment
	probe_dos();

	// DOSLIB: what CPU are we using?
	// NTS: I can see from the makefile Sparky4 intends this to run on 8088 by the -0 switch in CFLAGS.
	//      So this code by itself shouldn't care too much what CPU it's running on. Except that other
	//      parts of this project (DOSLIB itself) rely on CPU detection to know what is appropriate for
	//      the CPU to carry out tasks. --J.C.
	cpu_probe();

	// DOSLIB: check for VGA
	if (!probe_vga()) {
		printf("VGA probe failed\n");
		return;
	}
	// hardware must be VGA or higher!
	if (!(vga_state.vga_flags & VGA_IS_VGA)) {
		printf("This program requires VGA or higher graphics hardware\n");
		return;
	}

	player[0].persist_aniframe=0;
	player[0].speed=4;
	//player[0].data = &pp;

	printf("starting timer	");
	start_timer(&gvar);
	printf("ok\n");

//	atexit(qclean());

	/* create the map */
	fprintf(stderr, "testing map load~	");
	loadmap("data/test.map", &map);
	chkmap(&map, 0);
	printf("chkmap ok	");
	fprintf(stderr, "yay map loaded~~\n");
	mv[0].map = &map;
	mv[1].map = &map;
	mv[2].map = &map;

	/* draw the tiles */
#ifdef MODEX
	ptr = map.data;
	//mappalptr = map.tiles->btdata->palette;

	/* data */
	//++++p = bitmapLoadPcx("data/ptmp.pcx"); // load sprite
	//npctmp = bitmapLoadPcx("ptmp1.pcx"); // load sprite

	/* create the planar buffer */
////++++	(player[0].data) = *planar_buf_from_bitmap(&p);
	/*++++printf("load pee!!	");
	pp = planar_buf_from_bitmap(&p);
	printf("done!\n");*/

#endif
	/*	input!	*/
	IN_Startup();
	IN_Default(0,&player,ctrl_Joystick);

	/* save the palette */
#ifdef MODEX
#ifdef FADE
	dpal = modexNewPal();
	default_pal = modexNewPal();
	*default_pal = *dpal;
	modexPalSave(dpal);
	modexFadeOff(4, dpal);
#endif

	textInit();
	VGAmodeX(bakapee, 1, &gvar);
//	printf("%dx%d\n", gvar.video.page[0].sw, gvar.video.page[0].sh);
#ifdef FADE
	modexPalBlack();	//reset the palette~
#endif
#endif
//	printf("Total used @ before palette initiation:		%zu\n", oldfreemem-GetFreeSize());
//++++	player[0].data.offset=(paloffset/3);
//++++	modexPalUpdate1(&player[0].data, &paloffset, 0, 0);
		//modexPalUpdate1(p.palette);
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
	gvar.video.page[0] = modexDefaultPage(&gvar.video.page[0]);
	gvar.video.page[0].width += (TILEWH*2);
	gvar.video.page[0].height += (TILEWH*2);
	mv[0].page = &gvar.video.page[0];
	gvar.video.page[1] = modexNextPage(mv[0].page);
	mv[1].page = &gvar.video.page[1];

	gvar.video.page[2] = modexNextPageFlexibleSize(mv[1].page, gvar.video.page[0].sw, gvar.video.page[0].sh);	//(352*176)+1024 is the remaining amount of memory left wwww
	//gvar.video.page[2] = modexNextPage0(mv2.page, 320, 192);	//(352*176)+1024 is the remaining amount of memory left wwww
	mv[2].page = &gvar.video.page[2];

	/* set up paging */
//TODO: LOAD map data and position the map in the middle of the screen if smaller then screen
	mapGoTo(&mv[0], 0, 0);
	mapGoTo(&mv[1], 0, 0);
	//mapGoTo(mask, 0, 0);
	//TODO: put player in starting position of spot
	//default player position on the viewable map
	player[0].tx = mv[0].tx + mv[0].page->tilemidposscreenx;
	player[0].ty = mv[0].ty + mv[0].page->tilemidposscreeny;
	IN_initplayer(&player, 0);

	modexCopyPageRegion(mv[1].page, mv[0].page, 0, 0, 0, 0, mv[0].page->width, mv[0].page->height);
#ifndef	SPRITE
	modexClearRegion(mv[1].page, player[0].x-4, player[0].y-TILEWH, 24, 32, 15);
#else
	//PBUFSFUN(mv[1].page, player[0].x-4, player[0].y-TILEWH, 24, 64, 24, 32,	PLAYERBMPDATA);
	PBUFSFUN(mv[1].page, player[0].x-4, player[0].y-TILEWH, 24, 64, 24, 32,	&pp);
#endif

	modexShowPage(mv[1].page);
	shinku_fps_indicator_page = 1; // we're on page 1 now, shinku(). follow along please or it will not be visible.
	//modexClearRegion(mv[2].page, 0, 0, mv[2].page->width, mv[2].page->height, 1);
#ifdef MODEX
#ifdef FADE
	modexFadeOn(4, gpal);
#endif
#endif
	while(!IN_KeyDown(sc_Escape) && player[0].hp>0)
	{
		shinku(&gvar);
		IN_ReadControl(0,&player);
	//top left corner & bottem right corner of map veiw be set as map edge trigger since maps are actually square
	//to stop scrolling and have the player position data move to the edge of the screen with respect to the direction
	//when player[0].tx or player[0].ty == 0 or player[0].tx == 20 or player[0].ty == 15 then stop because that is edge of map and you do not want to walk of the map

	//player movement
	//if(!panswitch){
		walk(mv, player, 0);
	/*}else{
//88 switch!
	//right movement
	if((IN_KeyDown(77) && !IN_KeyDown(75) && pand == 0) || pand == 2)
	{
		if(pand == 0){ pand = 2; }
			if(panq<=(TILEWH/(player[0].speed)))
			{
				switch(panpagenum)
				{
					case 0:
						//bg
						mv[0].page->dx++;
						modexShowPage(mv[0].page);
					break;
					case 1:
						//spri
						mv[1].page->dx++;
						modexShowPage(mv[1].page);
					break;
					case 2:
						//fg
						mv[2].page->dx++;
						modexShowPage(mv[2].page);
					break;
				}
				panq++;
			} else { panq = 1; pand = 0; }
	}
	//left movement
	if((IN_KeyDown(75) && !IN_KeyDown(77) && pand == 0) || pand == 4)
	{
		if(pand == 0){ pand = 4; }
			if(panq<=(TILEWH/(player[0].speed)))
			{
				switch(panpagenum)
				{
					case 0:
						//bg
						mv[0].page->dx--;
						modexShowPage(mv[0].page);
					break;
					case 1:
						//spri
						mv[1].page->dx--;
						modexShowPage(mv[1].page);
					break;
					case 2:
						//fg
						mv[2].page->dx--;
						modexShowPage(mv[2].page);
					break;
				}
				panq++;
			} else { panq = 1; pand = 0; }
	}
	//down movement
	if((IN_KeyDown(72) && !IN_KeyDown(80) && pand == 0) || pand == 3)
	{
		if(pand == 0){ pand = 3; }
			if(panq<=(TILEWH/(player[0].speed)))
			{
				switch(panpagenum)
				{
					case 0:
						//bg
						mv[0].page->dy--;
						modexShowPage(mv[0].page);
					break;
					case 1:
						//spri
						mv[1].page->dy--;
						modexShowPage(mv[1].page);
					break;
					case 2:
						//fg
						mv[2].page->dy--;
						modexShowPage(mv[2].page);
					break;
				}
				panq++;
			} else { panq = 1; pand = 0; }
	}
	//up movement
	if((IN_KeyDown(80) && !IN_KeyDown(72) && pand == 0) || pand == 1)
	{
		if(pand == 0){ pand = 1; }
			if(panq<=(TILEWH/(player[0].speed)))
			{
				switch(panpagenum)
				{
					case 0:
						//bg
						mv[0].page->dy++;
						modexShowPage(mv[0].page);
					break;
					case 1:
						//spri
						mv[1].page->dy++;
						modexShowPage(mv[1].page);
					break;
					case 2:
						//fg
						mv[2].page->dy++;
						modexShowPage(mv[2].page);
					break;
				}
				panq++;
			} else { panq = 1; pand = 0; }
	}
}*/


	//the scripting stuf....

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
	//if(IN_KeyDown(0x0E)) while(1){ if(xmsmalloc(24)) break; }
	if(IN_KeyDown(2)){ modexShowPage(mv[0].page); panpagenum=0; }
	if(IN_KeyDown(3)){ modexShowPage(mv[1].page); panpagenum=1; }
	if(IN_KeyDown(4)){ modexShowPage(mv[2].page); panpagenum=2; }
	if(IN_KeyDown(25)){ modexpdump(mv[0].page); modexpdump(mv[1].page); }	//p
#ifdef MODEX
#ifdef FADE
	if(IN_KeyDown(24)){ modexPalUpdate0(gpal); paloffset=0; modexpdump(mv[0].page); modexpdump(mv[1].page); }
	if(IN_KeyDown(22)){
	paloffset=0; modexPalBlack(); modexPalUpdate(&player[0].data, &paloffset, 0, 0);
	printf("1paloffset	=	%d\n", paloffset/3);
	 modexPalUpdate(map.tiles->data, &paloffset, 0, 0);
	printf("2paloffset	=	%d\n", paloffset/3);
	 modexpdump(mv[0].page); modexpdump(mv[1].page); }
#endif
#endif
	//pan switch
	//if(IN_KeyDown(88)){if(!panswitch) panswitch++; else panswitch--; }	//f12
	//TSR
	if(IN_KeyDown(87))	//f11
	{
		VGAmodeX(0, 0, &gvar);
		IN_Shutdown();
		__asm
		{
			mov ah,31h
			int 21h
		}
	}
	if(IN_KeyDown(88))	//s
	{
		switch(gvar.kurokku.fpscap)
		{
			case 0:
				gvar.kurokku.fpscap=1;
			break;
			case 1:
				gvar.kurokku.fpscap=0;
			break;
		}
		IN_Ack();
	}
	//TODO fmemtest into page
	if(IN_KeyDown(4+1))	//4
	{
		pg=1;
		SELECT_ALL_PLANES();
		_fmemset(((mv[pg].page->data+4)+(16*(mv[pg].page->width/4))), 15, 4);
	}

	//9
	if(IN_KeyDown(10)){ modexPalOverscan(default_pal, rand()%56); modexPalUpdate1(default_pal); }
	//if(IN_KeyDown(11)){ modexPalOverscan(default_pal, 15); }
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
	IN_Shutdown();
	printf("Project 16 scroll.exe. This is just a test file!\n");
	printf("version %s\n", VERSION);
	printf("tx: %d\n", mv[0].tx);
	printf("ty: %d\n", mv[0].ty);
	printf("player[0].x: %d", player[0].x); printf("		player[0].y: %d\n", player[0].y);
	//if(player[0].hp==0) printf("%d wwww\n", player[0].y+8);
	//else printf("\nplayer[0].y: %d\n", player[0].y);
	printf("player[0].tx: %d", player[0].tx); printf("		player[0].ty: %d\n", player[0].ty);
	printf("player[0].triggx: %d", player[0].triggerx); printf("	player[0].triggy: %d\n", player[0].triggery);
	printf("player[0].hp: %d", (player[0].hp));	printf("	player[0].q: %d", player[0].q);	printf("	player[0].info.dir: %d", player[0].info.dir);	printf("	player[0].d: %d\n", player[0].d);
		printf("pdir=%d\n", player[0].pdir);
	printf("tile data value at player trigger position: %d\n", mv[0].map->data[(player[0].triggerx-1)+(map.width*(player[0].triggery-1))]);
//	printf("palette offset:	%d\n", paloffset/3);
//++++	printf("Total used: %zu\n", oldfreemem-GetFreeSize());
//++++	printf("Total free: %zu\n", GetFreeSize());
//not used now	printf("temporary player sprite 0: http://www.pixiv.net/member_illust.php?mode=medium&illust_id=45556867\n");
//not used now	printf("temporary player sprite 1: http://www.pixiv.net/member_illust.php?mode=medium&illust_id=44606385\n");
	printf("Virtual Screen: %dx", gvar.video.page[0].width);	printf("%d\n", gvar.video.page[0].height);
	printf("Screen: %dx", gvar.video.page[0].sw);	printf("%d\n", gvar.video.page[0].sh);
	printf("tile resolution: %dx", gvar.video.page[0].tilesh);	printf("%d\n", gvar.video.page[0].tilesh);
	printf("middle tile position: %dx", gvar.video.page[0].tilemidposscreenx);	printf("%d\n", gvar.video.page[0].tilemidposscreeny);
//	printf("Screen2: %dx", gvar.video.page[1].width);	printf("%d\n", gvar.video.page[1].height);
//	printf("map: %dx%d\n", map.width, map.height);
//	printf("\n");
//	printf("player[0].info.x: %d", player[0].info.xaxis); printf("		player[0].info.y: %d\n", player[0].info.yaxis);
//	printf("player[0].info.tx: %d", player[0].info.x); printf("		player[0].info.ty: %d\n", player[0].info.y);
	//printf("map.width=%d	map.height=%d	map.data[0]=%d\n", mv[0].map->width, mv[0].map->height, mv[0].map->data[0]);

	switch(detectcpu())
	{
		case 0: cpus = "8086/8088 or 186/88"; break;
		case 1: cpus = "286"; break;
		case 2: cpus = "386 or newer"; break;
		default: cpus = "internal error"; break;
	}
	printf("detected CPU type: %s\n", cpus);
#ifdef MODEX
#ifdef FADE
	modexFadeOn(4, dpal);
#endif
#endif
}
