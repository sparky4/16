#include <dos.h>
#include <string.h>
#include <mem.h>
#include <conio.h>
#include "modex16.h"


byte far* VGA=(byte far*) 0xA0000000; 	/* this points to video memory. */

static void
vgaSetMode(byte mode)
{
  union REGS regs;

  regs.h.ah = SET_MODE;
  regs.h.al = mode;
  int86(VIDEO_INT, &regs, &regs);
}


/* -========================= Entry  Points ==========================- */
void
modexEnter() {
    word i;
    dword far*ptr=(dword far*)VGA;      /* used for faster screen clearing */
    word CRTParms[] = {
	0x0d06,		/* vertical total */
	0x3e07,		/* overflow (bit 8 of vertical counts) */
	0x4109,		/* cell height (2 to double-scan */
	0xea10,		/* v sync start */
	0xac11,		/* v sync end and protect cr0-cr7 */
	0xdf12,		/* vertical displayed */
	0x0014,		/* turn off dword mode */
	0xe715,		/* v blank start */
	0x0616,		/* v blank end */
	0xe317		/* turn on byte mode */
    };
    int CRTParmCount = sizeof(CRTParms) / sizeof(CRTParms[0]);

    /* TODO save current video mode and palette */
    vgaSetMode(VGA_256_COLOR_MODE);

    /* disable chain4 mode */
    outpw(SC_INDEX, 0x0604);

    /* synchronous reset while setting Misc Output */
    outpw(SC_INDEX, 0x0100);

    /* select 25 MHz dot clock & 60 Hz scanning rate */
    outp(MISC_OUTPUT, 0xe3);

    /* undo reset (restart sequencer) */
    outpw(SC_INDEX, 0x0300);

    /* reprogram the CRT controller */
    outp(CRTC_INDEX, 0x11); /* VSync End reg contains register write prot */
    outp(CRTC_DATA, 0x7f);  /* get current write protect on varios regs */

    /* send the CRTParms */
    for(i=0; i<CRTParmCount; i++) {
	outpw(CRTC_INDEX, CRTParms[i]);
    }

    /* clear video memory */
    outpw(SC_INDEX, 0x0f02);
    for(i=0; i<0x8000; i++) {
	ptr[i] = 0x0000;
    }
}


void
modexLeave() {
    /* TODO restore original mode and palette */
    vgaSetMode(TEXT_MODE);
}


void
modexShowPage(page_t page) {
    word high_address;
    word low_address;

    high_address = HIGH_ADDRESS | ((word)(page) & 0xff00);
    low_address  = LOW_ADDRESS  | ((word)(page) << 8);

    /* wait for appropriate timing */
    while ((inp(INPUT_STATUS_1) & DISPLAY_ENABLE));
    outpw(CRTC_INDEX, high_address);
    outpw(CRTC_INDEX, low_address);

    /*  wait for one retrace */
    while (!(inp(INPUT_STATUS_1) & VRETRACE)); 
}


void
modexPanPage(page_t *page, int dx, int dy) {
    /* TODO figure out how the $@#! you do horizontal panning */
    *page += dy * SCREEN_WIDTH;
}


void
modexSelectPlane(byte plane) {
    outp(SC_INDEX, MAP_MASK);          /* select plane */
    outp(SC_DATA,  plane);
}


void
modexClearRegion(page_t page, int x, int y, int w, int h, byte color) {
    byte plane;
    word endx = x + w;
    word endy = y + h;
    word dx, dy;

    /* TODO Make this fast.  It's SLOOOOOOW */
    for(plane=0; plane < 4; plane++) {
	modexSelectPlane(PLANE(plane+x));
	for(dx = x; dx < endx; dx+=4) {
	    for(dy=y; dy<endy; dy++) {
		page[PAGE_OFFSET(dx, dy)] = color;
	    }
	}
    }
}
