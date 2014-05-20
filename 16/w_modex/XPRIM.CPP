#include <conio.h>
#include <dos.h>
#include <mem.h>
#include <stdio.h>

#include "modex.hpp"
#include "xprim.hpp"

#define SEQU_ADDR       0x3C4
#define GRACON_ADDR     0x3CE
#define CRTC_ADDR       0x3D4
#define STATUS_ADDR     0x3DA

BYTE Xfont[2048];


void
wait_for_retrace(void)
{
    while (!(inp(STATUS_ADDR) & 0x08));
}


void
setDrawPage(unsigned int page)
{
    activeStart = page_offset[page];
}


void
setVisiblePage(unsigned int page)
{
    // setVisibleStart() tells the VGA from which byte to fetch the first
    // pixel when starting refresh at the top of the screen.
    visibleStart = page_offset[page];

    // set high byte
    outpw(CRTC_ADDR, page_mask_high[page]);

    // set low byte
    outpw(CRTC_ADDR, page_mask_low[page]);
}


void
clearX(BYTE color)
{
    outpw(SEQU_ADDR, 0x0F02);
    memset((unsigned char *)(0xA000 << 4) + activeStart, color, 0x00010000);
}


void
putpixelX(COORD x, COORD y, BYTE color)
{
    BYTE temp;

    if (write_plane != (temp = (x & 3))) {
        write_plane = temp;
        outpw(SEQU_ADDR, plane_mask[temp]);
    }

    *(RowsX[y] + (x >> 2) + activeStart) = color;
}


BYTE
getpixelX(COORD x, COORD y)
{
    BYTE temp;

    if (read_plane != (temp = (x & 3))) {
        read_plane = temp;
        outpw(GRACON_ADDR, read_mask[temp]);
    }

    return (*(RowsX[y] + (x >> 2) + activeStart));
}


void
internal_vert_lineX(COORD x, COORD top_y, int len, BYTE color)
{
    BYTE *ptr;
    BYTE temp;

    if (write_plane != (temp = (x & 3))) {
        write_plane = temp;
        outpw(SEQU_ADDR, plane_mask[temp]);
    }

    ptr = RowsX[top_y] + (x >> 2) + activeStart;

    while (len--) {
        *ptr = color;
        ptr += widthBytes;
    }
}


void
internal_horiz_lineX(COORD left_x, COORD y, int len, BYTE color)
{
    BYTE *ptr;
    BYTE temp;

    ptr = RowsX[y] + (left_x >> 2) + activeStart;

    // Set current plane to invalid value
    write_plane = -1;

    if (temp = (left_x & 3)) {
        outp(SEQU_ADDR, 0x02);
        outp(0x3C5, line_head[temp]);
        *ptr++ = color;
        len -= (4 - temp);
    }

    if (temp = (len >> 2)) {
        outpw(SEQU_ADDR, 0x0F02);

        len &= 3;

        memset(ptr, color, temp);
        ptr += temp;
    }

    if (len) {
        outp(SEQU_ADDR, 0x02);
        outp(0x3C5, line_tail[len]);
        *ptr = color;
    }
}


void
boxX(COORD x1, COORD y1, COORD x2, COORD y2, BYTE color)
{
    int xsize, ysize;

    xsize = (x2 - x1) + 1;
    internal_horiz_lineX(x1, y1, xsize, color);
    internal_horiz_lineX(x1, y2, xsize, color);

    y1++;
    ysize = (y2 - y1);

    internal_vert_lineX(x1, y1, ysize, color);
    internal_vert_lineX(x2, y1, ysize, color);
}


