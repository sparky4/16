/* Project 16 Source Code~
 * Copyright (C) 2012-2022 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

//#define FADE

//#define NOMAPLOAD
//#define OLDPLAYERSPRITESTUFF
#define XC_CTRLTYPE ctrl_Keyboard//ctrl_Joystick1//ctrl_Mouse
#ifdef __DEBUG__
#define SCROLLEXEDEBUG
#endif

//static map_t map;
#ifdef OLDPLAYERSPRITESTUFF
#include "src/lib/bitmapl.h"
static int persist_aniframe = 0;    /* gonna be increased to 1 before being used, so 0 is ok for default */
#define INC_PER_FRAME if(gvar->player[pn].enti.q&1) persist_aniframe++; if(persist_aniframe>4) persist_aniframe = 1;

void modexDrawSprite(page_t *page, int x, int y, bitmap_t *bmp);
void modexDrawSpriteRegion(page_t *page, int x, int y, int rx, int ry, int rw, int rh, bitmap_t *bmp);
boolean ZCL_ScreenMidPosChk(map_view_t *map_v, nibble dir, int tx, int ty);
boolean ZCL_CollCheck(map_view_t *map_v, nibble dir, int tx, int ty);
boolean ZCL_mapEdgeChk(map_view_t *map_v, nibble dir, int tx, int ty, boolean pansw, boolean noscrollsw);
void oldwalk(global_game_variables_t *gvar, word pn, bitmap_t *pbmp);
void animatePlayer(global_game_variables_t *gvar, /*map_view_t *top, */short d1, short d2, int x, int y, int ls, int lp, bitmap_t *bmp);
#endif
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
//memptr pal;
boolean spriteswitch=0;

#define FILENAME_1	"data/spri/chikyuu.vrs"
#define FILENAME_1P	"data/spri/chikyuu.pal"
#define FILENAME_2	"data/spri/me.vrs"
#define FILENAME_2P	"data/spri/me.pal"
//#define FILENAME_3	"data/spri/chistnd.vrl"

