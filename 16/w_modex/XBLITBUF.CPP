#include <conio.h>
#include <mem.h>
#include <stdio.h>

#include "xtypes.hpp"
#include "modex.hpp"
#include "xpal.hpp"
#include "xblitbuf.hpp"

#define SEQU_ADDR       0x3C4
#define GRACON_ADDR     0x3CE


void
clear_blitbuf(blitbuf *buf)
{
    buf->xsize = 0;
    buf->ysize = 0;

    delete buf->image;
}


void
fill_blitbuf(BYTE color, blitbuf *buf)
{
    memset(buf->image, color, buf->xsize * buf->ysize);
}


void
alloc_blitbuf(blitbuf *buf, DIST xsize, DIST ysize)
{
    buf->xsize = xsize;
    buf->ysize = ysize;
    buf->image = new BYTE[xsize * ysize];
}


void
lin_2_pln_blitbuf(blitbuf *buf)
{
    int i, j, size;
    BYTE *oldbuf;
    BYTE *newbuf;
    BYTE *src_ptr;
    BYTE *dest_ptr;

    oldbuf = buf->image;

    size = (buf->xsize * buf->ysize);
    newbuf = new BYTE[size];
    size = (size >> 2);

    dest_ptr = newbuf;

    for (i=0; i < 4; i++) {
        src_ptr = oldbuf + i;

        j=size;
        while (j--) {
            *dest_ptr++ = *src_ptr;
            src_ptr += 4;
        }
    }

    buf->image = newbuf;
    delete oldbuf;
}


void
pln_2_lin_blitbuf(blitbuf *buf)
{
    int i, j, size;
    BYTE *oldbuf;
    BYTE *newbuf;
    BYTE *src_ptr;
    BYTE *dest_ptr;

    oldbuf = buf->image;

    size = (buf->xsize * buf->ysize);
    newbuf = new BYTE[size];
    size = (size >> 2);

    src_ptr = oldbuf;
    for (i=0; i < 4; i++) {
        dest_ptr = newbuf + i;

        j=size;
        while (j--) {
            *dest_ptr = *src_ptr++;
            dest_ptr += 4;
        }
    }

    buf->image = newbuf;
    delete oldbuf;
}


void
vanilla_bitblitX(COORD x, COORD y, blitbuf *buf)
{
    short int ysize, plane, i, j, loop, skip, rewind, len;
    short int xsize[4];
    BYTE *base_vga;
    BYTE *vga_ptr;
    BYTE *buf_ptr;

    // Length of bitmap in each plane
    plane = (x & 3);
    i = buf->xsize + plane - 1;
    xsize[0] = ((i--) >> 2);
    xsize[1] = ((i--) >> 2);
    xsize[2] = ((i--) >> 2);
    xsize[3] = (i >> 2) + 1;

    for (i=plane; i < 3; i++) {
        xsize[i]++;
    }

    ysize = buf->ysize;
    base_vga = RowsX[y] + (x >> 2) + activeStart;
    write_plane = -1;

    for (loop = 0; loop < 4; loop++) {
        len = xsize[plane];
        rewind = buf->xsize - (len << 2);
        skip = widthBytes - len;
        buf_ptr = buf->image + loop;
        vga_ptr = base_vga;

        outpw(SEQU_ADDR, plane_mask[plane++]);
        if (plane == 4) {
            plane = 0;
            base_vga++;
        }

        i=ysize;
        while (i--) {
            j=len;
            while (j--) {
                *vga_ptr++ = *buf_ptr;
                buf_ptr += 4;
            }

            buf_ptr += rewind;
            vga_ptr += skip;
        }
    }
}


void
vanilla_getblitX(COORD x, COORD y, blitbuf *buf)
{
    // Do nothing
}


void
aligned_bitblitX(COORD x, COORD y, blitbuf *buf)
{
    short int i, j, plane, skip, xsize, ysize;
    BYTE *base_vga;
    BYTE *vga_ptr;
    BYTE *buf_ptr;

    xsize = (buf->xsize >> 2);
    ysize = buf->ysize;
    skip = widthBytes - xsize;
    base_vga = RowsX[y] + (x >> 2) + activeStart;

    for (plane=0; plane < 4; plane++) {
        buf_ptr = buf->image + plane;
        vga_ptr = base_vga;

        outpw(SEQU_ADDR, plane_mask[plane]);

        i=ysize;
        while (i--) {
            j=xsize;
            while (j--) {
                *vga_ptr++ = *buf_ptr;
                buf_ptr += 4;
            }
            vga_ptr += skip;
        }
    }

    write_plane = 3;
}


