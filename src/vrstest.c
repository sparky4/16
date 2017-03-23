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

#include "src/lib/16_head.h"
#include "src/lib/16_tail.h"
#include "src/lib/16_vl.h"
#include "src/lib/16_spri.h"
#include "src/lib/16_pm.h"
#include "src/lib/16_ca.h"
#include "src/lib/16_mm.h"

extern boolean dbg_nointest;
boolean panswitch;

static word far* clockw= (word far*) 0x046C; /* 18.2hz clock */

void main()
{
	static global_game_variables_t gvar;
	//__segment sega;
	memptr bigbuffer;
	int i;
	word start,w;
	float t1, t2;
	//boolean baka;
	entity_t enti;
	//vrl1_vgax_offset_t * off, *off1;
/*	struct vrs_container vrs; vrl1_vgax_offset_t **vrl_line_offsets;
	uint32_t far *vrl_headers_offsets;
	uint16_t far *vrl_id_iter;
	uint32_t vrl_size;
	int num_of_vrl;
	struct vrl1_vgax_header far *curr_vrl;
	int size=0;
*/

	dbg_nointest=0;
	Startup16(&gvar);

/*	CA_LoadFile("data/spri/chikyuu.vrs", &bigbuffer, &gvar);

	// Insert sanity cheks later
	vrs.buffer = bigbuffer;
	vrs.data_size = size - sizeof(struct vrl1_vgax_header);
	num_of_vrl = 0;
	vrl_id_iter = (uint16_t far *)(vrs.buffer + vrs.vrs_hdr->offset_table[VRS_HEADER_OFFSET_SPRITE_ID_LIST]);
	while(vrl_id_iter[num_of_vrl]){
		num_of_vrl++;
	}

	// Allocate memory for vrl line offsets table
	vrl_line_offsets = malloc(sizeof(vrl1_vgax_offset_t *)*num_of_vrl);

	vrl_headers_offsets = (uint32_t far *)(vrs.buffer + vrs.vrs_hdr->offset_table[VRS_HEADER_OFFSET_VRS_LIST]);
	// Calculate line offsets for each vrl
	for(i = 0; i < num_of_vrl; i++){
		curr_vrl = (struct vrl1_vgax_header far *)(vrs.buffer + vrl_headers_offsets[i]);

		// Calc. vrl size as (next_offset - curr_offset)
		if (i != num_of_vrl - 1){
			vrl_size = vrl_headers_offsets[i+1] - vrl_headers_offsets[i] - sizeof(struct vrl1_vgax_header);
		}
		// If it's the last vrl, size is (next_vrs_struct_offset - curr_offset)
		else{
			vrl_size = vrs.vrs_hdr->offset_table[VRS_HEADER_OFFSET_SPRITE_ID_LIST] - vrl_headers_offsets[i] - sizeof(struct vrl1_vgax_header);
		}
		vrl_line_offsets[i] = vrl1_vgax_genlineoffsets(curr_vrl, (byte *)curr_vrl + sizeof(struct vrl1_vgax_header), vrl_size);
	}
	vrs.vrl_line_offsets = vrl_line_offsets;


//	read_vrs(&gvar, "data/spri/chikyuu.vrs", &vrs);


	enti.spri.spritesheet = &vrs;
	enti.spri.sprite_vrl_cont = malloc(sizeof(struct vrl_container));*/
	VRS_LoadVRS("data/spri/chikyuu.vrs", &enti, &gvar);
	i = set_anim_by_id(&(enti.spri), 31);
	if (i == -1)
	{
		//Quit(&gvar, "ERROR!! glitch IN FILE!\n");
	}
	enti.spri.x = TILEWH-4;
	enti.spri.y = TILEWH;

//	Uncomment to see broken sprites
/*	sega = mm.bufferseg;
	if(CA_LoadFile("data/spri/CHUBACW1.vrl", &bigbuffer, &mm, &mmi)) baka=1; else baka=0;*/

	VGAmodeX(1, 1, &gvar);
	modexHiganbanaPageSetup(&gvar.video);
	VL_ShowPage(&gvar.video.page[0], 1, 0);

	/* non sprite comparison */
	start = *clockw;
	//modexCopyPageRegion(&gvar.video.page[0], &gvar.video.page[0], 0, 0, 0, 0, 320, 240);
	t1 = (*clockw-start) /18.2;

	start = *clockw;

	t2 = (*clockw-start)/18.2;

	/*modexLoadPalFile("data/spri/chikyuu.pal", &pal);
	modexPalUpdate(pal);*/
	//modexClearRegion(&gvar.video.page[0], 0, 0, gvar.video.page[0].width, gvar.video.page[0].height, 2);
	VL_LoadPalFile("data/spri/chikyuu.pal", &gvar.video.palette);
	for (i = 0; i < 10; i++){
		enti.spri.delay = 1;

		if(i==5) set_anim_by_id(&enti.spri, 21);

		animate_spri(&enti, &gvar.video);// enti.spri.x += 16;
		delay(500);
	}

	while(!IN_KeyDown(sc_Escape))
	{
		switch(w)
		{
			case 768:
//				modexPalUpdate0(pal);
				w=0;
			default:
				w++;
//				gvar.video.palette[w]=rand();
			break;
		}
		TAIL_FUNCTIONKEYDRAWJUNKNOMV
		if(IN_KeyDown(sc_F7)){ VL_ShowPage(&gvar.video.page[0], 1, 1);							IN_UserInput(1, &gvar); }
		if(IN_KeyDown(sc_F6)){ VL_ShowPage(&gvar.video.page[0], 1, 0);							IN_UserInput(1, &gvar); }
		if(IN_KeyDown(25)){ modexpdump(&gvar.video.page[0]);  IN_UserInput(1, &gvar); } //p
	}
	VGAmodeX(0, 1, &gvar);
	MM_ShowMemory(&gvar);
	MM_DumpData(&gvar);
	MM_Report_(&gvar);
	Shutdown16(&gvar);
	MM_FreePtr(&bigbuffer, &gvar);
	//MM_FreePtr(&((void __based(sega)*)enti.spri.spritesheet->buffer), &mm);
	//printf("CPU to VGA: %f\n", t1);
	//printf("VGA to VGA: %f\n", t2);
	//HC_heapdump(&gvar);
	printf("Project 16 vrstest.exe. This is just a test file!\n");
	printf("version %s\n", VERSION);
	printf("t1: %f\n", t1);
	printf("t2: %f\n", t2);
//0000	printf("gvar.video.page[0].width: %u\n", gvar.video.page[0].width);
//0000	printf("gvar.video.page[0].height: %u\n", gvar.video.page[0].height);
//	printf("Num %d", num_of_vrl);
//	if(baka) printf("\nyay!\n");
//	else printf("\npoo!\n");
}
