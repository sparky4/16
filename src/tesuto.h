#ifndef __TESUTO_H__
#define __TESUTO_H__

#include "src/lib/16_head.h"
#include "src/lib/modex16.h"
#include <hw/cpu/cpu.h>
#include <hw/vga/vga.h>
#include <hw/dos/dos.h>


typedef unsigned char far *VGA_RAM_PTR;
//VGA_RAM_PTR vga_graphics_ram = (VGA_RAM_PTR)MK_FP(0xA000,0x0000);
//unsigned char vga_stride = 80; // 80 x 4 = 320 for 320-pixel wide modes

/*static inline void vga_write_sequencer(unsigned char i,unsigned char c) {
    outp(0x3C4,i);
    outp(0x3C5,c);
}*/
#endif