void main(int argc, char *argv[])
{
#ifdef OLDPLAYERSPRITESTUFF
	bitmap_t ptmp; // player sprite
#endif
	static global_game_variables_t gvar;
	struct glob_game_vars *ggvv;

	char bakapee[64] = FILENAME_1;
	char bakapeep[64] = FILENAME_1P;
//	char bakapee3[64] = FILENAME_3;
#ifdef __DEBUG__
	dbg_nointest=0;
	dbg_joymousedelta=0;
#endif

	Startup16(&gvar);

	ggvv=&gvar;

	if (argc >= 2) {
		if(argv[1]){ strcpy(bakapee, argv[1]);
		if(argv[2]) strcpy(bakapeep, argv[2]); }
	}

	// OK, this one takes hellova time and needs to be done in farmalloc or MM_...
	//IN CA i think you use CAL_SetupGrFile but i do think we should work together on this part --sparky4
	//----gvar.player[0].enti.spri.spritesheet = malloc(sizeof(struct vrs_container));

	// create the map
		#ifdef SCROLLEXEDEBUG
	//strcpy(gvar.handle.datadumpfilename, "mmdump1.16w");	MM_DumpData (&gvar);
	//strcpy(gvar.handle.heapdumpfilename, "heap1.16w");		HC_heapdump(&gvar);
	//fprintf(stderr, "testing map load~	");
		#endif
#ifndef NOMAPLOAD
	CA_loadmap("data/test.map", &gvar.map, &gvar);
	chkmap(&gvar.map, 0);
#else
	chkmap(&gvar.map, 1);
#endif
	//initMap(&gvar.map);
		#ifdef SCROLLEXEDEBUG
	printf("chkmap ok\n");
//	fprintf(stderr, "yay map loaded~~\n");
	//strcpy(gvar.handle.datadumpfilename, "mmdump2.16w");	MM_DumpData (&gvar);
	//strcpy(gvar.handle.heapdumpfilename, "heap2.16w");		HC_heapdump(&gvar);
		#endif

	// data
#ifdef OLDPLAYERSPRITESTUFF
	ptmp = bitmapLoadPcx("data/chikyuu.pcx"); // load sprite
	modexPalUpdate(ptmp.palette);
#else
		#ifdef SCROLLEXEDEBUG
	fprintf(stderr, "VRS_LoadVRS~	");
	//strcpy(gvar.handle.datadumpfilename, "mmdump3.16w");	MM_DumpData (&gvar);
	//strcpy(gvar.handle.heapdumpfilename, "heap3.16w");		HC_heapdump(&gvar);
		#endif
	VRS_LoadVRS(bakapee, &gvar.player[0].enti, &gvar);
		#ifdef SCROLLEXEDEBUG
	printf("VRS load OK~\n");
	//strcpy(gvar.handle.datadumpfilename, "mmdump4.16w");	MM_DumpData (&gvar);
	//strcpy(gvar.handle.heapdumpfilename, "heap4.16w");		HC_heapdump(&gvar);
		#endif
#endif

	// input!
	IN_Default(0, &gvar.player[0],XC_CTRLTYPE, &gvar);
	IN_SetControlType(&gvar.player[0],XC_CTRLTYPE);

	// save the palette
#ifdef FADE
	modexPalSave(&gvar.video.dpal);
	modexFadeOff(4, &gvar.video.dpal);
	modexPalBlack();
#endif

	//--VGAmodeX(1/*bakapee*/, 1, &gvar);
	VL_Startup(&gvar);

	/* load color palette */
	/*ptmp.offset=(paloffset/3);
	modexPalUpdate(&ptmp, &paloffset, 0, 0);
	//modexClearRegion(bg->page, 0, 0, 320, 240, 255);
	//printf("	%d\n", sizeof(ptmp.data));
	//printf("1:	%d\n", paloffset);
	map.tiles->data->offset=(paloffset/3);
	modexPalUpdate(map.tiles->data, &paloffset, 0, 0);*/
	VL_LoadPalFile(bakapeep, &gvar.video.palette, &gvar);
	//VL_LoadPalFile("data/default.pal", &gvar.video.palette);

#ifdef FADE
	modexPalSave(&gvar.video.palette);
	modexSavePalFile("data/g.pal", &gvar.video.palette);
	modexPalBlack();	//so player will not see loadings~
#endif

	// setup camera and screen~
	modexHiganbanaPageSetup(&gvar);
	ZC_MVSetup(&gvar.mv, &gvar.map, &gvar);

#ifdef FADE
	modexFadeOn(4, &gvar.video.palette);
#endif

//0000	IN_StartAck (&gvar);	MM_ShowMemory(&gvar);	while (!IN_CheckAck (&gvar)){}
#ifdef FADE
	modexPalBlack();	//so player will not see loadings~
#endif

	// set up paging
	//TODO: LOAD map data and position the map in the middle of the screen if smaller then screen
	mapGoTo(&gvar.mv, 0, 0);

	ZC_PlayerXYpos(0, 0, &gvar.player[0], &gvar.mv, 1);
	EN_initPlayer(&gvar.player[0], &gvar.video);
#ifdef SPRITE
#ifndef OLDPLAYERSPRITESTUFF
	//print_anim_ids(gvar.player[0].enti.spri);
	if (gvar.video.sprifilei == -1)
	{
#ifdef FADE
		modexFadeOff(4, &gvar.video.palette);
#endif
		Quit (&gvar, "Wrong ID for sprite");
#ifdef FADE
		modexFadeOn(4, &gvar.video.dpal);
#endif
	}
#endif
#endif

//	while(!gvar.in.inst->Keyboard[sc_Escape) && !gvar.in.inst->Keyboard[sc_Space) && !gvar.in.inst->Keyboard[sc_Enter]){ FUNCTIONKEYSHOWMV }
	gvar.video.page[0].tlx=gvar.mv[0].tx*TILEWH;
	gvar.video.page[0].tly=gvar.mv[0].ty*TILEWH;

#ifdef OLDWALKSHOWPAGESTUFF
	VL_ShowPage(&(gvar.video.page[gvar.video.sp]), gvar.kurokku.fpscap, 0);
#endif

	shinku(&gvar);
//modexpdump(gvar.mv[0].page);
#ifdef FADE
	modexFadeOn(4, &gvar.video.palette);
#endif
	while(!gvar.in.inst->Keyboard[sc_Escape] && gvar.player[0].enti.hp>0)
	{
		gvar.video.page[0].tlx=gvar.mv[0].tx*TILEWH;
		gvar.video.page[0].tly=gvar.mv[0].ty*TILEWH;
		shinku(&gvar);
		//top left corner & bottem right corner of map veiw be set as map edge trigger since maps are actually square
		//to stop scrolling and have the gvar.player position data move to the edge of the screen with respect to the direction
		//when gvar.player[0].tx or gvar.player[0].ty == 0 or gvar.player[0].tx == 20 or gvar.player[0].ty == 15 then stop because that is edge of map and you do not want to walk of the map

		//gvar.player movement
		IN_ReadControl(&gvar.player[0], &gvar);
		if(!panswitch){
			//ZC_walk2(gvar.player[0].ent, mv);
			//ZC_walk(&gvar.mv, &gvar.player, 0);
#ifdef OLDPLAYERSPRITESTUFF
			oldwalk(&gvar, 0, &ptmp);
#else
			ZC_walk(&gvar, 0);
#endif
		}else{
			TAIL_PANKEYFUNZC;
			//printf("	gvar.player[0].enti.q: %d", gvar.player[0].enti.q);	printf("	gvar.player[0].d: %d\n", gvar.player[0].d);
		}

		//the scripting stuff....
		//if(((gvar.player[0].enti.triggerx == TRIGGX && gvar.player[0].enti.triggery == TRIGGY) && gvar.in.inst->Keyboard[0x1C))||(gvar.player[0].enti.tx == 5 && gvar.player[0].enti.ty == 5))
		if(((gvar.mv[0].map->layerdata[0].data[(gvar.player[0].enti.triggerx-1)+(gvar.map.width*(gvar.player[0].enti.triggery-1))] == 0) && gvar.in.inst->Keyboard[0x1C])||(gvar.player[0].enti.tx == 5 && gvar.player[0].enti.ty == 5))
		{
			short i;
			for(i=800; i>=400; i--)
			{
				sound(i);
			}
			nosound();
		}
		if(gvar.player[0].enti.q == (TILEWH/(gvar.player[0].enti.spt))+1 && gvar.player[0].info.dir != 2 && (gvar.player[0].enti.triggerx == 5 && gvar.player[0].enti.triggery == 5)){ gvar.player[0].enti.hp--; }
		//debugging binds!
//		if(gvar.in.inst->Keyboard[24]){ VL_modexPalScramble(&gvar.video.palette); /*paloffset=0;*/ VL_LoadPalFileCore(&gvar.video.palette, &gvar); modexpdump(gvar.mv[0].page); IN_UserInput(1, &gvar); } //o
		if(gvar.in.inst->Keyboard[sc_O]){ VL_LoadPalFileCore(&gvar.video.palette, &gvar); modexpdump(gvar.video.sp, &gvar); IN_UserInput(1, &gvar); } //o
		if(gvar.in.inst->Keyboard[22]){ VL_modexPalScramble(&gvar.video.palette); VL_LoadPalFileCore(&gvar.video.palette, &gvar); } //u

		TAIL_FUNCTIONKEYFUNCTIONS
		TAIL_FUNCTIONKEYDRAWJUNK
		if(gvar.in.inst->Keyboard[sc_L]){ modexClearRegion(&gvar.video.page[0], gvar.player[0].enti.x, gvar.player[0].enti.y, 16, 16, 1); }

//===============================================================================
#if 0
		if(gvar.in.inst->Keyboard[sc_BackSlash]){
			VRS_ReadVRL(bakapee3, &gvar.player[0].enti, &gvar);
			DrawVRL(
				gvar.player[0].enti.x,
				gvar.player[0].enti.y,
				gvar.player[0].enti.spri.sprite_vrl_cont.vrl_header,
				gvar.player[0].enti.spri.sprite_vrl_cont.line_offsets,
				gvar.player[0].enti.spri.sprite_vrl_cont.buffer + sizeof(struct vrl1_vgax_header),
				gvar.player[0].enti.spri.sprite_vrl_cont.data_size
			);	while (!IN_CheckAck (&gvar)){}
			VRS_ReadVRS(bakapee, &gvar.player[0].enti, &gvar);
		IN_UserInput(1, &gvar); }
#endif
//===============================================================================

#ifdef OLDPLAYERSPRITESTUFF
		if(gvar.in.inst->Keyboard[sc_J])// || gvar.in.inst->Keyboard[sc_K])
		{
//			sprintf(global_temp_status_text2, "%Fp", &ptmp.data);
//			modexprint(&(gvar.video.page[0]), gvar.video.page[0].sw/2, gvar.video.page[0].sh/2, 1, 1, 3/*ccolor*/, 8, gvar.video.VL_Started, global_temp_status_text2);
			modexDrawSprite(&gvar.video.page[0], 16, 16, &ptmp);
		}
		if(gvar.in.inst->Keyboard[sc_K])
		{
			_fmemset(ptmp.data, 4, (ptmp.width)*ptmp.height);
		}
#else
		if(gvar.in.inst->Keyboard[sc_J])// || gvar.in.inst->Keyboard[sc_K])
		{
			if(spriteswitch)//gvar.in.inst->Keyboard[sc_J])
			{
				strcpy(bakapee, FILENAME_1);
				strcpy(bakapeep, FILENAME_1P);
				gvar.player[0].enti.overdraww=0;
			}
			else//if(gvar.in.inst->Keyboard[sc_K])
			{
				strcpy(bakapee, FILENAME_2);
				strcpy(bakapeep, FILENAME_2P);
				gvar.player[0].enti.overdraww=2;
			}
			spriteswitch=!spriteswitch;

			//read_vrs(&gvar, bakapee, gvar.player[0].enti.spri->spritesheet);
			VRS_ReadVRS(bakapee, &gvar.player[0].enti, &gvar);
			VL_LoadPalFile(bakapeep, &gvar.video.palette, &gvar);
			ZC_animatePlayer(&gvar.mv, &gvar.player, 0);
			IN_UserInput(1, &gvar);
		}//JK
#endif
//#ifdef FADE
//		if(gvar.in.inst->Keyboard[10]){ modexPalOverscan(rand()%56); modexPalUpdate(gvar.video.dpal); IN_UserInput(1, &gvar); }
//#endif
		if(gvar.in.inst->Keyboard[sc_R]){ VL_modexPalOverscan(&gvar.video.palette, rand()%32); } //r
		if(gvar.in.inst->Keyboard[sc_F]){ VL_SetPalette(&gamepal, &gvar.video); } //f


		//if((gvar.player[0].enti.q==1) && !(gvar.player[0].enti.x%TILEWH==0 && gvar.player[0].enti.y%TILEWH==0)) Quit (&gvar, "PLAYER OFF THE RAILS!");//break;	//incase things go out of sync!
		ZC_GirdChk(&gvar, &gvar.player[0]);
	}

	/* fade back to text mode */
	/* but 1st lets save the game palette~ */
#ifdef FADE
	modexPalSave(&gvar.video.palette);
	modexSavePalFile("data/g.pal", &gvar.video.palette);
	modexFadeOff(4, &gvar.video.palette);
#endif
		#ifdef SCROLLEXEDEBUG
	MM_DumpData (&gvar);
	HC_heapdump (&gvar);
		#endif
	Shutdown16(&gvar);
	printf("\nProject 16 xcroll.exe. This is just a test file!\n");
	printf("version %s\n", VERSION);
	SCROLLEXITMESG;
	WCPU_cpufpumesg();
#ifdef FADE
	modexFadeOn(4, gvar.video.dpal);
#endif
}











