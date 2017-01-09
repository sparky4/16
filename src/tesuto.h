#ifndef __TESUTO_H__
#define __TESUTO_H__

#include "src/lib/16_head.h"
#include "src/lib/16_vl.h"
#include "src/lib/16_in.h"
#include "src/lib/scroll16.h"
#include <hw/cpu/cpu.h>
#include <hw/dos/dos.h>
#include <hw/vga/vga.h>
#include <hw/vga/vrl.h>

//panPageManual(mv, player, 0);
#define PANKEY0EXE \
			panVmemManual(mv, player, 0); \
			if(IN_KeyDown(1+1)){ pan.pn=0; modexShowPage(&(gvar.video.page[pan.pn])); } \
			if(IN_KeyDown(2+1)){ pan.pn=1; modexShowPage(&(gvar.video.page[pan.pn])); } \
			if(IN_KeyDown(3+1)){ pan.pn=2; modexShowPage(&(gvar.video.page[pan.pn])); } \
			if(IN_KeyDown(4+1)){ pan.pn=3; modexShowPage(&(gvar.video.page[pan.pn])); } \
			if(IN_KeyDown(25)){ modexpdump(mv[1].page); modexShowPage(&(gvar.video.page[1])); IN_UserInput(1,1); }

////corner markers
#define GVPO gvar.video.page[0]
#define GVPI gvar.video.page[1]
#define OW	GVPO.width-16
#define OH	GVPO.height-16
#define OE	GVPO.sw
#define OJ	GVPO.sh
#define IW	GVPI.width-16
#define IH	GVPI.height-16
#define IE	GVPI.sw
#define IJ	GVPI.sh
#define DCBO modexClearRegion(&GVPO
#define DCBI modexClearRegion(&GVPI

#define DRAWOTHERCORNERBOX_TOPLEFT \
DCBO,	0,	0,	16, 16, 0); \
DCBO,	16,	16,	16, 16, 2); \
DCBI,		0,	0,	16, 16, 0); \
DCBI,		16,	16,	16, 16, 2);

#define DRAWOTHERCORNERBOX_TOPRIGHT \
DCBO,	OW,	0,	16, 16, 0); \
DCBO,	OE,	16,	16, 16, 2); \
DCBI,		IW,	0,	16, 16, 0); \
DCBI,		IE,	16,	16, 16, 2);

#define DRAWOTHERCORNERBOX_BOTTOMLEFT \
DCBO,	0,	OH,	16, 16, 0); \
DCBO,	16,	OJ,	16, 16, 2); \
DCBI,		0,	IH,	16, 16, 0); \
DCBI,		16,	IJ,	16, 16, 2);

#define DRAWOTHERCORNERBOX_BOTTOMRIGHT \
DCBO,	OE,	OJ,	16, 16, 2); \
DCBO,	OW,	OH,	16, 16, 0); \
DCBI,		IE,	IJ,	16, 16, 2);\
DCBI,		IW,	IH,	16, 16, 0);

#define DRAWCORNERBOXES \
DRAWOTHERCORNERBOX_TOPLEFT; \
DRAWOTHERCORNERBOX_TOPRIGHT; \
DRAWOTHERCORNERBOX_BOTTOMLEFT; \
DRAWOTHERCORNERBOX_BOTTOMRIGHT; \

#define TESTBG \
	modexClearRegion(&gvar.video.page[0], 0, 0, gvar.video.page[0].width, gvar.video.page[0].height, 15); \
	modexClearRegion(&gvar.video.page[0], 16, 16, gvar.video.page[0].sw, gvar.video.page[0].sh, 128); \
	modexClearRegion(&gvar.video.page[0], 32, 32, gvar.video.page[0].sw-32, gvar.video.page[0].sh-32, 42); \
	modexClearRegion(&gvar.video.page[0], 48, 48, gvar.video.page[0].sw-64, gvar.video.page[0].sh-64, 128); \
	modexCopyPageRegion(&gvar.video.page[1], &gvar.video.page[0], 0, 0, 0, 0, gvar.video.page[0].width, gvar.video.page[0].height); \
	modexClearRegion(&gvar.video.page[2], 0, 0, gvar.video.page[2].sw, gvar.video.page[2].sh, 47); \
	modexClearRegion(&gvar.video.page[3], 0, 0, gvar.video.page[3].sw, gvar.video.page[3].sh, 45);

typedef unsigned char far *VGA_RAM_PTR;
//VGA_RAM_PTR vga_graphics_ram = (VGA_RAM_PTR)MK_FP(0xA000,0x0000);
//unsigned char vga_stride = 80; // 80 x 4 = 320 for 320-pixel wide modes

/*static inline void vga_write_sequencer(unsigned char i,unsigned char c) {
    outp(0x3C4,i);
    outp(0x3C5,c);
}*/
#endif
