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
 * You screen.heightould have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "src/bakapi.h"

/*
 * BAKAPEE!
 */
static bakapee_t bakapee;
word key,d,xpos,ypos,xdir,ydir;
sword vgamodex_mode = 1; //	1 = 320x240 with buffer
void TL_VidInit(global_game_variables_t *gvar){}
//int ch=0x0;

#define SETUPPAGEBAKAPI \
gvar.video.page[0] = modexDefaultPage(&gvar.video.page[0], &gvar.video); \
gvar.video.page[1] = modexNextPage(&gvar.video.page[0]);


void
main(int argc, char *argvar[])
{
	static global_game_variables_t gvar;
	struct glob_game_vars	*ggvv;
	char *a;
	int i;
	word panq=1, pand=0;
	boolean panswitch=0;
	word	showpage=0;

	ggvv=&gvar;

	// allow changing default mode from command line
	for (i=1;i < argc;) {
		a = argvar[i++];

		if (*a == '-') {
			do { a++; } while (*a == '-');

			if (!strcmp(a,"mx")) {
				// (based on src/lib/modex16.c)
				// 1 = 320x240
				// 2 = 160x120
				// 3 = 320x200
				// 4 = 192x144
				// 5 = 256x192
				vgamodex_mode = (sword)strtoul(argvar[i++],NULL,0);
			}
			else {
				fprintf(stderr,"Unknown switch %s\n",a);
				return;
			}
		}
		else {
			fprintf(stderr,"Unknown command arg %s\n",a);
			return;
		}
	}

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
	textInit();

	// main variables values
	d=4; // switch variable
	key=2; // default screensaver number
	xpos=TILEWHD;
	ypos=TILEWHD;
	xdir=1;
	ydir=1;

	VGAmodeX(vgamodex_mode, 0, &gvar); // TODO: Suggestion: Instead of magic numbers for the first param, might I suggest defining an enum or some #define constants that are easier to remember? --J.C.
		// this code is written around modex16 which so far is a better fit than using DOSLIB vga directly, so leave MXLIB code in.
		// we'll integrate DOSLIB vga into that part of the code instead for less disruption. -- J.C.

	bakapee.xx = rand()&0%gvar.video.page[0].width;
	bakapee.yy = rand()&0%gvar.video.page[0].height;
	bakapee.gq = 0;
	bakapee.sx=	bakapee.sy=0;
	bakapee.bakax=	bakapee.bakay=0;
	bakapee.coor=0;

	//once where #defines
	bakapee.tile=0;
	bakapee.bonk=400;
	bakapee.lgq=32;
	bakapee.hgq=55;

	switch(WCPU_detectcpu())
	{
		case 0:
			bakapee.tile=1;
		break;
		default:
			bakapee.tile=0;
		break;
	}

	/* setup camera and screen~ */
	SETUPPAGEBAKAPI

	//modexPalUpdate(bmp.palette); //____
	//modexDrawBmp(VGA, 0, 0, &bmp, 0); //____
	//getch(); //____
	VL_SetLineWidth (40, &gvar);

	VL_ShowPage(&gvar.video.page[showpage], 0, 0);
	{
		word w;
		for(w=0;w<64000;w++)
		{
			ding(&gvar.video.page[showpage], &bakapee, 4);
			ding(&gvar.video.page[1], &bakapee, 4);
		}
		if(!baka_FizzleFade (gvar.video.ofs.bufferofs, gvar.video.ofs.displayofs, vga_state.vga_width, vga_state.vga_height, 70, true, &gvar))
		//ding(&gvar.video.page[showpage], &bakapee, 9);
		modexprint(&gvar.video.page[0], gvar.video.page[0].sw/2, gvar.video.page[0].sh/2, 1, 0, 47, 0, 1, "bakapi ok");
	}
	while(!kbhit()){}

// screen savers
//#ifdef BOINK
	while(d>0)	// on!
	{
		int c;
		/* run screensaver routine until keyboard input */
		while (key > 0) {
			if (kbhit()) {
				if(!panswitch)
				{
					getch(); // eat keyboard input
					break;
				}else c=getch();
			}

			if(!panswitch)	ding(&gvar.video.page[0], &bakapee, key);
			else			ding(&gvar.video.page[0], &bakapee, 2);
			if(panswitch!=0)
			{
				//right movement
				if((c==0x4d && pand == 0) || pand == 2)
				{
					if(pand == 0){ pand = 2; }
					if(panq<=(TILEWH/(4)))
					{
						gvar.video.page[0].dx++;
						VL_ShowPage(&gvar.video.page[0], 0, 0);
						panq++;
					} else { panq = 1; pand = 0; }
				}
				//left movement
				if((c==0x4b && pand == 0) || pand == 4)
				{
					if(pand == 0){ pand = 4; }
					if(panq<=(TILEWH/(4)))
					{
						gvar.video.page[0].dx--;
						VL_ShowPage(&gvar.video.page[0], 0, 0);
						panq++;
					} else { panq = 1; pand = 0; }
				}
				//down movement
				if((c==0x50 && pand == 0) || pand == 3)
				{
					if(pand == 0){ pand = 3; }
					if(panq<=(TILEWH/(4)))
					{
						gvar.video.page[0].dy++;
						VL_ShowPage(&gvar.video.page[0], 0, 0);
						panq++;
					} else { panq = 1; pand = 0; }
				}
				//up movement
				if((c==0x48 && pand == 0) || pand == 1)
				{
					if(pand == 0){ pand = 1; }
					if(panq<=(TILEWH/(4)))
					{
						gvar.video.page[0].dy--;
						VL_ShowPage(&gvar.video.page[0], 0, 0);
						panq++;
					} else { panq = 1; pand = 0; }
				}
				if((c==0x4d && pand == 0) || pand == 2)
				{
					if(pand == 0){ pand = 2; }
					if(panq<=(TILEWH/(4)))
					{
						gvar.video.page[0].dx++;
						VL_ShowPage(&gvar.video.page[0], 0, 0);
						panq++;
					} else { panq = 1; pand = 0; }
				}
				if(c==0x01+1)
				{
					VL_ShowPage(&gvar.video.page[0], 0, 0);
				}
				if(c==0x02+1)
				{
					VL_ShowPage(&gvar.video.page[1], 0, 0);
				}
				if(c==0x71 || c==0xb1 || c=='p')
				{
					//getch(); // eat keyboard input
					panswitch=0;
					break; // 'q' or 'ESC' or 'p'
				}
			}
		}

		{

		// this code is written around modex16 which so far is a better fit than using DOSLIB vga directly, so leave MXLIB code in.
		// we'll integrate DOSLIB vga into that part of the code instead for less disruption. -- J.C.
			VGAmodeX(0, 0, &gvar);
			// user imput switch
			//fprintf(stderr, "xx=%d	yy=%d	tile=%d\n", bakapee.xx, bakapee.yy, bakapee.tile);
			//fprintf(stderr, "dx=%d	dy=%d	", gvar.video.page[0].dx, gvar.video.page[0].dy);
			printf("Tiled mode is ");
			switch (bakapee.tile)
			{
				case 0:
					printf("off.	");
				break;
				case 1:
					printf("on.	");
				break;
			}
			printf("Pan mode is ");
			switch (panswitch)
			{
				case 0:
					printf("off.");
				break;
				case 1:
					printf("on.");
				break;
			}
			printf(" Showing page %u", showpage);
			printf("\n");
			printf("Incrementation of color happens at every %uth plot.\n", bakapee.bonk);
			printf("Enter 1, 2, 3, 4, 5, 6, 8, or 9 to run a screensaver, or enter 0 to quit.\n");
pee:
			c = getch();
			switch (c) {
				case 27: /* Escape key */
				case '0':
					d=0;
					break;
				case 'p': // test pan
				switch (panswitch)
				{
					case 0:
						panswitch=1;
					break;
					case 1:
						panswitch=0;
					break;
				}
				goto pee;
				break;
				case 'b': // test tile change
					switch (bakapee.tile)
					{
						case 0:
							bakapee.tile=1;
						break;
						case 1:
							bakapee.tile=0;
						break;
					}
					key=0;
				break;
				case 'q':
					switch (showpage)
					{
						case 0:
							showpage=1;
						break;
						case 1:
							showpage=0;
						break;
					}
					key=0;
				break;
				case '8':
					c+=8;
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '9':
					key = c - '0';
					VGAmodeX(vgamodex_mode, 0, &gvar);
					VL_ShowPage(&gvar.video.page[showpage], 0, 0);
					SETUPPAGEBAKAPI
		// this code is written around modex16 which so far is a better fit than using DOSLIB vga directly, so leave MXLIB code in.
		// we'll integrate DOSLIB vga into that part of the code instead for less disruption. -- J.C.
					VL_ShowPage(&gvar.video.page[showpage], 0, 0);
					break;
				case '-':
					if(bakapee.bonk>0)
						bakapee.bonk-=100;
				break;
				case '=':
				case '+':
					if(bakapee.bonk<1000)
						bakapee.bonk+=100;
				break;
				default:
					key=0;
				break;
			}
			clrscr();	//added to clear screen wwww
		}
	}
#if 0
//#else // !defined(BOINK)
// FIXME: Does not compile. Do you want to remove this?
// INFO: This is a testing section for textrendering and panning for project 16 --sparky4
	while(1)
	{ // conditions of screen saver
// 		while(!kbhit())
// 		{
// 			ding(&gvar.video.page[0], &bakapee, key);
// 		}
		//end of screen savers
		//pdump(&gvar.video.page[0]);

// 		mxOutText(xpos+1, ypos+gvar.video.page[0].height-48, "========================================");
// 		mxOutText(xpos+1, ypos+gvar.video.page[0].height-40, "|    |Chikyuu:$line1");
// 		mxOutText(xpos+1, ypos+gvar.video.page[0].height-32, "|    |$line2");
// 		mxOutText(xpos+1, ypos+gvar.video.page[0].height-24, "|    |$line3");
// 		mxOutText(xpos+1, ypos+gvar.video.page[0].height-16, "|    |$line4");
// 		mxOutText(xpos+1, ypos+gvar.video.page[0].height-8,  "========================================");

	ding(&gvar.video.page[0], &bakapee, key);
	modexPanPage(&gvar.video.page[0], xpos, ypos);
	c = getch();

// 	xpos+=xdir;
// 	ypos+=ydir;
// 	if( (xpos>(gvar.video.page[0].sw-gvar.video.page[0].width-1))  || (xpos<1)){xdir=-xdir;}
// 	if( (ypos>(gvar.video.page[0].sh-gvar.video.page[0].height-1)) || (ypos<1)){ydir=-ydir;}
//	ch=getch();
	if(ch==0x71)break; // 'q'
	if(ch==0x1b)break; // 'ESC'
	}
	VGAmodeX(0, 1, &gvar);
#endif // defined(BOINK)
//	printf("page.width=%u	", gvar.video.page[0].width); printf("page.height=%u\n", gvar.video.page[0].height);
	printf("bakapi ver. 1.04.16.04\nis made by sparky4i†ƒÖ…j feel free to use it ^^\nLicence: GPL v3\n");
	printf("compiled on 2016/04/04\n");
//	printf("[%u]%dx%d	[%dx%d]	%u %u %u\n[%u	%u	%u]", key, bakapee.bakax, bakapee.bakay, bakapee.xx, bakapee.yy,
//bakapee.coor, bakapee.tile, bakapee.gq, bakapee.bonk, bakapee.lgq, bakapee.hgq);
}
//pee!