#ifdef OLDPLAYERSPRITESTUFF
void
modexDrawSprite(page_t *page, int x, int y, bitmap_t *bmp) {
    /* draw the whole sprite */
    modexDrawSpriteRegion(page, x, y, 0, 0, bmp->width, bmp->height, bmp);
//    	sprintf(global_temp_status_text2, "%Fp", &bmp->data);
//	modexprint(page, page->sw/2, (page->sh/2)+8, 1, 1, 3/*ccolor*/, 8, gvar->video.VL_Started, global_temp_status_text2);
}

void
modexDrawSpriteRegion(page_t *page, int x, int y,
		      int rx, int ry, int rw, int rh, bitmap_t *bmp) {
    word poffset = (word)page->data + y*(page->width/4) + x/4;
    byte *data = bmp->data;
    word bmpOffset = (word) data + ry * bmp->width + rx;
    word width = rw;
    word height = rh;
    byte plane = 1 << ((byte) x & 0x03);
    word scanCount = width/4 + (width%4 ? 1 :0);
    word nextPageRow = page->width/4 - scanCount;
    word nextBmpRow = (word) bmp->width - width;
    word rowCounter=0;
    byte planeCounter = 4;

//	sprintf(global_temp_status_text2, "%Fp", bmp->data);
//	modexprint(page, page->sw/2, (page->sh/2)+8, 1, 1, 3/*ccolor*/, 8, ggvv->video.VL_Started, global_temp_status_text2);
//	sprintf(global_temp_status_text2, "%Fp", data);
//	modexprint(page, page->sw/2, (page->sh/2)+16, 1, 1, 3/*ccolor*/, 8, ggvv->video.VL_Started, global_temp_status_text2);

    __asm {
		MOV AX, SCREEN_SEG      ; go to the VGA memory
		MOV ES, AX

		MOV DX, SC_INDEX	; point at the map mask register
		MOV AL, SC_MAPMASK	;
		OUT DX, AL		;

	PLANE_LOOP:
		MOV DX, SC_DATA		; select the current plane
		MOV AL, plane		;
		OUT DX, AL		;

		;-- begin plane painting
		MOV AX, height		; start the row counter
		MOV rowCounter, AX	;
		MOV DI, poffset		; go to the first pixel
		MOV SI, bmpOffset	; go to the bmp pixel
	ROW_LOOP:
		MOV CX, width		; count the columns
	SCAN_LOOP:
		LODSB
		DEC SI
		CMP AL, 0
		JNE DRAW_PIXEL		; draw non-zero pixels

		INC DI			; skip the transparent pixel
		ADD SI, 1
		JMP NEXT_PIXEL
	DRAW_PIXEL:
		MOVSB			; copy the pixel
	NEXT_PIXEL:
		SUB CX, 3		; we skip the next 3
		ADD SI, 3		; skip the bmp pixels
		LOOP SCAN_LOOP		; finish the scan

		MOV AX, nextPageRow
		ADD DI, AX		; go to the next row on screen
		MOV AX, nextBmpRow
		ADD SI, AX		; go to the next row on bmp

		DEC rowCounter
		JNZ ROW_LOOP		; do all the rows
		;-- end plane painting

		MOV AL, plane		; advance to the next plane
		SHL AL, 1		;
		AND AL, 0x0f		; mask the plane properly
		MOV plane, AL		; store the plane

		INC bmpOffset		; start bmp at the right spot

		DEC planeCounter
		JNZ PLANE_LOOP		; do all 4 planes
    }
}

