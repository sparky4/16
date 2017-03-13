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

#include <hw/cpu/cpu.h>
#include <hw/dos/dos.h>
#include <hw/vga/vga.h>
#include <hw/vga/vrl.h>

#include "src/tesuto.h"

//#define FILENAME_1 "data/aconita.vrl"
//#define FILENAME_2 "data/aconita.pal"
#define FILENAME_1 "data/spri/chikyuu.vrl"
#define FILENAME_2 "data/spri/chikyuu.pal"

#define INITTNUM 1

map_view_t mv[4];
map_t map;//junk var

int main(int argc,char **argv)
{
	static global_game_variables_t gvar;
	struct vrl1_vgax_header *vrl_header;
	vrl1_vgax_offset_t *vrl_lineoffs;
	unsigned char *buffer;
	unsigned int bufsz;
	int fd;
	//nibble i;
	char *bakapee1,*bakapee2;

	boolean anim=1,noanim=0,zerostoppause=1;

	bakapee1=malloc(64);
	bakapee2=malloc(64);

	if (argc < 2) {
		//fprintf(stderr,"drawvrl <VRL file> <palette file>\n palette file optional\n");
		bakapee1 = FILENAME_1;//"data/aconita.vrl";
		bakapee2 = FILENAME_2;//"data/aconita.pal";

	}else{
		if(argv[1]) bakapee1 = argv[1];
		if(argv[2]) bakapee2 = argv[2];
	}

	fd = open(bakapee1,O_RDONLY|O_BINARY);
	if (fd < 0) {
		fprintf(stderr,"Unable to open '%s'\n", bakapee1);
		return 1;
	}
	{
		unsigned long sz = lseek(fd,0,SEEK_END);
		if (sz < sizeof(*vrl_header)) return 1;
		if (sz >= 65535UL) return 1;

		bufsz = (unsigned int)sz;
		buffer = malloc(bufsz);
		if (buffer == NULL) return 1;

		lseek(fd,0,SEEK_SET);
		if ((unsigned int)read(fd,buffer,bufsz) < bufsz) return 1;

		vrl_header = (struct vrl1_vgax_header*)buffer;
		if (memcmp(vrl_header->vrl_sig,"VRL1",4) || memcmp(vrl_header->fmt_sig,"VGAX",4)) return 1;
		if (vrl_header->width == 0 || vrl_header->height == 0) return 1;
	}
	close(fd);

	probe_dos();
	if (!probe_vga()) {
		printf("VGA probe failed\n");
		return 1;
	}
	VGAmodeX(1, 1, &gvar);
	modexPalUpdate0(gvar.video.palette);

	/* load color palette */
	VL_LoadPalFile(bakapee2, &gvar.video.palette);

	/* preprocess the sprite to generate line offsets */
	vrl_lineoffs = vrl1_vgax_genlineoffsets(vrl_header,buffer+sizeof(*vrl_header),bufsz-sizeof(*vrl_header));
	if (vrl_lineoffs == NULL) return 1;

	IN_Startup();
	IN_Default(0,&gvar.player[0],ctrl_Keyboard1);
	EN_initPlayer(&gvar.player[0], &gvar.video);

	/* setup camera and screen~ */
	modexHiganbanaPageSetup(&gvar.video);
	ZC_MVSetup(&MVVAR, &map, &gvar);
	ZC_ShowMV(&MVVAR, gvar.video.sp, 0);

//	VL_PatternDraw(video_t *video, word pn, boolean sw, boolean allsw);
	VL_PatternDraw(&gvar.video, 0/*gvar.video.sp*/, 0, 1);
	TESTBG34
	DRAWCORNERBOXES

	/* make distinctive pattern offscreen, render sprite, copy onscreen.
	 * this time, we render the distinctive pattern to another offscreen location and just copy.
	 * note this version is much faster too! */
	{
		unsigned int i,o,o2;
		int x,y,rx,ry,w,h;
		unsigned int overdraw = 1;	// how many pixels to "overdraw" so that moving sprites with edge pixels don't leave streaks.
						// if the sprite's edge pixels are clear anyway, you can set this to 0.
		VGA_RAM_PTR omemptr;
		int xdir=1,ydir=1;

//		VL_PatternDraw(video_t *video, word pn, boolean sw, boolean allsw);
//		VL_PatternDraw(&gvar.video, 0, 1, 1);
//		TESTBG;
//		DRAWCORNERBOXES;

		/* starting coords. note: this technique is limited to x coordinates of multiple of 4 */
		x = -(gvar.video.page[0].dx);
		y = -(gvar.video.page[0].dy);

		/* do it */
		omemptr = vga_state.vga_graphics_ram; // save original mem ptr

		while(!IN_KeyDown(sc_Escape))
		{
			IN_ReadControl(&gvar.player[0]);
			if(IN_KeyDown(68)){ gvar.kurokku.fpscap=!gvar.kurokku.fpscap; IN_UserInput(1); } //f10
			TAIL_PANKEYFUN;
			if(IN_KeyDown(sc_Space) || zerostoppause)	//space
			{
				anim=!anim;
				DRAWCORNERBOXES;
				if(!zerostoppause) IN_UserInput(1); else zerostoppause=0;
			}
			if(IN_KeyDown(sc_R)){
				gvar.video.page[0].dx=gvar.video.page[0].dy=gvar.video.page[1].dx=gvar.video.page[1].dy=16;
				MVVAR[0].tx = MVVAR[0].ty = MVVAR[1].tx = MVVAR[1].ty = INITTNUM;
				VL_ShowPage(&gvar.video.page[gvar.video.sp], 1, 0);
				gvar.player[0].enti.q = 1; gvar.player[0].enti.d = 2;
				x=y=0;
				xdir=ydir=1;
			} //R
			TAIL_FUNCTIONKEYFUNCTIONS0EXE;

			if(anim && !noanim)
			{
				/* render box bounds. y does not need modification, but x and width must be multiple of 4 */
				if (x >= overdraw) rx = (x - overdraw) & (~3);
				else rx = -(gvar.video.page[0].dx);
				if (y >= overdraw) ry = (y - overdraw);
				else ry = -(gvar.video.page[0].dy);
				h = vrl_header->height + overdraw + y - ry;
				w = (x + vrl_header->width + (overdraw*2) + 3/*round up*/ - rx) & (~3);
				if ((rx+w) > gvar.video.page[0].width) w = gvar.video.page[0].width-rx;
				if ((ry+h) > gvar.video.page[0].height) h = gvar.video.page[0].height-ry;

				/* block copy pattern to where we will draw the sprite */
				vga_setup_wm1_block_copy();
				o2 = gvar.video.page[0].pagesize+gvar.video.page[1].pagesize;
				o = ((uint16_t)gvar.video.page[2].data) + (ry * gvar.video.page[0].stridew) + (rx >> 2); // source offscreen		0x10000UL -
				for (i=0;i < h;i++,o += gvar.video.page[0].stridew,o2 += (w >> 2)) vga_wm1_mem_block_copy(o2,o,w >> 2);
				/* must restore Write Mode 0/Read Mode 0 for this code to continue drawing normally */
				vga_restore_rm0wm0();

				/* replace VGA stride with our own and mem ptr. then sprite rendering at this stage is just (0,0) */
				vga_state.vga_draw_stride_limit = (gvar.video.page[0].width + 3/*round up*/ - x) >> 2;
				vga_state.vga_draw_stride = w >> 2;
				vga_state.vga_graphics_ram = omemptr + gvar.video.page[0].pagesize + gvar.video.page[1].pagesize;

				/* then the sprite. note modding ram ptr means we just draw to (x&3,0) */
draw_vrl1_vgax_modex(x-rx,y-ry,vrl_header,vrl_lineoffs,buffer+sizeof(*vrl_header),bufsz-sizeof(*vrl_header));
//printf("x=%d	y=%d	rx=%d	ry=%d\n", x, y, rx, ry);

				/* restore ptr */
				vga_state.vga_graphics_ram = omemptr;

				/* block copy to visible RAM from offscreen */
				vga_setup_wm1_block_copy();
				o = gvar.video.page[0].pagesize+gvar.video.page[1].pagesize; // source offscreen
				o2 = (ry * gvar.video.page[0].stridew) + (rx >> 2); // dest visible (original stride)
				for (i=0;i < h;i++,o += vga_state.vga_draw_stride,o2 += gvar.video.page[0].stridew) vga_wm1_mem_block_copy(o2,o,w >> 2);
				/* must restore Write Mode 0/Read Mode 0 for this code to continue drawing normally */
				vga_restore_rm0wm0();

				/* restore stride */
				vga_state.vga_draw_stride_limit = vga_state.vga_draw_stride = gvar.video.page[0].stridew;

				/* step */
				x += xdir; y += ydir;
				if ((x + vrl_header->width) >= ((gvar.video.page[0].width + gvar.video.page[0].dx) - 1) || x == -(gvar.video.page[0].dx))
					xdir = -xdir;
				if ((y + vrl_header->height) >= ((gvar.video.page[0].height + gvar.video.page[0].dy) - 1) || y == -(gvar.video.page[0].dy))
					ydir = -ydir;
				//printf("[x%u y%u]	[rx%u ry%u]		[w%u h%u]\n", x, y, rx, ry, w, h);
			}
		}
	}

	IN_UserInput(1);

//===========================================================================//

	ZC_ShowMV(&MVVAR, 0, 0);
if(!noanim) {
	/* another handy "demo" effect using VGA write mode 1.
	 * we can take what's on screen and vertically squash it like an old analog TV set turning off. */
	{
		unsigned int blank_line_ofs = (gvar.video.page[0].stridew * gvar.video.page[0].height * 2);
		unsigned int copy_ofs = (gvar.video.page[0].stridew * gvar.video.page[0].height);
		unsigned int display_ofs = 0x0000;
		unsigned int i,y,soh,doh,dstart;
		unsigned int dh_blankfill = 8;
		unsigned int dh_step = 8;
		uint32_t sh,dh,yf,ystep;

		/* copy active display (0) to offscreen buffer (0x4000) */
		vga_state.vga_draw_stride_limit = vga_state.vga_draw_stride = gvar.video.page[0].stridew;
		vga_setup_wm1_block_copy();
		vga_wm1_mem_block_copy(copy_ofs,display_ofs,gvar.video.page[0].stridew * gvar.video.page[0].height);
		vga_restore_rm0wm0();

		/* need a blank line as well */
		for (i=0;i < gvar.video.page[0].stridew;i++) vga_state.vga_graphics_ram[i+blank_line_ofs] = 0;

		sh = dh = gvar.video.page[0].height;
		while (dh >= dh_step) {
			/* stop animating if the user hits ENTER */
			if (kbhit()) {
				if (getch() == 13) break;
			}

			/* wait for vsync end */
			if(gvar.kurokku.fpscap)
			vga_wait_for_vsync_end();

			/* what scalefactor to use for stretching? */
			ystep = (0x10000UL * sh) / dh;
			dstart = (gvar.video.page[0].height - dh) / 2; // center the squash effect on screen, otherwise it would squash to top of screen
			doh = display_ofs;
			soh = copy_ofs;
			yf = 0;
			y = 0;

			/* for performance, keep VGA in write mode 1 the entire render */
			vga_setup_wm1_block_copy();

			/* blank lines */
			if (dstart >= dh_blankfill) y = dstart - dh_blankfill;
			else y = 0;
			doh = gvar.video.page[0].stridew * y;

			while (y < dstart) {
				vga_wm1_mem_block_copy(doh,blank_line_ofs,gvar.video.page[0].stridew);
				doh += gvar.video.page[0].stridew;
				y++;
			}

			/* draw */
			while (y < (dh+dstart)) {
				soh = copy_ofs + ((yf >> 16UL) * gvar.video.page[0].stridew);
				vga_wm1_mem_block_copy(doh,soh,gvar.video.page[0].stridew);
				doh += gvar.video.page[0].stridew;
				yf += ystep;
				y++;
			}

			/* blank lines */
			while (y < gvar.video.page[0].height && y < (dh+dstart+dh_blankfill)) {
				vga_wm1_mem_block_copy(doh,blank_line_ofs,gvar.video.page[0].stridew);
				doh += gvar.video.page[0].stridew;
				y++;
			}

			/* done */
			vga_restore_rm0wm0();

			/* wait for vsync */
			if(gvar.kurokku.fpscap)
			vga_wait_for_vsync();

			/* make it shrink */
			dh -= dh_step;
			if (dh < 40) dh_step = 1;
		}
	}
}
	IN_Shutdown();
	VGAmodeX(0, 1, &gvar);
	free(vrl_lineoffs);
	buffer = NULL;
	free(buffer);
	bufsz = 0;
	free(bakapee1);
	free(bakapee2);
	printf("\nProject 16 0.exe. This is just a test file!\n");
	printf("version %s\n", VERSION);
	//SCROLLEXITMESG;
	printf("mv 0\n- tx=%d	ty=%d	dx=%d	dy=%d\n", MVVAR[0].tx, MVVAR[0].ty, gvar.video.page[0].dx, gvar.video.page[0].dy);
	printf("mv 1\n- tx=%d	ty=%d	dx=%d	dy=%d\n", MVVAR[1].tx, MVVAR[1].ty, gvar.video.page[1].dx, gvar.video.page[1].dy);
	return 0;
}
