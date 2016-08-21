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
	VGAmodeX(1, 1, &gvar);
	gvar.video.page[0]=modexDefaultPage(&gvar.video.page[0]);

	mm.mmstarted=0;
	MM_Startup(&mm, &mmi);
	CA_Startup(&gvar);
	if(CA_LoadFile("data/spri/chikyuu.vrs", &bigbuffer, &mm, &mmi)) baka=1; else baka=0;

	/* fix up the palette and everything */
	//modexPalUpdate1(bmp.palette);
	//modexLoadPalFile("data/spri/chikyuu.pal", &pal);
	//modexPalUpdate1(pal);

	/* clear and draw one sprite and one bitmap */
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
	//}
	t2 = (*clockw-start)/18.2;


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
	MM_FreePtr(&bigbuffer, &mm);
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
	if(baka) printf("\nyay!\n");
	else printf("\npoo!\n");
}