void oldwalk(global_game_variables_t *gvar, word pn, bitmap_t *pbmp)
{
	switch(gvar->player[pn].enti.d)
	{
		//no direction
		case 2:
			//0000gvar->mv[0].video->startclk = (*clockw);
		break;
		//right movement
		case 3:
			if(ZCL_ScreenMidPosChk(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty) &&
			ZCL_CollCheck(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty))//!(gvar->player[pn].enti.tx+1 == TRIGGX && gvar->player[pn].enti.ty == TRIGGY))	//collision detection!
			{
				gvar->player[pn].walktype=2;
				if(gvar->player[pn].enti.q<=gvar->player[pn].enti.spt)
				{
					INC_PER_FRAME;
					animatePlayer(gvar, 1, 1, gvar->player[pn].enti.x, gvar->player[pn].enti.y, persist_aniframe, gvar->player[pn].enti.q, pbmp);
					ZC_mapScroll(gvar->mv, gvar->player, pn);
					gvar->player[pn].enti.q++;
					//0000gvar->mv[0].video->clk = ((*clockw)-gvar->mv[0].video->startclk)/18.2;
					modexShowPage(&gvar->video.page[0]);
				} else { gvar->player[pn].enti.q = 1; gvar->player[pn].enti.d = 2; gvar->player[pn].enti.tx++; }
			}
			else if(ZCL_mapEdgeChk(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty, 0, 1) &&
			ZCL_CollCheck(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty))//!(gvar->player[pn].enti.tx+1 == TRIGGX && gvar->player[pn].enti.ty == TRIGGY))
			{
				gvar->player[pn].walktype=1;
				if(gvar->player[pn].enti.q<=gvar->player[pn].enti.spt)
				{
					INC_PER_FRAME;
					gvar->player[pn].enti.x+=(gvar->player[pn].enti.spt);
					animatePlayer(gvar, 1, 0, gvar->player[pn].enti.x, gvar->player[pn].enti.y, persist_aniframe, gvar->player[pn].enti.q, pbmp);
					gvar->player[pn].enti.q++;
					modexShowPage(&gvar->video.page[0]);
				} else { gvar->player[pn].enti.q = 1; gvar->player[pn].enti.d = 2; gvar->player[pn].enti.tx++; }
			}
			else
			{
				gvar->player[pn].walktype=0;
				//ZC_animatePlayer(gvar->mv, gvar->player, pn);
				modexShowPage(&gvar->video.page[0]);
				gvar->player[pn].enti.d = 2;
			}
			gvar->player[pn].enti.triggerx = gvar->player[pn].enti.tx+1;
			gvar->player[pn].enti.triggery = gvar->player[pn].enti.ty;
		break;
		//left movement
		case 1:
			if(ZCL_ScreenMidPosChk(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty) &&
			ZCL_CollCheck(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty))//!(gvar->player[pn].enti.tx-1 == TRIGGX && gvar->player[pn].enti.ty == TRIGGY))	//collision detection!
			{
				gvar->player[pn].walktype=2;
				if(gvar->player[pn].enti.q<=gvar->player[pn].enti.spt)
				{
					INC_PER_FRAME;
					animatePlayer(gvar, 1, 1, gvar->player[pn].enti.x, gvar->player[pn].enti.y, persist_aniframe, gvar->player[pn].enti.q, pbmp);
					ZC_mapScroll(gvar->mv, gvar->player, pn);
					gvar->player[pn].enti.q++;
					//0000gvar->mv[0].video->clk = ((*clockw)-gvar->mv[0].video->startclk)/18.2;
					modexShowPage(&gvar->video.page[0]);
				} else { gvar->player[pn].enti.q = 1; gvar->player[pn].enti.d = 2; gvar->player[pn].enti.tx--; }
			}
			else if(ZCL_mapEdgeChk(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty, 0, 1) &&
			ZCL_CollCheck(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty))//!(gvar->player[pn].enti.tx-1 == TRIGGX && gvar->player[pn].enti.ty == TRIGGY))
			{
				gvar->player[pn].walktype=1;
				if(gvar->player[pn].enti.q<=gvar->player[pn].enti.spt)
				{
					INC_PER_FRAME;
					gvar->player[pn].enti.x-=(gvar->player[pn].enti.spt);
					animatePlayer(gvar, 1, 0, gvar->player[pn].enti.x, gvar->player[pn].enti.y, persist_aniframe, gvar->player[pn].enti.q, pbmp);
					gvar->player[pn].enti.q++;
					modexShowPage(&gvar->video.page[0]);
				} else { gvar->player[pn].enti.q = 1; gvar->player[pn].enti.d = 2; gvar->player[pn].enti.tx--; }
			}
			else
			{
				gvar->player[pn].walktype=0;
				//ZC_animatePlayer(gvar->mv, gvar->player, pn);
				modexShowPage(&gvar->video.page[0]);
				gvar->player[pn].enti.d = 2;
			}
			gvar->player[pn].enti.triggerx = gvar->player[pn].enti.tx-1;
			gvar->player[pn].enti.triggery = gvar->player[pn].enti.ty;
		break;
		//down movement
		case 4:
			if(ZCL_ScreenMidPosChk(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty) &&
			ZCL_CollCheck(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty))//!(gvar->player[pn].enti.tx == TRIGGX && gvar->player[pn].enti.ty+1 == TRIGGY))	//collision detection!
			{
				gvar->player[pn].walktype=2;
				if(gvar->player[pn].enti.q<=gvar->player[pn].enti.spt)
				{
					INC_PER_FRAME;
					animatePlayer(gvar, 1, 1, gvar->player[pn].enti.x, gvar->player[pn].enti.y, persist_aniframe, gvar->player[pn].enti.q, pbmp);
					ZC_mapScroll(gvar->mv, gvar->player, pn);
					gvar->player[pn].enti.q++;
					//0000gvar->mv[0].video->clk = ((*clockw)-gvar->mv[0].video->startclk)/18.2;
					modexShowPage(&gvar->video.page[0]);
				} else { gvar->player[pn].enti.q = 1; gvar->player[pn].enti.d = 2; gvar->player[pn].enti.ty++; }
			}
			else if(ZCL_mapEdgeChk(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty, 0, 1) &&
			ZCL_CollCheck(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty))//!(gvar->player[pn].enti.tx == TRIGGX && gvar->player[pn].enti.ty+1 == TRIGGY))
			{
				gvar->player[pn].walktype=1;
				if(gvar->player[pn].enti.q<=gvar->player[pn].enti.spt)
				{
					INC_PER_FRAME;
					gvar->player[pn].enti.y+=(gvar->player[pn].enti.spt);
					animatePlayer(gvar, 1, 0, gvar->player[pn].enti.x, gvar->player[pn].enti.y, persist_aniframe, gvar->player[pn].enti.q, pbmp);
					gvar->player[pn].enti.q++;
					modexShowPage(&gvar->video.page[0]);
				} else { gvar->player[pn].enti.q = 1; gvar->player[pn].enti.d = 2; gvar->player[pn].enti.ty++; }
			}
			else
			{
				gvar->player[pn].walktype=0;
				//ZC_animatePlayer(gvar->mv, gvar->player, pn);
				modexShowPage(&gvar->video.page[0]);
				gvar->player[pn].enti.d = 2;
			}
			gvar->player[pn].enti.triggerx = gvar->player[pn].enti.tx;
			gvar->player[pn].enti.triggery = gvar->player[pn].enti.ty+1;
		break;
		//up movement
		case 0:
			if(ZCL_ScreenMidPosChk(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty) &&
			ZCL_CollCheck(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty))//!(gvar->player[pn].enti.tx == TRIGGX && gvar->player[pn].enti.ty-1 == TRIGGY))	//collision detection!
			{
				gvar->player[pn].walktype=2;
				if(gvar->player[pn].enti.q<=gvar->player[pn].enti.spt)
				{
					INC_PER_FRAME;
					animatePlayer(gvar, 1, 1, gvar->player[pn].enti.x, gvar->player[pn].enti.y, persist_aniframe, gvar->player[pn].enti.q, pbmp);
					ZC_mapScroll(gvar->mv, gvar->player, pn);
					gvar->player[pn].enti.q++;
					//0000gvar->mv[0].video->clk = ((*clockw)-gvar->mv[0].video->startclk)/18.2;
					modexShowPage(&gvar->video.page[0]);
				} else { gvar->player[pn].enti.q = 1; gvar->player[pn].enti.d = 2; gvar->player[pn].enti.ty--; }
			}
			else if(ZCL_mapEdgeChk(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty, 0, 1) &&
			ZCL_CollCheck(gvar->mv, gvar->player[pn].enti.d, gvar->player[pn].enti.tx, gvar->player[pn].enti.ty))//!(gvar->player[pn].enti.tx == TRIGGX &&  gvar->player[pn].enti.ty-1 == TRIGGY))
			{
				gvar->player[pn].walktype=1;
				if(gvar->player[pn].enti.q<=gvar->player[pn].enti.spt)
				{
					INC_PER_FRAME;
					gvar->player[pn].enti.y-=(gvar->player[pn].enti.spt);
					animatePlayer(gvar, 1, 0, gvar->player[pn].enti.x, gvar->player[pn].enti.y, persist_aniframe, gvar->player[pn].enti.q, pbmp);
					gvar->player[pn].enti.q++;
					modexShowPage(&gvar->video.page[0]);
				} else { gvar->player[pn].enti.q = 1; gvar->player[pn].enti.d = 2; gvar->player[pn].enti.ty--; }
			}
			else
			{
				gvar->player[pn].walktype=0;
				//ZC_animatePlayer(gvar->mv, gvar->player, pn);
				modexShowPage(&gvar->video.page[0]);
				gvar->player[pn].enti.d = 2;
			}
			gvar->player[pn].enti.triggerx = gvar->player[pn].enti.tx;
			gvar->player[pn].enti.triggery = gvar->player[pn].enti.ty-1;
		break;
	}
}

