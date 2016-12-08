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
#include "src/lib/16_dbg.h"

#define MODEXZ
boolean dbg_noplayerinpu=0;

//TODO: known issues the array dependent mv stuff and player arrays
global_game_variables_t gvar;
static map_t map;
player_t player[MaxPlayers];
map_view_t mv[4];
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
#ifdef FADE
static word paloffset=0;
byte *dpal;
#endif
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
	//TODO: USE MM AND CA AND PM WWWWWWWW
	player[0].ent = malloc(sizeof(entity_t));
	player[0].ent->spri = malloc(sizeof(struct sprite));
	player[0].ent->spri->spritesheet = malloc(sizeof(struct vrs_container));

	// create the map
	fprintf(stderr, "testing map load~	");
	loadmap("data/test.map", &map);
	chkmap(&map, 0);
	printf("chkmap ok	");
	fprintf(stderr, "yay map loaded~~\n");

	// data
	read_vrs(&gvar, "data/spri/chikyuu.vrs", player[0].ent->spri->spritesheet);

	//	input!
if(!dbg_noplayerinpu)
	IN_Default(0, &player,ctrl_Keyboard1);

	// save the palette
#ifdef FADE
	dpal = modexNewPal();
	modexPalSave(dpal);
	modexFadeOff(4, dpal);
#endif
	textInit();
	VGAmodeX(bakapee, 1, &gvar);
#ifdef MODEXZ
#ifdef FADE
	modexPalBlack();	//reset the palette~
	CA_LoadFile("data/spri/chikyuu.pal", &pal, &gvar);
	modexPalUpdate1(pal);
	gpal = modexNewPal();
	modexPalSave(gpal);
	modexSavePalFile("data/g.pal", gpal);
	modexPalBlack();	//so player will not see loadings~
#endif

	// setup camera and screen~
	strcpy(global_temp_status_text, "press a key for video setup");
	modexprint(mv[0].page, 0, 64, 1, 7, 0, global_temp_status_text);

	modexHiganbanaPageSetup(&gvar.video);
	mv[0].page = &gvar.video.page[0];
	mv[0].map = &map;
	mv[0].video = &gvar.video;
	mv[0].pan	= &pan;
	player[0].ent->spri->x = player[0].ent->spri->y = 20;

	// set up paging
	//TODO: LOAD map data and position the map in the middle of the screen if smaller then screen
	mapGoTo(mv, 0, 0);
#endif

	//TODO: put player in starting position of spot
	//default player position on the viewable map
	player[0].tx = mv[0].tx + mv[0].page->tilemidposscreenx;
	player[0].ty = mv[0].ty + mv[0].page->tilemidposscreeny;
	IN_initplayer(&player, 0);

	i = set_anim_by_id(player[0].ent->spri, 11);
	print_anim_ids(player[0].ent->spri);
	if (i == -1)
	{
#ifdef FADE
		modexFadeOff(4, gpal);
#endif
	VGAmodeX(0, 1, &gvar);
	Shutdown16(&gvar);
	printf("Wrong");
#ifdef FADE
		modexFadeOn(4, dpal);
#endif
	exit(-4);
	}
	//animate_spri(&(player[0].ent->spri));

	modexShowPage(mv[0].page);//!(gvar.video.p)
	shinku_fps_indicator_page = 0; // we're on page 1 now, shinku(). follow along please or it will not be visible.
#ifdef FADE
	modexFadeOn(4, gpal);
#endif
	if(!dbg_noplayerinpu)
	while(!IN_KeyDown(sc_Escape))// && player[0].hp>0)
	{
		shinku(&gvar);
		//top left corner & bottem right corner of map veiw be set as map edge trigger since maps are actually square
		//to stop scrolling and have the player position data move to the edge of the screen with respect to the direction
		//when player[0].tx or player[0].ty == 0 or player[0].tx == 20 or player[0].ty == 15 then stop because that is edge of map and you do not want to walk of the map

		//player movement
		IN_ReadControl(0, &player);
		if(!panswitch){
			walk(player[0].ent, mv);
			//walk(mv, &player, 0);
		}

		//the scripting stuff....
/*		if(((mv[0].map->data[(player[0].triggerx-1)+(map.width*(player[0].triggery-1))] == 0) && IN_KeyDown(0x1C))||(player[0].tx == 5 && player[0].ty == 5))
		{
			short i;
			for(i=800; i>=400; i--)
			{
				sound(i);
			}
			nosound();
		}
		if(player[0].q == (TILEWH/(player[0].speed))+1 && player[0].info.dir != 2 && (player[0].triggerx == 5 && player[0].triggery == 5)){ player[0].hp--; }
*/		//debugging binds!
		if(IN_KeyDown(2)){ modexShowPage(mv[0].page); pan.pn=0; }
		if(IN_KeyDown(25)){ modexpdump(mv[0].page);
			 IN_UserInput(1,1);
		}	//p
#ifdef FADE
		if(IN_KeyDown(24)){ modexPalUpdate0(gpal); paloffset=0; modexpdump(mv[0].page); IN_UserInput(1,1); }
		if(IN_KeyDown(22)){
			printf("2paloffset	=	%d\n", paloffset/3);
			IN_UserInput(1,1);
		}
#endif

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
			modexClearRegion(mv[0].page, 0, 0, mv[0].page->width, mv[0].page->height, 2);
		}
		if(IN_KeyDown(66))	//f8
		{
	//		modexDrawSprite(mv[0].page, 16, 16, p);
			modexDrawSprite(mv[0].page, 32+48, 16, (player[0].data));
		}
		//TODO fmemtest into page
		/*if(IN_KeyDown(4+1))	//4
		{
			pg=1;
			SELECT_ALL_PLANES();
			_fmemset(((mv[pg].page->data+4)+(16*(mv[pg].page->width/4))), 15, 4);
		}*/

		//9