void
aligned_getblitX(COORD x, COORD y, blitbuf *buf)
{
    short int i, j, plane, skip, xsize, ysize;
    BYTE *base_vga;
    BYTE *vga_ptr;
    BYTE *buf_ptr;

    xsize = (buf->xsize >> 2);
    ysize = buf->ysize;
    skip = widthBytes - xsize;
    base_vga = RowsX[y] + (x >> 2) + activeStart;

    for (plane=0; plane < 4; plane++) {
        buf_ptr = buf->image + plane;
        vga_ptr = base_vga;

        outpw(GRACON_ADDR, read_mask[plane]);

        i=ysize;
        while (i--) {
            j=xsize;
            while (j--) {
                *buf_ptr = *vga_ptr++;
                buf_ptr += 4;
            }
            vga_ptr += skip;
        }
    }

    read_plane = 3;
}


void
transparent_bitblitX(COORD x, COORD y, blitbuf *buf)
{
    short int i, j, plane, skip, xsize, ysize;
    BYTE *base_vga;
    BYTE *vga_ptr;
    BYTE *buf_ptr;

    xsize = (buf->xsize >> 2);
    ysize = buf->ysize;
    skip = widthBytes - xsize;
    base_vga = RowsX[y] + (x >> 2) + activeStart;

    for (plane=0; plane < 4; plane++) {
        buf_ptr = buf->image + plane;
        vga_ptr = base_vga;

        outpw(SEQU_ADDR, plane_mask[plane]);

        i=ysize;
        while (i--) {
            j=xsize;
            while (j--) {
                if (*buf_ptr) {
                    *vga_ptr = *buf_ptr;
                }
                vga_ptr++;
                buf_ptr += 4;
            }
            vga_ptr += skip;
        }
    }

    write_plane = 3;
}


void
planar_bitblitX(COORD x, COORD y, blitbuf *buf)
{
    short int i, plane, xsize, ysize;
    BYTE *base_vga;
    BYTE *vga_ptr;
    BYTE *buf_ptr;

    xsize = (buf->xsize >> 2);
    ysize = buf->ysize;
    base_vga = RowsX[y] + (x >> 2) + activeStart;
    buf_ptr = buf->image;

    for (plane=0; plane < 4; plane++) {
        vga_ptr = base_vga;

        outpw(SEQU_ADDR, plane_mask[plane]);

        i=ysize;
        while (i--) {
            memcpy(vga_ptr, buf_ptr, xsize);
            vga_ptr += widthBytes;
            buf_ptr += xsize;
        }
    }

    write_plane = 3;
}


void
planar_getblitX(COORD x, COORD y, blitbuf *buf)
{
    short int i, plane, xsize, ysize;
    BYTE *base_vga;
    BYTE *vga_ptr;
    BYTE *buf_ptr;

    xsize = (buf->xsize >> 2);
    ysize = buf->ysize;
    base_vga = RowsX[y] + (x >> 2) + activeStart;
    buf_ptr = buf->image;

    for (plane=0; plane < 4; plane++) {
        vga_ptr = base_vga;

        outpw(GRACON_ADDR, read_mask[plane]);

        i=ysize;
        while (i--) {
            memcpy(buf_ptr, vga_ptr, xsize);
            vga_ptr += widthBytes;
            buf_ptr += xsize;
        }
    }

    read_plane = 3;
}


void
wide_bitblitX(COORD y, blitbuf *buf)
{
    short int bufsize;
    BYTE *vga_ptr;
    BYTE *buf_ptr;

    write_plane = 3;
    buf_ptr = buf->image;
    vga_ptr = RowsX[y] + activeStart;
    bufsize = (buf->ysize * widthBytes);

    outpw(SEQU_ADDR, PLANE_0);
    memcpy(vga_ptr, buf_ptr, bufsize);
    buf_ptr += bufsize;

    outpw(SEQU_ADDR, PLANE_1);
    memcpy(vga_ptr, buf_ptr, bufsize);
    buf_ptr += bufsize;

    outpw(SEQU_ADDR, PLANE_2);
    memcpy(vga_ptr, buf_ptr, bufsize);
    buf_ptr += bufsize;

    outpw(SEQU_ADDR, PLANE_3);
    memcpy(vga_ptr, buf_ptr, bufsize);
}