void animatePlayer(global_game_variables_t *gvar, /*map_view_t *top, */short d1, short d2, int x, int y, int ls, int lp, bitmap_t *bmp)
{
	short dire=32*d1; //direction
	short qq; //scroll offset

	if(d2==0) qq = 0;
	else qq = ((lp)*gvar->player[0].enti.spt);
	switch (d1)
	{
		case 0:
			//up
			x=x-4;
			y=y-qq-TILEWH;
		break;
		case 1:
			// right
			x=x+qq-4;
			y=y-TILEWH;
		break;
		case 2:
			//down
			x=x-4;
			y=y+qq-TILEWH;
		break;
		case 3:
			//left
			x=x-qq-4;
			y=y-TILEWH;
		break;
	}
	modexCopyPageRegion(gvar->mv[0].page, gvar->mv[1].page, x-4, y-4, x-4, y-4, 28, 40);
	if(2>ls && ls>=1) { modexDrawSpriteRegion(gvar->mv[0].page, x, y, 48, dire, 24, 32, bmp); }else
	if(3>ls && ls>=2) { modexDrawSpriteRegion(gvar->mv[0].page, x, y, 24, dire, 24, 32, bmp); }else
	if(4>ls && ls>=3) { modexDrawSpriteRegion(gvar->mv[0].page, x, y, 0, dire, 24, 32, bmp); }else
	if(5>ls && ls>=4) { modexDrawSpriteRegion(gvar->mv[0].page, x, y, 24, dire, 24, 32, bmp); }
	//TODO: mask copy //modexCopyPageRegion(gvar->mv[0].page, gvar->mv[0].page, x-4, y-4, x-4, y-4, 28, 40);
	//modexClearRegion(top->page, 66, 66, 2, 40, 0);
	//modexCopyPageRegion(gvar->mv[0].page, top->page, 66, 66, 66, 66, 2, 40);
	//turn this off if XT
	//if(detectcpu() > 0) modexWaitBorder();
	switch(gvar->kurokku.fpscap)
	{
		case 0: //turn this off if XT
			//modexprint(&(gv->video.page[0]), x, y+8, type, 1, col, bgcol, "sanic!");
			gvar->kurokku.frames_per_second=1;
		break;
		case 1:
			modexWaitBorder();
			gvar->kurokku.frames_per_second=FPSVALUE;
		break;
	}
	PM_NextFrame(gvar);
}
#endif
