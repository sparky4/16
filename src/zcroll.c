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

#include "src/lib/zcroll16.h"
#include "src/lib/16_timer.h"
#include "src/lib/wcpu/wcpu.h"
#include "src/lib/16render.h"

global_game_variables_t gvar;
static map_t map;
player_t *player;
map_view_t *mv;
float t;
sword bakapee;
pan_t pan;
//debugswitches
boolean panswitch=0,baka=0;
//extern boolean pageflipflop=1;
unsigned int i;
const char *cpus;
//static int persist_aniframe = 0;    /* gonna be increased to 1 before being used, so 0 is ok for default */

//map_view_db_t pgid[4];
word pg;
//#ifdef FADE
static word paloffset=0;
byte *dpal;
//#endif
byte *gpal;
byte *ptr;
memptr pal;

void main(int argc, char *argv[])
{
	if(argv[1]) bakapee = atoi(argv[1]);
	else bakapee = 1;

	Startup16(&gvar);

	pan.pn=0;
	// OK, this one takes hellova time and needs to be done in farmalloc or MM_...
	player = malloc(sizeof(player_t));
	player->ent = malloc(sizeof(entity_t));
	player->ent->spri = malloc(sizeof(struct sprite));
	player->ent->spri->spritesheet = malloc(sizeof(struct vrs_container));

	// create the map
	fprintf(stderr, "testing map load~	");
	loadmap("data/test.map", &map);
	chkmap(&map, 0);
	printf("chkmap ok	");
	fprintf(stderr, "yay map loaded~~\n");

//goto pee;
	// data
	read_vrs(&gvar, "data/spri/chikyuu.vrs", player->ent->spri->spritesheet);

	//	input!
	IN_Default(0, player,ctrl_Joystick);

	// save the palette
	dpal = modexNewPal();
	modexPalSave(dpal);
	modexFadeOff(4, dpal);

	textInit();
	VGAmodeX(bakapee, 1, &gvar);
	modexPalBlack();	//reset the palette~

	CA_LoadFile("data/spri/chikyuu.pal", &pal, &gvar);
	modexPalUpdate1(pal);

	gpal = modexNewPal();
	modexPalSave(gpal);
	modexSavePalFile("data/g.pal", gpal);
	modexPalBlack();	//so player will not see loadings~
	// setup camera and screen~
	modexHiganbanaPageSetup(&gvar.video);
	mv->page = &gvar.video.page[0];
	mv->map = &map;
	mv->video = &gvar.video;
	mv->pan	= &pan;
	player->ent->spri->x = player->ent->spri->y = 20;

	// set up paging
	//TODO: LOAD map data and position the map in the middle of the screen if smaller then screen
	mapGoTo(mv, 0, 0);
	//_fmemcpy(mv[1].page->data, mv->page->data, mv->page->pagesize);

	//TODO: put player in starting position of spot
	//default player position on the viewable map
	player->tx = mv->tx + mv->page->tilemidposscreenx;
	player->ty = mv->ty + mv->page->tilemidposscreeny;
	IN_initplayer(player);
	//IN_initplayer(&player, 1);

#ifndef	SPRITE
	modexClearRegion(mv->page, player->x, player->y-TILEWH, 16, 32, 15);
	//modexClearRegion(mv[1].page, player->x, player->y-TILEWH, 16, 32, 15);
#else
	//PBUFSFUN(mv[1].page, player->x, player->y-TILEWH, 16, 64, 24, 32,	PLAYERBMPDATA);
//	PBUFSFUN(mv->page, player->x, player->y-TILEWH, 16, 64, 16, 32,	player->data);
	i = set_anim_by_id(player->ent->spri, 11);
	print_anim_ids(player->ent->spri);
	if (i == -1)
	{
		modexFadeOff(4, gpal);
	VGAmodeX(0, 1, &gvar);
	Shutdown16(&gvar);
	printf("Wrong");
		modexFadeOn(4, dpal);
return;
	}
return;
	//animate_spri(&(player->ent->spri));
#endif

	modexShowPage(mv->page);//!(gvar.video.p)
	shinku_fps_indicator_page = 0; // we're on page 1 now, shinku(). follow along please or it will not be visible.

	// buffer pages
// 	modexClearRegion(mv[2].page, 0, 0, mv[2].page->width, mv[2].page->height, 47);
// 	modexClearRegion(mv[3].page, 0, 0, mv[3].page->width, mv[3].page->height, 45);
// 	{
// 		unsigned int k,j,o;
// 		// fill screen with a distinctive pattern
// 		for (k=0;k < vga_state.vga_width;k++) {
// 			o = k >> 2;
// 			vga_write_sequencer(0x02/*map mask*/,1 << (k&3));
// 				for (j=0;j < vga_state.vga_height;j++,o += vga_state.vga_stride)
// 					vga_state.vga_graphics_ram[o] = (k^j)&15; // VRL samples put all colors in first 15!
// 		}
// 	}

//	modexFadeOn(4, gpal);*/
	while(!IN_KeyDown(sc_Escape) && player->hp>0)
	{
		shinku(&gvar);
		//top left corner & bottem right corner of map veiw be set as map edge trigger since maps are actually square
		//to stop scrolling and have the player position data move to the edge of the screen with respect to the direction
		//when player->tx or player->ty == 0 or player->tx == 20 or player->ty == 15 then stop because that is edge of map and you do not want to walk of the map

		//player movement
		IN_ReadControl(player);
		if(!panswitch){
			walk(player->ent, mv);
		}

		//the scripting stuff....
/*		if(((mv->map->data[(player->triggerx-1)+(map.width*(player->triggery-1))] == 0) && IN_KeyDown(0x1C))||(player->tx == 5 && player->ty == 5))
		{
			short i;
			for(i=800; i>=400; i--)
			{
				sound(i);
			}
			nosound();
		}
		if(player->q == (TILEWH/(player->speed))+1 && player->info.dir != 2 && (player->triggerx == 5 && player->triggery == 5)){ player->hp--; }
*/		//debugging binds!
		if(IN_KeyDown(2)){ modexShowPage(mv->page); pan.pn=0; }
		if(IN_KeyDown(25)){ modexpdump(mv->page);
			 IN_UserInput(1,1);
		}	//p
		if(IN_KeyDown(24)){ modexPalUpdate0(gpal); paloffset=0; modexpdump(mv->page); IN_UserInput(1,1); }
		if(IN_KeyDown(22)){
			printf("2paloffset	=	%d\n", paloffset/3);
			IN_UserInput(1,1);
		}

		//pan switch
		if(IN_KeyDown(88)){panswitch=!panswitch; IN_UserInput(1,1);}	//f12
		if(IN_KeyDown(87))	//f11
		{
			pageflipflop=!pageflipflop;
			IN_UserInput(1,1);
		}
		if(IN_KeyDown(68))	//f10
		{
			gvar.kurokku.fpscap=!gvar.kurokku.fpscap;
			IN_UserInput(1,1);
		}
		if(IN_KeyDown(67))	//f9
		{
			modexClearRegion(mv->page, 0, 0, mv->page->width, mv->page->height, 2);
		}
		if(IN_KeyDown(66))	//f8
		{
	//		modexDrawSprite(mv->page, 16, 16, p);
			modexDrawSprite(mv->page, 32+48, 16, (player->data));
		}
		//TODO fmemtest into page
		/*if(IN_KeyDown(4+1))	//4
		{
			pg=1;
			SELECT_ALL_PLANES();
			_fmemset(((mv[pg].page->data+4)+(16*(mv[pg].page->width/4))), 15, 4);
		}*/

		//9
		if(IN_KeyDown(10)){ modexPalOverscan(rand()%56); modexPalUpdate1(dpal); IN_UserInput(1,1); }
		//if(IN_KeyDown(11)){ modexPalOverscan(15); }
		if((player->q==1) && !(player->x%TILEWH==0 && player->y%TILEWH==0)) break;	//incase things go out of sync!
		player->hp = 0;
	}
//pee:
	/* fade back to text mode */
	/* but 1st lets save the game palette~ */
	modexPalSave(gpal);
	modexSavePalFile("data/g.pal", gpal);
	modexFadeOff(4, gpal);
	VGAmodeX(0, 1, &gvar);
	Shutdown16(&gvar);
	printf("\nProject 16 scroll.exe. This is just a test file!\n");
	printf("version %s\n", VERSION);
	printf("tx: %d	", mv->tx);
	printf("ty: %d\n", mv->ty);
	printf("\n");
	printf("player vars:\n");
	printf("	x: %d", player->x); printf("	y: %d\n", player->y);
	//if(player->hp==0) printf("%d wwww\n", player->y+8);
	//else printf("\nplayer->y: %d\n", player->y);
	printf("	tx: %d", player->tx); printf("	ty: %d\n", player->ty);
	printf("	triggx: %d", player->triggerx); printf("	triggy: %d\n", player->triggery);
	printf("	hp: %d", (player->hp));	printf("	q: %d", player->q);	printf("	player.info.dir: %d", player->info.dir);	printf("	player.d: %d ", player->d);
		printf("	pdir=%d\n", player->pdir);
	printf("	tile data value at player trigger position: %d\n\n", mv->map->data[(player->triggerx-1)+(map.width*(player->triggery-1))]);
	printf("Virtual Screen: %dx", gvar.video.page[0].width);	printf("%d	", gvar.video.page[0].height);
	printf("Screen: %dx", gvar.video.page[0].sw);	printf("%d\n", gvar.video.page[0].sh);
	printf("virtual tile resolution: %dx", gvar.video.page[0].tilesw);	printf("%d	", gvar.video.page[0].tilesh);
	printf("tile resolution: %dx", gvar.video.page[0].tw);	printf("%d\n", gvar.video.page[0].th);
	printf("middle tile position: %dx", gvar.video.page[0].tilemidposscreenx);	printf("%d\n", gvar.video.page[0].tilemidposscreeny);
	modexprintmeminfo(&gvar.video);
	//printf("mv[%u].tx: %d", pan.pn, mv[pan.pn].tx); printf("	mv[%u].ty: %d	", pan.pn, mv[pan.pn].ty);
	printf("gvar.video.p=%u ", gvar.video.p); printf("gvar.video.r=%u ", gvar.video.r);
	printf("pageflipflop=%u\n", pageflipflop);
	//0000printf("\ngvar.video.tickclk=%f\n", gvar.video.tickclk);
	//0000printf("gvar.video.clk=%f", gvar.video.clk);
	printf("\n");
	//printf("map.width=%d	map.height=%d	map.data[0]=%d\n", mv->map->width, mv->map->height, mv->map->data[0]);

	printf("\n");
	switch(detectcpu())
	{
		case 0: cpus = "8086/8088 or 186/88"; break;
		case 1: cpus = "286"; break;
		case 2: cpus = "386 or newer"; break;
		default: cpus = "internal error"; break;
	}
	printf("detected CPU type: %s\n", cpus);
	modexFadeOn(4, dpal);
}