void
wide_getblitX(COORD y, blitbuf *buf)
{
    short int bufsize;
    BYTE *vga_ptr;
    BYTE *buf_ptr;

    read_plane = 3;
    buf_ptr = buf->image;
    vga_ptr = RowsX[y] + activeStart;
    bufsize = (buf->ysize * widthBytes);

    outpw(GRACON_ADDR, READ_PLANE_0);
    memcpy(buf_ptr, vga_ptr, bufsize);
    buf_ptr += bufsize;

    outpw(GRACON_ADDR, READ_PLANE_1);
    memcpy(buf_ptr, vga_ptr, bufsize);
    buf_ptr += bufsize;

    outpw(GRACON_ADDR, READ_PLANE_2);
    memcpy(buf_ptr, vga_ptr, bufsize);
    buf_ptr += bufsize;

    outpw(GRACON_ADDR, READ_PLANE_3);
    memcpy(buf_ptr, vga_ptr, bufsize);
}


void
save_blitbufPCX(char *fname, blitbuf *buf)
{
    FILE *fp;
    unsigned int i, size, temp_int;
    BYTE VGA_pal[768];
    BYTE *buf_ptr;
    BYTE temp_char, match, count;

    fp = fopen(fname, "wb");

    if (fp != NULL) {
        // Write manufacturer's byte
        temp_char = 10;
        fwrite(&temp_char, 1, 1, fp);

        // Write version of PCX.  5 = 256 color (PCX Version 5.0)
        temp_char = 5;
        fwrite(&temp_char, 1, 1, fp);

        // Write encoding type, always 1 for RLE.
        temp_char = 1;
        fwrite(&temp_char, 1, 1, fp);

        // Write bits_per_pixel = 8.
        temp_char = 8;
        fwrite(&temp_char, 1, 1, fp);

        // Write starting X and Y coords
        temp_int = 0;
        fwrite(&temp_int, 2, 1, fp);
        fwrite(&temp_int, 2, 1, fp);

        // Write X size
        temp_int = (buf->xsize - 1);
        fwrite(&temp_int, 2, 1, fp);

        // Write Y size
        temp_int = (buf->ysize - 1);
        fwrite(&temp_int, 2, 1, fp);

        // Do HRES and VRES **
        temp_int = buf->xsize;
        fwrite(&temp_int, 2, 1, fp);
        temp_int = buf->ysize;
        fwrite(&temp_int, 2, 1, fp);

        // Write 16 color palette, not used.
        temp_int = 0;
        i=24;
        while (i--) {
            fwrite(&temp_int, 2, 1, fp);
        }

        // Write vmode byte.
        temp_char = 0;
        fwrite(&temp_char, 1, 1, fp);

        // Write bit_planes
        temp_char = 1;
        fwrite(&temp_char, 1, 1, fp);

        // Write bytes_per_line
        temp_int = buf->xsize;
        fwrite(&temp_int, 2, 1, fp);

        // Write palette type
        temp_int = 1;
        fwrite(&temp_int, 2, 1, fp);

        // Write junk filler
        temp_int = 0;
        i=29;
        while (i--) {
            fwrite(&temp_int, 2, 1, fp);
        }

        // Write the actual image
        buf_ptr = buf->image;
        size = (buf->xsize * buf->ysize);

        count = 0;
        match = *buf_ptr;

        i=size;
        while (i--) {
            temp_char = *buf_ptr++;

            if ((temp_char == match) && (count < 63)) {
               count++;
            } else {
                if ((count == 1) && (match < 192)) {
                    // Write single byte
                    fwrite(&match,1,1,fp);
                } else {
                    // Write run of pixels
                    count += 192;
                    fwrite(&count, 1, 1, fp);
                    fwrite(&match, 1, 1, fp);
                }
                count = 1;
                match = temp_char;
            }
        }

        if ((count == 1) && (match < 192)) {
            // Write single byte
            fwrite(&match,1,1,fp);
        } else {
            // Write run of pixels
            count += 192;
            fwrite(&count, 1, 1, fp);
            fwrite(&match, 1, 1, fp);
        }

        // Write palette verification byte
        temp_char = 12;
        fwrite(&temp_char, 1, 1, fp);

        get_paletteX(VGA_pal);

        // Write 256 color palette
        fwrite(VGA_pal, 1, 768, fp);

        fclose(fp);
    }
}