#ifdef FADE
		if(IN_KeyDown(10)){ modexPalOverscan(rand()%56); modexPalUpdate1(dpal); IN_UserInput(1,1); }
		//if(IN_KeyDown(11)){ modexPalOverscan(15); }
#endif
		if((player[0].q==1) && !(player[0].x%TILEWH==0 && player[0].y%TILEWH==0)) break;	//incase things go out of sync!
		player[0].hp = 0;
	}

	else
		while(!kbhit())
		{
			shinku(&gvar);
		}

	/* fade back to text mode */
	/* but 1st lets save the game palette~ */
#ifdef FADE
	modexPalSave(gpal);
	modexSavePalFile("data/g.pal", gpal);
	modexFadeOff(4, gpal);
#endif
	VGAmodeX(0, 1, &gvar);
	Shutdown16(&gvar);
	printf("\nProject 16 zcroll.exe. This is just a test file!\n");
	printf("version %s\n", VERSION);
	printf("tx: %d	", mv[0].tx);
	printf("ty: %d\n", mv[0].ty);
	printf("\n");
	printf("player vars:\n");
	printf("	x: %d", player[0].x); printf("	y: %d\n", player[0].y);
	//if(player[0].hp==0) printf("%d wwww\n", player[0].y+8);
	//else printf("\nplayer[0].y: %d\n", player[0].y);
	printf("	tx: %d", player[0].tx); printf("	ty: %d\n", player[0].ty);
	printf("	triggx: %d", player[0].triggerx); printf("	triggy: %d\n", player[0].triggery);
	printf("	hp: %d", (player[0].hp));	printf("	q: %d", player[0].q);	printf("	player.info.dir: %d", player[0].info.dir);	printf("	player.d: %d ", player[0].d);
		printf("	pdir=%d\n", player[0].pdir);
	printf("	tile data value at player trigger position: %d\n\n", mv[0].map->data[(player[0].triggerx-1)+(map.width*(player[0].triggery-1))]);
	printf("Virtual Screen: %dx", gvar.video.page[0].width);	printf("%d	", gvar.video.page[0].height);
	printf("Screen: %dx", gvar.video.page[0].sw);	printf("%d\n", gvar.video.page[0].sh);
	printf("virtual tile resolution: %dx", gvar.video.page[0].tilesw);	printf("%d	", gvar.video.page[0].tilesh);
	printf("tile resolution: %dx", gvar.video.page[0].tw);	printf("%d\n", gvar.video.page[0].th);
	printf("middle tile position: %dx", gvar.video.page[0].tilemidposscreenx);	printf("%d\n", gvar.video.page[0].tilemidposscreeny);
	modexprintmeminfo(&gvar.video);
	//printf("mv[%u].tx: %d", pan.pn, mv[pan.pn].tx); printf("	mv[%u].ty: %d	", pan.pn, mv[pan.pn].ty);
	printf("gvar.video.p=%u ", gvar.video.p); printf("gvar.video.r=%u ", gvar.video.r);
	printf("pageflipflop=%u\n", pageflipflop);
	printf("\n");
	//printf("map.width=%d	map.height=%d	map.data[0]=%d\n", mv[0].map->width, mv[0].map->height, mv[0].map->data[0]);

	printf("\n");
	switch(detectcpu())
	{
		case 0: cpus = "8086/8088 or 186/88"; break;
		case 1: cpus = "286"; break;
		case 2: cpus = "386 or newer"; break;
		default: cpus = "internal error"; break;
	}
	printf("detected CPU type: %s\n", cpus);
#ifdef FADE
	modexFadeOn(4, dpal);
#endif
}
