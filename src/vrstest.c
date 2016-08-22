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

#include <stdio.h>
#include <dos.h>
#include <string.h>
#include "src/lib/modex16.h"
#include "src/lib/16_sprite.h"
#include "src/lib/16_ca.h"
#include "src/lib/16_mm.h"

global_game_variables_t gvar;

void main() {
	mminfo_t mm; mminfotype mmi;
	__segment sega;
	void __based(sega)* bigbuffer;
	int i;
	word start;
	int plane;
	float t1, t2;
	boolean baka;
	byte *pal;
	int fd, size, size1;
	struct sprite spri;
	vrl1_vgax_offset_t * off, *off1;
	struct vrs_container vrs;
	vrl1_vgax_offset_t **vrl_line_offsets;
	uint32_t huge *vrl_headers_offsets;
	uint16_t huge *vrl_id_iter;
	uint32_t vrl_size;
	int num_of_vrl;
	struct vrl1_vgax_header huge *curr_vrl;
	struct vrl_container *vrl;
	int success;



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

	//bmp = bitmapLoadPcx("data/chikyuu.pcx");
	gvar.video.page[0]=modexDefaultPage(&gvar.video.page[0]);

	mm.mmstarted=0;
	MM_Startup(&mm, &mmi);
	CA_Startup(&gvar);
	if(CA_LoadFile("data/spri/chikyuu.vrs", &bigbuffer, &mm, &mmi)) baka=1; else baka=0;
	fd = open("data/spri/chikyuu.vrs", O_RDONLY|O_BINARY);
	size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	close(fd);
	// Insert sanity cheks later
	vrs.buffer = bigbuffer;
	vrs.data_size = size - sizeof(struct vrl1_vgax_header);
	num_of_vrl = 0;
	vrl_id_iter = (uint16_t huge *)(vrs.buffer + vrs.vrs_hdr->offset_table[VRS_HEADER_OFFSET_SPRITE_ID_LIST]);
	while(vrl_id_iter[num_of_vrl]){
		num_of_vrl++;
	}
	
	// Allocate memory for vrl line offsets table
	vrl_line_offsets = malloc(sizeof(vrl1_vgax_offset_t *)*num_of_vrl);

	vrl_headers_offsets = (uint32_t huge *)(vrs.buffer + vrs.vrs_hdr->offset_table[VRS_HEADER_OFFSET_VRS_LIST]);
	// Calculate line offsets for each vrl
	for(i = 0; i < num_of_vrl; i++){
		curr_vrl = (struct vrl1_vgax_header huge *)(vrs.buffer + vrl_headers_offsets[i]);

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

	//read_vrs(&gvar, "data/spri/chikyuu.vrs", &vrs);
	spri.spritesheet = &vrs;
	spri.sprite_vrl_cont = malloc(sizeof(struct vrl_container));

	vrl = malloc(sizeof(struct vrl_container));
	i = get_vrl_by_id(&vrs, 10, vrl);
	if(i == -2)
	{
		puts("Die");
		return;
	}
	off = vrl1_vgax_genlineoffsets(vrl->buffer, vrl->buffer + sizeof(struct vrl1_vgax_header), vrl->data_size);
	fd = open("data/spri/chikyuu.vrl", O_RDONLY|O_BINARY);
	size = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	close(fd);

	if(CA_LoadFile("data/spri/chikyuu.vrl", &bigbuffer, &mm, &mmi)) baka=1; else baka=0;
	off1 = vrl1_vgax_genlineoffsets(bigbuffer, (byte *)bigbuffer + sizeof(struct vrl1_vgax_header), size - sizeof(struct vrl1_vgax_header));


	//read_vrs(&gvar, "data/spri/chikyuu.vrs", spri.spritesheet);
	i = set_anim_by_id(&spri, 10);
	if (i == -1)
	{
		return;
	}
	spri.x = spri.y = 70;


	/* fix up the palette and everything */
	//modexPalUpdate1(bmp.palette);
	//modexLoadPalFile("data/spri/chikyuu.pal", &pal);
	//modexPalUpdate1(pal);

	/* clear and draw one sprite and one bitmap */
	VGAmodeX(1, 1, &gvar);
	modexClearRegion(&gvar.video.page[0], 0, 0, gvar.video.page[0].sw, gvar.video.page[0].sh, 1);

	/* non sprite comparison */
	start = *clockw;
	//for(i=0; i<100 ;i++) {
	//	oldDrawBmp(VGA, 20, 20, &bmp, 0);
	//}

	start = *clockw;
	//for(i=0; i<100 ;i++) {
//0000		modexDrawBmp(&gvar.video.page[0], 20, 20, &bmp);
	//	modexDrawBmp(&gvar.video.page[0], 160, 120, &bmp);
	//}
	t1 = (*clockw-start) /18.2;

	start = *clockw;
	//for(i=0; i<100; i++) {
//0000		modexCopyPageRegion(&gvar.video.page[0], &gvar.video.page[0], 20, 20, 128, 20, 64, 64);
		modexCopyPageRegion(&gvar.video.page[0], &gvar.video.page[0], 0, 0, 0, 0, 320, 240);
		animate_spri(&spri);
	draw_vrl1_vgax_modex(5,5,vrl->buffer,vrl->line_offsets,vrl->buffer + sizeof(struct vrl1_vgax_header),vrl->data_size);
	draw_vrl1_vgax_modex(40,40,vrs.buffer + vrl_headers_offsets[0],vrs.vrl_line_offsets[0],vrs.buffer + vrl_headers_offsets[0] + sizeof(struct vrl1_vgax_header),vrl_headers_offsets[1] - vrl_headers_offsets[0] - sizeof(struct vrl1_vgax_header));
	draw_vrl1_vgax_modex(100, 5, bigbuffer, off1, (byte *)bigbuffer + sizeof(struct vrl1_vgax_header), size - sizeof(struct vrl1_vgax_header));

	//}
	t2 = (*clockw-start)/18.2;

	//for (i = 0; i < 5; i++){
	//animate_spri(&spri);
	
	start = *clockw;
	//for(i=0; i<100 ;i++) {
	//	oldDrawBmp(VGA, 20, 20, &bmp, 1);
	//}


	start = *clockw;
	//for(i=0; i<100 ;i++) {
//0000		modexDrawSprite(&gvar.video.page[0], 20, 20, &bmp);
	//	modexDrawSprite(&gvar.video.page[0], 160, 120, &bmp);
	//}
	//_fmemset(MK_FP(0xA000, 0), (int)p->plane, gvar.video.page[0].sw*(gvar.video.page[0].sh*2));
	//modexDrawBmp(&gvar.video.page[0], 0, 0, &bmp);
	while(!kbhit())
	{
		//DrawPBuf(&gvar.video.page[0], 0, 0, p, 0);
	}
	VGAmodeX(0, 1, &gvar);
	free(spri.sprite_vrl_cont);
	MM_FreePtr(&bigbuffer, &mm);
	//MM_FreePtr(&((void __based(sega)*)spri.spritesheet->buffer), &mm);
	MM_Shutdown(&mm);
	CA_Shutdown(&gvar);
	/*printf("\nmain=%Fp\n\n", &i);
	printf("bmp.data=%Fp\n", bmp.data);
	printf("*bmp.data=%Fp\n", *(bmp.data));
	printf("&bmp.data=%Fp\n", &(bmp.data));

	printf("\n%d\n", sizeof(p->plane));
	printf("%d\n", sizeof(bmp));*/

	/*for(i=0; i<(320*240); i++)
	{
		fprintf(stdout, "%d", bmp.data[i]);
		if(i%bmp.width==0) fprintf(stdout, "\n");
	}*/
	printf("CPU to VGA: %f\n", t1);
	printf("VGA to VGA: %f\n", t2);
	printf("gvar.video.page[0].width: %u\n", gvar.video.page[0].width);
	printf("gvar.video.page[0].height: %u\n", gvar.video.page[0].height);
	printf("Num %d", num_of_vrl);
	if(baka) printf("\nyay!\n");
	else printf("\npoo!\n");
}