void
filledboxX(COORD x1, COORD y1, COORD x2, COORD y2, BYTE color)
{
    BYTE *base_ptr;
    BYTE *ptr;
    BYTE temp;
    short int i, xsize, ysize;

    // Set current plane to invalid value
    write_plane = -1;

    xsize = (x2 - x1) + 1;
    ysize = (y2 - y1) + 1;

    if (ysize == 1) {
        internal_horiz_lineX(x1, y1, xsize, color);
        return;
    }

    base_ptr = RowsX[y1] + (x1 >> 2) + activeStart;

    if (temp = (x1 & 3)) {
        outp(SEQU_ADDR, 0x02);
        outp(0x3C5, line_head[temp]);
        ptr = base_ptr;
        i=ysize;
        while (i--) {
            *ptr = color;
            ptr += widthBytes;
        }
        xsize -= (4 - temp);
        base_ptr++;
    }

    if (temp = (xsize >> 2)) {
        outpw(SEQU_ADDR, 0x0F02);

        xsize &= 3;
        ptr = base_ptr;
        i=ysize;
        while (i--) {
            memset(ptr, color, temp);
            ptr += widthBytes;
        }
        base_ptr += temp;
    }

    if (xsize) {
        outp(SEQU_ADDR, 0x02);
        outp(0x3C5, line_tail[xsize]);
        while (ysize--) {
            *base_ptr = color;
            base_ptr += widthBytes;
        }
    }
}


void
circleX(COORD x, COORD y, DIST r, BYTE color)
{
    int ix, iy, d, deltaE, deltaSE;

    ix = 0;
    iy = r;
    d = 1 - r;
    deltaE = 3;
    deltaSE = (-2 * r) + 5;

    putpixelX(x + ix, y + iy, color);
    putpixelX(x + ix, y - iy, color);
    putpixelX(x + iy, y + ix, color);
    putpixelX(x + iy, y - ix, color);
    putpixelX(x - ix, y + iy, color);
    putpixelX(x - ix, y - iy, color);
    putpixelX(x - iy, y + ix, color);
    putpixelX(x - iy, y - ix, color);

    while (iy > ix++) {
        if (d < 0) {
            d += deltaE;
            deltaE += 2;
            deltaSE += 2;
        } else {
            d += deltaSE;
            deltaE += 2;
            deltaSE += 4;
            iy--;
        }

        putpixelX(x + ix, y + iy, color);
        putpixelX(x + ix, y - iy, color);
        putpixelX(x + iy, y + ix, color);
        putpixelX(x + iy, y - ix, color);
        putpixelX(x - ix, y + iy, color);
        putpixelX(x - ix, y - iy, color);
        putpixelX(x - iy, y + ix, color);
        putpixelX(x - iy, y - ix, color);
    }
}


void
filledcircleX(COORD x, COORD y, DIST r, BYTE color)
{
    int ix, iy, d, deltaE, deltaSE;

    ix = 0;
    iy = r;
    d = 1 - r;
    deltaE = 3;
    deltaSE = (-2 * r) + 5;

    internal_horiz_lineX(x - ix, y + iy, (ix << 1) + 1, color);
    internal_horiz_lineX(x - ix, y - iy, (ix << 1) + 1, color);
    internal_horiz_lineX(x - iy, y + ix, (iy << 1) + 1, color);
    internal_horiz_lineX(x - iy, y - ix, (iy << 1) + 1, color);

    while (iy > ix++) {
        if (d < 0) {
            d += deltaE;
            deltaE += 2;
            deltaSE += 2;
        } else {
            d += deltaSE;
            deltaE += 2;
            deltaSE += 4;
            iy--;
        }

        internal_horiz_lineX(x - ix, y + iy, (ix << 1) + 1, color);
        internal_horiz_lineX(x - ix, y - iy, (ix << 1) + 1, color);
        internal_horiz_lineX(x - iy, y + ix, (iy << 1) + 1, color);
        internal_horiz_lineX(x - iy, y - ix, (iy << 1) + 1, color);
    }
}


void
internal_xmajor(BYTE *vga_ptr, short int len, short int yskip,
    unsigned long ErrorAcc, unsigned long ErrorAdj, BYTE color)
{
    if (len) {
        len--;
        while (len--) {
            *vga_ptr++ = color;
            ErrorAcc += ErrorAdj;

            if (ErrorAcc & ~0xFFFFL) {
                ErrorAcc &= 0xFFFFL;
                vga_ptr += yskip;
            }
        }
        *vga_ptr = color;
    }
}


