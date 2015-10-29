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

#include "src/lib/scroll16.h"
#include "src/lib/timer.h"
#include "src/lib/wcpu/wcpu.h"

//#define FADE
#define MODEX	//this is for mode x initiating

//word far *clock= (word far*) 0x046C; /* 18.2hz clock */

global_game_variables_t gvar;
static map_t map;
player_t player[MaxPlayers];
//page_t screen, gvar.video.page[1], gvar.video.page[2];
map_view_t mv[3];
map_view_t *bg, *spri, *mask;//, *tmp;
bitmap_t p;
float t;
sword bakapee;

void main(int argc, char *argv[])
{
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
	byte *mesg=malloc(sizeof(dword));


	if(argv[1]) bakapee = atoi(argv[1]);
	else bakapee = 1;

	player[0].persist_aniframe=0;
	player[0].speed=4;

	printf("starting timer\n");
	start_timer(&gvar);
	printf("ok\n");
	//extern struct inconfig inpu;

//	atexit(qclean());

	/* create the map */
	fprintf(stderr, "testing map load~\n");
	loadmap("data/test.map", &map);
//0000	map.width=0;
//0000	map.height=0;
	chkmap(&map, 0);
	printf("chkmap ok\n");
	fprintf(stderr, "yay map loaded~~\n");
	mv[0].map = &map;
	mv[1].map = &map;
	mv[2].map = &map;

	/* draw the tiles */
#ifdef MODEX
	ptr = map.data;
	mappalptr = map.tiles->data->palette;

	/* data */
	p = bitmapLoadPcx("data/ptmp.pcx"); // load sprite
	//npctmp = bitmapLoadPcx("ptmp1.pcx"); // load sprite

	/* create the planar buffer */
	(player[0].data) = *planar_buf_from_bitmap(&p);
//0000	printf("planar buffer ok\n");
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
	VGAmodeX(bakapee, &gvar);
	printf("%dx%d\n", gvar.video.page[0].sw, gvar.video.page[0].sh);
#ifdef FADE
	modexPalBlack();	//reset the palette~
#endif
#endif
//	printf("Total used @ before palette initiation:		%zu\n", oldfreemem-GetFreeSize());
//++++	player[0].data.offset=(paloffset/3);
//++++	modexPalUpdate1(&player[0].data, &paloffset, 0, 0);
		modexPalUpdate1(p.palette);
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
	gvar.video.page[0].height += (TILEWH*2);//+QUADWH;
	mv[0].page = &gvar.video.page[0];
	gvar.video.page[1] = modexNextPage(mv[0].page);
	mv[1].page = &gvar.video.page[1];
	gvar.video.page[2] = modexNextPageFlexibleSize(mv[1].page, gvar.video.page[0].sw, gvar.video.page[0].sh);	//(352*176)+1024 is the remaining amount of memory left wwww
	//gvar.video.page[2] = modexNextPage0(mv2.page, 320, 192);	//(352*176)+1024 is the remaining amount of memory left wwww
	mv[2].page = &gvar.video.page[2];

	/* set up paging */
	bg = &mv[0];
	spri = &mv[1];
	mask = &mv[2];

//TODO: LOAD map data and position the map in the middle of the screen if smaller then screen
	mapGoTo(bg, 0, 0);
	mapGoTo(spri, 0, 0);
	//mapGoTo(mask, 0, 0);

	//TODO: put player in starting position of spot
	//default player position on the viewable map
	player[0].tx = bg->tx + bg->page->tilemidposscreenx;
	player[0].ty = bg->ty + bg->page->tilemidposscreeny;
	player[0].x = player[0].tx*TILEWH;
	player[0].y = player[0].ty*TILEWH;
	player[0].triggerx = player[0].tx;
	player[0].triggery = player[0].ty+1;
/*	player[0].info.x = player[0].tx;
	player[0].info.xaxis = player[0].tx*TILEWH;
	player[0].info.y = player[0].ty;
	player[0].info.yaxis = player[0].ty*TILEWH;*/
	player[0].q=1;
	player[0].d=2;
	player[0].hp=4;
//	player[0].persist_aniframe=0;
	//npc
	/*npc0.tx = bg->tx + 1;
	npc0.ty = bg->ty + 1;
	npc0.x = npc0.tx*TILEWH;
	npc0.y = npc0.ty*TILEWH;
	npc0.triggerx = npc0.tx;
	npc0.triggery = npc0.ty+1;
	npc0.q=1;
	npc0.d=0;
	modexDrawSpriteRegion(spri->page, npc0.x-4, npc0.y-TILEWH, 24, 64, 24, 32, &npctmp);*/
	modexCopyPageRegion(mv[1].page, mv[0].page, 0, 0, 0, 0, mv[0].page->width, mv[0].page->height);
#ifdef	SPRITE
#ifdef BMPTYPE
	//oldDrawBmp(VGA, player[0].x-4, player[0].y-TILEWH, &player[0].data, 1);
#else
	modexDrawSpriteRegion(spri->page, player[0].x-4, player[0].y-TILEWH, 24, 64, 24, 32, &player[0].data);
#endif
#else
	modexClearRegion(mv[1].page, player[0].x-4, player[0].y-TILEWH, 24, 32, 15);
#endif

	modexShowPage(spri->page);
	modexClearRegion(mv[2].page, 0, 0, mv[2].page->width, mv[2].page->height, 1);
#ifdef MODEX
#ifdef FADE
	modexFadeOn(4, gpal);
#endif
#endif
	while(!IN_KeyDown(sc_Escape) && player[0].hp>0)
	{
		shinku(mv, &gvar);
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
						bg->page->dx++;
						modexShowPage(bg->page);
					break;
					case 1:
						//spri
						spri->page->dx++;
						modexShowPage(spri->page);
					break;
					case 2:
						//fg
						mask->page->dx++;
						modexShowPage(mask->page);
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
						bg->page->dx--;
						modexShowPage(bg->page);
					break;
					case 1:
						//spri
						spri->page->dx--;
						modexShowPage(spri->page);
					break;
					case 2:
						//fg
						mask->page->dx--;
						modexShowPage(mask->page);
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
						bg->page->dy--;
						modexShowPage(bg->page);
					break;
					case 1:
						//spri
						spri->page->dy--;
						modexShowPage(spri->page);
					break;
					case 2:
						//fg
						mask->page->dy--;
						modexShowPage(mask->page);
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
						bg->page->dy++;
						modexShowPage(bg->page);
					break;
					case 1:
						//spri
						spri->page->dy++;
						modexShowPage(spri->page);
					break;
					case 2:
						//fg
						mask->page->dy++;
						modexShowPage(mask->page);
					break;
				}
				panq++;
			} else { panq = 1; pand = 0; }
	}
}*/


	//the scripting stuf....

	//if(((player[0].triggerx == TRIGGX && player[0].triggery == TRIGGY) && IN_KeyDown(0x1C))||(player[0].tx == 5 && player[0].ty == 5))
	if(((bg->map->data[(player[0].triggerx-1)+(map.width*(player[0].triggery-1))] == 0) && IN_KeyDown(0x1C))||(player[0].tx == 5 && player[0].ty == 5))
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
	if(IN_KeyDown(2)){ modexShowPage(bg->page); panpagenum=0; }
	if(IN_KeyDown(3)){ modexShowPage(spri->page); panpagenum=1; }
	if(IN_KeyDown(4)){ modexShowPage(mask->page); panpagenum=2; }
	if(IN_KeyDown(25)){ pdump(bg->page); pdump(spri->page); }	//p