int
load_blitbufPCX(char *fname, blitbuf *buf)
{
    FILE *fp;
    int size;
    BYTE VGA_pal[768];
    BYTE PCX_byte, RLE_byte;
    BYTE *buf_ptr;
    BYTE *end_of_buf;

    fp = fopen(fname, "rb");

    if (fp == NULL) {
        buf->xsize = 0;
        buf->ysize = 0;
        buf->image = NULL;
        return 0;
    } else {
        fseek(fp, 8, SEEK_SET);
        fread(&buf->xsize, 2, 1, fp);
        fread(&buf->ysize, 2, 1, fp);

        buf->xsize++;
        buf->ysize++;

        size = (buf->xsize * buf->ysize);

        buf->image = new BYTE[size];
        buf_ptr = buf->image;
        end_of_buf = buf_ptr + size;

        // Load 256 color PCX palette
        fseek(fp, -768, SEEK_END);
        fread(VGA_pal, 1, 768, fp);

        set_paletteX(VGA_pal);

        fseek(fp, 128, SEEK_SET);

        while (buf_ptr < end_of_buf) {
            // Read next packet
            fread(&PCX_byte, 1, 1, fp);

            if (PCX_byte < 192) {
                // Raw Pixel
                *buf_ptr++ = PCX_byte;
            } else {
                // RLE Pixels
                PCX_byte = PCX_byte & 0x3F;
                fread(&RLE_byte, 1, 1, fp);
                memset(buf_ptr, RLE_byte, PCX_byte);
                buf_ptr += PCX_byte;
            }
        }

        fclose(fp);
        return 1;
    }
}


void
scale_blitbuf(DIST dest_x, DIST dest_y, blitbuf *buf1, blitbuf *buf2)
{
    unsigned long ErrorAccX, ErrorAccY, ErrorAdjX, ErrorAdjY;
    DIST oldx, oldy, newx, newy;
    short int i, j, count;
    BYTE *src_base;
    BYTE *src_ptr;
    BYTE *dest_ptr;
    BYTE *newbuf;

    oldx = buf1->xsize;
    oldy = buf1->ysize;
    newx = dest_x;
    newy = dest_y;

    newbuf = new BYTE[newx * newy];

    src_base = buf1->image;
    dest_ptr = newbuf;

    // My bitmap scaling routine.  As you probably noticed, it's
    // pretty Bresenhammy!

    ErrorAccY = 0x8000;

    if (newx > oldx) {
        // Biggering
        ErrorAdjX = ((((unsigned long)newx) << 16) /
                     (((unsigned long)oldx)));

        ErrorAdjY = ((((unsigned long)newy) << 16) /
                     (((unsigned long)oldy)));

        i=oldy;
        while (i--) {
            ErrorAccX = 0x8000;
            src_ptr = src_base;

            j=oldx;
            while (j--) {
                ErrorAccX += ErrorAdjX;
                if (count = (ErrorAccX >> 16)) {
                    ErrorAccX &= 0xFFFFL;
                    while (count--) {
                        *dest_ptr++ = *src_ptr;
                    }
                }
                src_ptr++;
            }

            ErrorAccY += ErrorAdjY;
            count = (ErrorAccY >> 16) - 1;
            while (count--) {
                memcpy(dest_ptr, dest_ptr - newx, newx);
                dest_ptr += newx;
            }
            ErrorAccY &= 0xFFFFL;
            src_base += oldx;
        }
    } else {
        // Smallering
        ErrorAdjX = ((((unsigned long)oldx) << 16) /
                     (((unsigned long)newx)));

        ErrorAdjY = ((((unsigned long)oldy) << 16) /
                     (((unsigned long)newy)));

        i=newy;
        while (i--) {
            ErrorAccX = 0x8000;
            src_ptr = src_base;

            j=newx;
            while (j--) {
                *dest_ptr++ = *src_ptr;
                ErrorAccX += ErrorAdjX;
                src_ptr += (ErrorAccX >> 16);
                ErrorAccX &= 0xFFFFL;
            }

            ErrorAccY += ErrorAdjY;
            src_base += (oldx * (ErrorAccY >> 16));
            ErrorAccY &= 0xFFFFL;
        }
    }

    if (buf2 == NULL) {
        delete buf1->image;
        buf1->xsize = newx;
        buf1->ysize = newy;
        buf1->image = newbuf;
    } else {
        if (buf2->image != NULL) {
            delete buf2->image;
        }
        buf2->xsize = newx;
        buf2->ysize = newy;
        buf2->image = newbuf;
    }
}