void
internal_middle(BYTE *vga_ptr, short int len, short int yskip,
    unsigned long ErrorAcc, unsigned long ErrorAdj, BYTE color)
{
    if (len) {
        len--;
        while (len--) {
            *vga_ptr++ = color;
            ErrorAcc += ErrorAdj;
            vga_ptr += (yskip * (ErrorAcc >> 16));
            ErrorAcc &= 0xFFFFL;
        }
        *vga_ptr = color;
    }
}


void
internal_ymajor(BYTE *vga_ptr, short int len, short int yskip,
    unsigned long ErrorAcc, unsigned long ErrorAdj, BYTE color)
{
    unsigned long TinyAdj;
    short int i;

    if (len) {
        TinyAdj = (ErrorAdj >> 2);
        ErrorAdj -= TinyAdj;

        len--;
        while (len--) {
            ErrorAcc += TinyAdj;
            i = (ErrorAcc >> 16);
            ErrorAcc &= 0xFFFFL;

            while (i--) {
                *vga_ptr = color;
                vga_ptr += yskip;
            }

            ErrorAcc += ErrorAdj;
            vga_ptr += (yskip * (ErrorAcc >> 16)) + 1;
            ErrorAcc &= 0xFFFFL;
        }
        ErrorAcc += TinyAdj;
        i = (ErrorAcc >> 16);
        while (i--) {
            *vga_ptr = color;
            vga_ptr += yskip;
        }
    }
}


void
lineX(COORD x1, COORD y1, COORD x2, COORD y2, BYTE color)
{
    unsigned long ErrorAcc, ErrorAdj, TinyAdj;
    short int i, DeltaX, DeltaY, yskip;
    short int len[4];
    BYTE *vga_ptr;
    COORD temp;

    // Mode X 4-way folded Bresenham line function - by David Boeren

    // Set invalid plane, because we're screwing with the plane mask
    write_plane = -1;

    // Make sure the line runs left to right
    if (x1 > x2) {
        temp = x1; x1 = x2; x2 = temp;
        temp = y1; y1 = y2; y2 = temp;
    }

    DeltaX = (x2 - x1);
    DeltaY = (y2 - y1);

    if (DeltaY >= 0) {
        yskip = widthBytes;
    } else {
        DeltaY = -DeltaY;  // Make DeltaY positive
        yskip = -widthBytes;
    }

    if (DeltaX == 0) {
        // Vertical Line (and one pixel lines)
        if (yskip > 0) {
            internal_vert_lineX(x1, y1, (DeltaY + 1), color);
        } else {
            internal_vert_lineX(x1, y2, (DeltaY + 1), color);
        }
        return;
    }

    if (DeltaY == 0) {
        // Horizontal Line
        internal_horiz_lineX(x1, y1, (DeltaX + 1), color);
        return;
    }

    vga_ptr = RowsX[y1] + (x1 >> 2) + activeStart;
    ErrorAcc = 0x8000;

    // Length of sub-line in each plane
    temp = (x1 & 3);
    i = DeltaX + temp;
    len[0] = ((i--) >> 2);
    len[1] = ((i--) >> 2);
    len[2] = ((i--) >> 2);
    len[3] = (i >> 2) + 1;

    for (i=temp; i < 3; i++) {
        len[i]++;
    }

    if ((DeltaX >> 2) >= DeltaY) {
        // X-Major line (0.00 < slope <= 0.25)
        ErrorAdj = ((((unsigned long)DeltaY << 18) / (unsigned long)DeltaX));
        TinyAdj = (ErrorAdj >> 2);
        while (i--) {
            outpw(SEQU_ADDR, plane_mask[temp]);
            internal_xmajor(vga_ptr, len[temp++], yskip, ErrorAcc, ErrorAdj, color);
            if (temp == 4) {
                temp = 0;
                vga_ptr++;
            }
            ErrorAcc += TinyAdj;
            if (ErrorAcc & ~0xFFFFL) {
                ErrorAcc &= 0xFFFFL;
                vga_ptr += yskip;
            }
        }
        outpw(SEQU_ADDR, plane_mask[temp]);
        internal_xmajor(vga_ptr, len[temp], yskip, ErrorAcc, ErrorAdj, color);
    } else if (DeltaX >= DeltaY) {
        // Middle line (0.25 < slope <= 1.00)
        ErrorAdj = ((((unsigned long)DeltaY << 18) / (unsigned long)DeltaX));
        TinyAdj = (ErrorAdj >> 2);
        while (i--) {
            outpw(SEQU_ADDR, plane_mask[temp]);
            internal_middle(vga_ptr, len[temp++], yskip, ErrorAcc, ErrorAdj, color);
            if (temp == 4) {
                temp = 0;
                vga_ptr++;
            }
            ErrorAcc += TinyAdj;
            if (ErrorAcc & ~0xFFFFL) {
                vga_ptr += yskip;
                ErrorAcc &= 0xFFFFL;
            }
        }
        outpw(SEQU_ADDR, plane_mask[temp]);
        internal_middle(vga_ptr, len[temp], yskip, ErrorAcc, ErrorAdj, color);
    } else {
        // Y-Major line (slope > 1)
        ErrorAdj = ((((unsigned long)(DeltaY+1) << 18) /
                      (unsigned long)(DeltaX+1)));
        TinyAdj = (ErrorAdj >> 2);
        while (i--) {
            outpw(SEQU_ADDR, plane_mask[temp]);
            internal_ymajor(vga_ptr, len[temp++], yskip, ErrorAcc, ErrorAdj, color);
            if (temp == 4) {
                temp = 0;
                vga_ptr++;
            }
            ErrorAcc += TinyAdj;
            vga_ptr += (yskip * (ErrorAcc >> 16));
            ErrorAcc &= 0xFFFFL;
        }
        outpw(SEQU_ADDR, plane_mask[temp]);
        internal_ymajor(vga_ptr, len[temp], yskip, ErrorAcc, ErrorAdj, color);
    }
}


