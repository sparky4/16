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

#define PANKEY0EXE \
			panpagemanual(mv, player, 0); \
			if(IN_KeyDown(1+1)){ pan.pn=0; modexShowPage(&(gvar.video.page[pan.pn])); } \
			if(IN_KeyDown(2+1)){ pan.pn=1; modexShowPage(&(gvar.video.page[pan.pn])); } \
			if(IN_KeyDown(3+1)){ pan.pn=2; modexShowPage(&(gvar.video.page[pan.pn])); } \
			if(IN_KeyDown(4+1)){ pan.pn=3; modexShowPage(&(gvar.video.page[pan.pn])); } \
			if(IN_KeyDown(25)){ modexpdump(mv[1].page); modexShowPage(&(gvar.video.page[1])); IN_UserInput(1,1); }

////corner markers
#define GVP gvar.video.page[0]
#define GW	GVP.width-16
#define GH	GVP.height-16
#define GE	GVP.sw
#define GJ	GVP.sh
#define DCBF modexClearRegion(&GVP

#define DRAWOTHERCORNERBOX_TOPLEFT \
DCBF, 0,	0,	16, 16, 0); \
DCBF, 16,	16,	16, 16, 2);

#define DRAWOTHERCORNERBOX_TOPRIGHT \
DCBF, GW,	0,	16, 16, 0); \
DCBF, GE,	16,	16, 16, 2);

#define DRAWOTHERCORNERBOX_BOTTOMLEFT \
DCBF, 0,	GH,	16, 16, 0); \
DCBF, 16,	GJ,	16, 16, 2);

#define DRAWOTHERCORNERBOX_BOTTOMRIGHT \
DCBF, GE,	GJ,	16, 16, 2);\
DCBF, GW,	GH,	16, 16, 0);



typedef unsigned char far *VGA_RAM_PTR;
//VGA_RAM_PTR vga_graphics_ram = (VGA_RAM_PTR)MK_FP(0xA000,0x0000);
//unsigned char vga_stride = 80; // 80 x 4 = 320 for 320-pixel wide modes

/*static inline void vga_write_sequencer(unsigned char i,unsigned char c) {
    outp(0x3C4,i);
    outp(0x3C5,c);
}*/
#endif