void
vertical_scale_blitbuf(DIST dest_y, blitbuf *buf1, blitbuf *buf2)
{
    unsigned long ErrorAccY, ErrorAdjY;
    DIST xsize, oldy, newy;
    short int i, count;
    BYTE *src_ptr;
    BYTE *dest_ptr;
    BYTE *newbuf;

    xsize = buf1->xsize;
    oldy = buf1->ysize;
    newy = dest_y;

    newbuf = new BYTE[xsize * newy];

    src_ptr = buf1->image;
    dest_ptr = newbuf;

    // My bitmap scaling routine.  As you probably noticed, it's
    // pretty Bresenhammy!

    ErrorAccY = 0x8000;
    ErrorAdjY = ((((unsigned long)newy) << 16) /
                 (((unsigned long)oldy)));

    if (newy >= oldy) {
        // Biggering
        i=oldy;
        while (i--) {
            ErrorAccY += ErrorAdjY;
            if (count = (ErrorAccY >> 16)) {
                ErrorAccY &= 0xFFFFL;

                while (count--) {
                    memcpy(dest_ptr, src_ptr, xsize);
                    dest_ptr += xsize;
                }
            }

            src_ptr += xsize;
        }
    } else {
        // Smallering
        i=oldy;
        while (i--) {
            ErrorAccY += ErrorAdjY;
            if (ErrorAccY & ~0xFFFFL) {
                ErrorAccY &= 0xFFFFL;
                memcpy(dest_ptr, src_ptr, xsize);
                dest_ptr += xsize;
            }

            src_ptr += xsize;
        }
    }

    if (buf2 == NULL) {
        delete buf1->image;
        buf1->ysize = newy;
        buf1->image = newbuf;
    } else {
        if (buf2->image != NULL) {
            delete buf2->image;
        }
        buf2->xsize = xsize;
        buf2->ysize = newy;
        buf2->image = newbuf;
    }
}


void
greyscale_blitbuf(blitbuf *buf)
{
    BYTE temp_pal[768];
    BYTE *buf_ptr;
    BYTE *temp;
    BYTE r, g;
    unsigned int i;

    buf_ptr = buf->image;

    get_paletteX(temp_pal, 0);

    for (i = (buf->xsize * buf->ysize); i; i--) {
        temp = temp_pal + ((*buf_ptr) * 3);
        r = *temp++;
        g = *temp++;

        *buf_ptr++ = ((r * 19) + (g * 37) + (*temp << 3)) >> 6;
    }
}


void
RGB_blitbuf(blitbuf *buf)
{
    BYTE temp_pal[768];
    BYTE *buf_ptr;
    BYTE *temp;
    BYTE r, g, b;
    unsigned int i;

    buf_ptr = buf->image;

    get_paletteX(temp_pal, 0);

    for (i = (buf->xsize * buf->ysize); i; i--) {
        temp = temp_pal + ((*buf_ptr) * 3);
        r = (*temp) + 4;
        temp++;
        g = (*temp) + 4;
        temp++;
        b = (*temp) + 8;

        *buf_ptr++ = ((r >> 3) << 5) + ((g >> 3) << 2) + (b >> 4);
    }
}


void
flip_vertical_blitbuf(blitbuf *buf)
{
    BYTE *top;
    BYTE *bottom;
    BYTE *temp;
    DIST i, x, y;;

    x = buf->xsize;
    y = buf->ysize;

    temp = new BYTE[x];

    top = buf->image;
    bottom = buf->image + (x * (y-1));

    i = (y >> 1);
    while (i--) {
        memcpy(temp, top, x);
        memcpy(top, bottom, x);
        memcpy(bottom, temp, x);
        top += x;
        bottom -= x;
    }

    delete temp;
}


