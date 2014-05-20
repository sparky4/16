#include <conio.h>
#include <dos.h>
#include <mem.h>

#include "modex.hpp"
#include "xpal.hpp"

BYTE Xpal[768];

void
set_paletteX(BYTE *pal, FLAG downgrade)
{
    short int i;
    BYTE *buf;

    memcpy(Xpal, pal, 768);

    buf = Xpal;
    if (downgrade) {
        i=768;
        while (i--) {
            *buf++ = (*buf >> 2);
        }
    }

    outp(0x03c8, 0);  // Start with color 0
    buf = Xpal;
    i=256;
    while (i--) {
        outp(0x03c9, *buf++);
        outp(0x03c9, *buf++);
        outp(0x03c9, *buf++);
    }
}


void
get_paletteX(BYTE *pal, FLAG upgrade)
{
    int i;

    memcpy(pal, Xpal, 768);

    if (upgrade) {
        i=768;
        while (i--) {
            *pal++ = (*pal << 2);
        }
    }
}


void
get_BIOSpaletteX(BYTE *pal, FLAG upgrade)
{
    int i;
    union REGS r;

    r.x.eax = 0x1017;
    r.x.ebx = 0;
    r.x.ecx = 256;
    r.x.edx = (unsigned long) pal;

    int386(0x10, &r, &r);

    if (upgrade) {
        i=768;
        while (i--) {
            *pal++ = (*pal << 2);
        }
    }
}


void
photo_negativeX(void)
{
    short int i;
    BYTE temp_pal[768];
    BYTE *temp;

    get_paletteX(temp_pal, 0);
    temp = temp_pal;

    for (i=0; i < 256; i++) {
        *temp++ = (64 - (*temp));
        *temp++ = (64 - (*temp));
        *temp++ = (64 - (*temp));
    }

    set_paletteX(temp_pal, 0);
}


void
grey_paletteX(void)
{
    smooth64_paletteX(1, 1, 1);
}


void
RGB_paletteX(void)
{
    BYTE r, g, b;
    BYTE temp_pal[768];
    BYTE *temp;

    temp = temp_pal;

    for (r=0; r < 8; r++) {
        for (g=0; g < 8; g++) {
            for (b=0; b < 4; b++) {
                *temp++ = (r << 3);
                *temp++ = (g << 3);
                *temp++ = (b << 4);
            }
        }
    }

    set_paletteX(temp_pal, 0);
}


void
smooth64_paletteX(BYTE r, BYTE g, BYTE b)
{
    short int i;
    BYTE temp_pal[768];
    BYTE *temp;

    memset(temp_pal, 0, 768);


    if (r) {
        temp = temp_pal;
        for (i=0; i < 64; i++) {
            *temp = i;
            temp += 3;
        }
    }

    if (g) {
        temp = temp_pal + 1;
        for (i=0; i < 64; i++) {
            *temp = i;
            temp += 3;
        }
    }

    if (b) {
        temp = temp_pal + 2;
        for (i=0; i < 64; i++) {
            *temp = i;
            temp += 3;
        }
    }

    set_paletteX(temp_pal, 0);
}


void
brighten_paletteX(SBYTE r, SBYTE g, SBYTE b)
{
    short int i, j, scratch;
    BYTE temp_pal[768];
    BYTE *temp;
    SBYTE dummy[3];

    get_paletteX(temp_pal, 0);
    temp = temp_pal;

    dummy[0] = r;
    dummy[1] = g;
    dummy[2] = b;

    for (i=0; i < 256; i++) {
        for (j=0; j < 3; j++) {
            scratch = *temp + dummy[j];
            if (scratch <= 0) {
                *temp++ = 0;
            } else if (scratch >= 63) {
                *temp++ = 63;
            } else {
                *temp++ = scratch;
            }
        }
    }

    set_paletteX(temp_pal, 0);
}


void
stretch_paletteX(BYTE r, BYTE g, BYTE b)
{
    short int i, j, scratch;
    BYTE temp_pal[768];
    BYTE *temp;
    BYTE dummy[3];

    get_paletteX(temp_pal, 0);
    temp = temp_pal;

    dummy[0] = r;
    dummy[1] = g;
    dummy[2] = b;

    for (i=0; i < 256; i++) {
        for (j=0; j < 3; j++) {
            scratch = ((((*temp - 32) * dummy[j]) + 8) >> 4) + 32;
            if (scratch <= 0) {
                *temp++ = 0;
            } else if (scratch >= 63) {
                *temp++ = 63;
            } else {
                *temp++ = scratch;
            }
        }
    }

    set_paletteX(temp_pal, 0);
}


void
rot_palette(BYTE dist)
{
    int shift, i;
    BYTE temp_pal[768];

    shift = (dist * 3);
    memcpy(temp_pal, Xpal + shift, 768 - shift);
    memcpy(temp_pal + (768 - shift), Xpal, shift);

    set_paletteX(temp_pal, 0);
}


BYTE
find_RGB(BYTE r, BYTE g, BYTE b)
{
    long shortest_dist, temp_dist;
    short int i, shortest_pal;

    shortest_pal = 0;
    shortest_dist = (r - Xpal[0]) * (r - Xpal[0]) +
                    (g - Xpal[1]) * (g - Xpal[1]) +
                    (b - Xpal[2]) * (b - Xpal[2]);

    for (i=1; i < 256; i++) {
        temp_dist = (r - Xpal[(i * 3) + 0]) * (r - Xpal[(i * 3) + 0]) +
                    (g - Xpal[(i * 3) + 1]) * (g - Xpal[(i * 3) + 1]) +
                    (b - Xpal[(i * 3) + 2]) * (b - Xpal[(i * 3) + 2]);

        if (temp_dist < shortest_dist) {
            shortest_dist = temp_dist;
            shortest_pal = i;
        }
    }

    return i;
}