int
loadfontX(char *fname)
{
    FILE *fp;

    fp = fopen(fname, "rb");

    if (fp == NULL) {
        return 0;
    } else {
        fread(Xfont, 8, 256, fp);
        fclose(fp);
        return 1;
    }
}


void
putchX(COORD x, COORD y, char c, BYTE color)
{
    int i;
    BYTE *vga_ptr;
    BYTE *font_ptr;
    BYTE temp;

    // 8x8 font
    vga_ptr = RowsX[y << 3] + (x << 1) + activeStart;
    write_plane = -1;

    font_ptr = Xfont + (c << 3);

    i=8;
    while (i--) {
        temp = *font_ptr++;
        outpw(SEQU_ADDR, text_mask[temp & 0x0F]);
        *vga_ptr++ = color;

        outpw(SEQU_ADDR, text_mask[temp >> 4]);
        *vga_ptr-- = color;
        vga_ptr += widthBytes;
    }
}


void
putstringX(COORD x, COORD y, char *str, BYTE color)
{
    int i, skip;
    BYTE *vga_ptr;
    BYTE *font_ptr;
    BYTE c, temp;

    // 8x8 font
    vga_ptr = RowsX[y << 3] + (x << 1) + activeStart;
    write_plane = -1;

    skip = 2 - (widthBytes << 3);

    while (c = *str++) {
        font_ptr = Xfont + (c << 3);

        i=8;
        while (i--) {
            temp = *font_ptr++;
            outpw(SEQU_ADDR, text_mask[temp & 0x0F]);
            *vga_ptr++ = color;

            outpw(SEQU_ADDR, text_mask[temp >> 4]);
            *vga_ptr-- = color;
            vga_ptr += widthBytes;
        }

        vga_ptr += skip;
    }
}