void
flip_horizontal_blitbuf(blitbuf *buf)
{
    BYTE *buf_ptr;
    BYTE *temp_ptr;
    BYTE *temp;
    DIST i, j, x;

    x = buf->xsize;

    temp = new BYTE[x];

    buf_ptr = buf->image;

    i = buf->ysize;
    while (i--) {
        memcpy(temp, buf_ptr, x);
        temp_ptr = temp + (x - 1);
        j=x;
        while (j--) {
            *buf_ptr++ = *temp_ptr--;
        }
    }

    delete temp;
}


void
brighten_blitbuf(SBYTE factor, blitbuf *buf)
{
    BYTE *buf_ptr;
    short int scratch;
    unsigned int i;

    buf_ptr = buf->image;

    for (i = (buf->xsize * buf->ysize); i; i--) {
        scratch = (*buf_ptr + factor);
        if (scratch <= 0) {
            *buf_ptr++ = 0;
        } else if (scratch >= 63) {
            *buf_ptr++ = 63;
        } else {
            *buf_ptr++ = scratch;
        }
    }
}


void
stretch_blitbuf(BYTE factor, blitbuf *buf)
{
    BYTE *buf_ptr;
    short int scratch;
    unsigned int i;

    buf_ptr = buf->image;

    for (i = (buf->xsize * buf->ysize); i; i--) {
        scratch = ((((*buf_ptr - 32) * factor) + 8) >> 4) + 32;
        if (scratch <= 0) {
            *buf_ptr++ = 0;
        } else if (scratch >= 63) {
            *buf_ptr++ = 63;
        } else {
            *buf_ptr++ = scratch;
        }
    }
}


void
pixelize(DIST pixfactor, blitbuf *buf)
{
    // Do nothing
}


void
GREYconvolve_blitbuf(BYTE *kernel, blitbuf *buf)
{
    // Do nothing
}


void
RGBconvolve_blitbuf(BYTE *kernel, blitbuf *buf)
{
    // Do nothing
}


void
scale_scanline(BYTE *source, BYTE *dest, DIST smap_size, DIST dmap_size,
    DIST dline_size)
{
    unsigned long ErrorAcc, ErrorAdj;
    short int i, temp, invert;

    ErrorAcc = 0x8000;

    // Prepare for backwards scanlines
    if (dline_size >= 0) {
        invert = 0;
    } else {
        invert = 1;
        dline_size = -dline_size;
    }

    if (dline_size > smap_size) {
        // Biggering
        if (smap_size == 0) {
            return;
        }
        ErrorAdj = ((((unsigned long)dline_size) << 16) /
                    (((unsigned long)smap_size)));

        i=smap_size;
        while (i--) {
            ErrorAcc += ErrorAdj;
            temp = (ErrorAcc >> 16);
            ErrorAcc &= 0xFFFFL;
            while (temp--) {
                *dest++ = *source;
            }
            source++;
        }
    } else {
        // Smallering
        if (dline_size == 0) {
            memset(dest, 0, dmap_size);
        } else {
            temp = dmap_size - dline_size;
            i = temp >> 1;
            temp -= i;
            while (i--) {
                *dest++ = 0;
            }

            ErrorAdj = ((((unsigned long)smap_size) << 16) /
                        (((unsigned long)dline_size)));

            i=dline_size;

            while (i--) {
                *dest++ = *source;
                ErrorAcc += ErrorAdj;
                source += (ErrorAcc >> 16);
                ErrorAcc &= 0xFFFFL;
            }

            while (temp--) {
                *dest++ = 0;
            }
        }
    }
}


int
load_blitbufRAW(char *rawname, char *palname, blitbuf *buf)
{
    FILE *fp;
    BYTE VGA_pal[768];

    fp = fopen(rawname, "rb");

    if (fp == NULL) {
        buf->xsize = 0;
        buf->ysize = 0;
        buf->image = NULL;
        return 0;
    } else {
        buf->xsize = 320;
        buf->ysize = 200;
        buf->image = new BYTE[64000L];

        // Load image
        fread(buf->image, 64000L, 1, fp);

        if (palname == NULL) {
            fread(VGA_pal, 1, 768, fp);
            set_paletteX(VGA_pal);
            fclose(fp);
        } else {
            fclose(fp);
            fp = fopen(palname, "rb");
            if (fp != NULL) {
                fread(VGA_pal, 1, 768, fp);
                set_paletteX(VGA_pal);
                fclose(fp);
            }
        }

        return 1;
    }
}