#ifdef MODEX
#ifdef FADE
	if(IN_KeyDown(24)){ modexPalUpdate0(gpal); paloffset=0; pdump(bg->page); pdump(spri->page); }
	if(IN_KeyDown(22)){
	paloffset=0; modexPalBlack(); modexPalUpdate(&player[0].data, &paloffset, 0, 0);
	printf("1paloffset	=	%d\n", paloffset/3);
	 modexPalUpdate(map.tiles->data, &paloffset, 0, 0);
	printf("2paloffset	=	%d\n", paloffset/3);
	 pdump(bg->page); pdump(spri->page); }
#endif
#endif
	//pan switch
	//if(IN_KeyDown(88)){if(!panswitch) panswitch++; else panswitch--; }	//f12
	//TSR
	if(IN_KeyDown(87))	//f11
	{
		VGAmodeX(0, &gvar);
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
	VGAmodeX(0, &gvar);
#endif
	IN_Shutdown();
	printf("Project 16 scroll.exe. This is just a test file!\n");
	printf("version %s\n", VERSION);
	printf("tx: %d\n", bg->tx);
	printf("ty: %d\n", bg->ty);
	printf("player[0].x: %d", player[0].x); printf("		player[0].y: %d\n", player[0].y);
	//if(player[0].hp==0) printf("%d wwww\n", player[0].y+8);
	//else printf("\nplayer[0].y: %d\n", player[0].y);
	printf("player[0].tx: %d", player[0].tx); printf("		player[0].ty: %d\n", player[0].ty);
	printf("player[0].triggx: %d", player[0].triggerx); printf("	player[0].triggy: %d\n", player[0].triggery);
	printf("player[0].hp: %d", (player[0].hp));	printf("	player[0].q: %d", player[0].q);	printf("	player[0].info.dir: %d", player[0].info.dir);	printf("	player[0].d: %d\n", player[0].d);
	printf("tile data value at player trigger position: %d\n", bg->map->data[(player[0].triggerx-1)+(map.width*(player[0].triggery-1))]);
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
	//printf("map.width=%d	map.height=%d	map.data[0]=%d\n", bg->map->width, bg->map->height, bg->map->data[0]);

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
