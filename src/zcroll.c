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
#include "src/lib/16render.h"
#include "src/lib/16_dbg.h"

#define FADE

static map_t map;
map_view_t mv[4];
float t;

//debugswitches
boolean panswitch=0,baka=0;
//extern boolean pageflipflop=1;
extern boolean pagenorendermap;	//default: 0
unsigned int i;

#ifdef FADE
//static word paloffset=0;
#endif
byte *ptr;
memptr pal;

#define FILENAME_1	"data/spri/chikyuu.vrs"
#define FILENAME_1P	"data/spri/chikyuu.pal"
#define FILENAME_2	"data/spri/ptmp.vrs"
#define FILENAME_2P	"data/spri/ptmp.pal"

void main(int argc, char *argv[])
{
	static global_game_variables_t gvar;
	static player_t player[MaxPlayers];
	char *bakapee1,*bakapee1p;
//	sword bakapee;
// 	if(argv[1]) bakapee = atoi(argv[1]);
// 	else bakapee = 1;

	Startup16(&gvar);

	bakapee1=malloc(64);
	bakapee1p=malloc(64);

	if (argc < 2) {
		//fprintf(stderr,"drawvrl <VRL file> <palette file>\n palette file optional\n");
		bakapee1 =		FILENAME_1;
		bakapee1p =	FILENAME_1P;

	}else{
		if(argv[1]) bakapee1 = argv[1];
		if(argv[2]) bakapee1p = argv[2];
	}

	// OK, this one takes hellova time and needs to be done in farmalloc or MM_...
	//IN CA i think you use CAL_SetupGrFile but i do think we should work together on this part --sparky4
	//player[0].ent = malloc(sizeof(entity_t));
	player[0].enti.spri = malloc(sizeof(struct sprite));
	player[0].enti.spri->spritesheet = malloc(sizeof(struct vrs_container));

	// create the map
//	fprintf(stderr, "testing map load~	");
	CA_loadmap("data/test.map", &map, &gvar);
	chkmap(&map, 0);
//	initMap(&map);
//	printf("chkmap ok	");
//	fprintf(stderr, "yay map loaded~~\n");

	// data
	VRS_LoadVRS(bakapee1, &player[0].enti, &gvar);

	// input!
	IN_Default(0, &player,ctrl_Keyboard1);

	// save the palette
#ifdef FADE
	modexPalSave(&gvar.video.dpal);
	modexFadeOff(4, &gvar.video.dpal);
	modexPalBlack();
#endif

	VGAmodeX(1/*bakapee*/, 1, &gvar);

	/* load color palette */
	/*ptmp.offset=(paloffset/3);
	modexPalUpdate(&ptmp, &paloffset, 0, 0);
	//modexClearRegion(bg->page, 0, 0, 320, 240, 255);
	//printf("	%d\n", sizeof(ptmp.data));
	//printf("1:	%d\n", paloffset);
	map.tiles->data->offset=(paloffset/3);
	modexPalUpdate(map.tiles->data, &paloffset, 0, 0);*/
	VL_LoadPalFile(bakapee1p, &gvar.video.palette);
	//VL_LoadPalFile("data/default.pal", &gvar.video.palette);

#ifdef FADE
	modexPalSave(&gvar.video.palette);
	modexSavePalFile("data/g.pal", &gvar.video.palette);
	modexPalBlack();	//so player will not see loadings~
#endif

	// setup camera and screen~
	modexHiganbanaPageSetup(&gvar.video);
	ZC_MVSetup(&mv, &map, &gvar);

	// set up paging
	//TODO: LOAD map data and position the map in the middle of the screen if smaller then screen
	mapGoTo(&mv, 0, 0);

	ZC_playerXYpos(0, 0, &player, &mv, 0, 1);
	EN_initplayer(&player, 0, &gvar.video);
	//print_anim_ids(player[0].enti.spri);
	if (gvar.video.sprifilei == -1)
	{
#ifdef FADE
		modexFadeOff(4, &gvar.video.palette);
#endif
		Quit(&gvar, "Wrong ID for sprite");
#ifdef FADE
		modexFadeOn(4, &gvar.video.dpal);
#endif
	}

//	while(!IN_KeyDown(sc_Escape) && !IN_KeyDown(sc_Space) && !IN_KeyDown(sc_Enter)){ FUNCTIONKEYSHOWMV }
	gvar.video.page[0].tlx=mv[0].tx*TILEWH;
	gvar.video.page[0].tly=mv[0].ty*TILEWH;
	shinku(&gvar);
//modexpdump(mv[0].page);
#ifdef FADE
	modexFadeOn(4, &gvar.video.palette);
#endif
	while(!IN_KeyDown(sc_Escape) && player[0].enti.hp>0)
	{
		gvar.video.page[0].tlx=mv[0].tx*TILEWH;
		gvar.video.page[0].tly=mv[0].ty*TILEWH;
		shinku(&gvar);
		//top left corner & bottem right corner of map veiw be set as map edge trigger since maps are actually square
		//to stop scrolling and have the player position data move to the edge of the screen with respect to the direction
		//when player[0].tx or player[0].ty == 0 or player[0].tx == 20 or player[0].ty == 15 then stop because that is edge of map and you do not want to walk of the map

		//player movement
		IN_ReadControl(0, &player);
		if(!panswitch){
			//ZC_walk2(player[0].ent, mv);
			ZC_walk(&mv, &player, 0);
		}else{
			PANKEYFUNZC;
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

		if(IN_KeyDown(24)){ modexPalUpdate0(&gvar.video.palette); /*paloffset=0;*/ modexpdump(mv[0].page); IN_UserInput(1,1); } //o
		if(IN_KeyDown(22)){ modexPalUpdate0(&gvar.video.palette); } //u

		FUNCTIONKEYFUNCTIONS
		FUNCTIONKEYDRAWJUNK
		if(IN_KeyDown(sc_L)){ modexClearRegion(&gvar.video.page[0], player[0].enti.x, player[0].enti.y, 16, 16, 1); }
		if(IN_KeyDown(sc_J) || IN_KeyDown(sc_K))
		{
			if(IN_KeyDown(sc_J))
			{
				bakapee1=FILENAME_1;
				bakapee1p=FILENAME_1P;
				player[0].enti.overdraww=0;
			}
			if(IN_KeyDown(sc_K))
			{
				bakapee1=FILENAME_2;
				bakapee1p=FILENAME_2P;
				player[0].enti.overdraww=2;
			}
			//read_vrs(&gvar, bakapee1, player[0].enti.spri->spritesheet);
			VRS_ReadVRS(bakapee1, &player[0].enti, &gvar);
			VL_LoadPalFile(bakapee1p, &gvar.video.palette);
		}//JK
#ifdef FADE
		if(IN_KeyDown(10)){ modexPalOverscan(rand()%56); modexPalUpdate(gvar.video.dpal); IN_UserInput(1,1); }
#endif
		if(IN_KeyDown(sc_R)){ modexPalOverscan(rand()%56); } //r

		if((player[0].enti.q==1) && !(player[0].enti.x%TILEWH==0 && player[0].enti.y%TILEWH==0)) Quit(&gvar, "PLAYER OFF THE RAILS!");//break;	//incase things go out of sync!
	}

	/* fade back to text mode */
	/* but 1st lets save the game palette~ */
#ifdef FADE
	modexPalSave(&gvar.video.palette);
	modexSavePalFile("data/g.pal", &gvar.video.palette);
	modexFadeOff(4, &gvar.video.palette);
#endif
	Shutdown16(&gvar);
	printf("\nProject 16 zcroll.exe. This is just a test file!\n");
	printf("version %s\n", VERSION);
	SCROLLEXITMESG;
	WCPU_cpufpumesg();
#ifdef FADE
	modexFadeOn(4, gvar.video.dpal);
#endif
}
