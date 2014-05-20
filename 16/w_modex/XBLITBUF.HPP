#ifndef X_BLITBUF_HPP
    #define X_BLITBUF_HPP

// Basic BlitBuf functions
void clear_blitbuf(blitbuf *buf);
void fill_blitbuf(BYTE color, blitbuf *buf);
void alloc_blitbuf(blitbuf *buf, DIST xsize, DIST ysize);
void lin_2_pln_blitbuf(blitbuf *buf);
void pln_2_lin_blitbuf(blitbuf *buf);

// Adjust color in blitbuf
void greyscale_blitbuf(blitbuf *buf);
void RGB_blitbuf(blitbuf *buf);
void brighten_blitbuf(SBYTE factor, blitbuf *buf);
void stretch_blitbuf(BYTE factor, blitbuf *buf);  // factor = 1/16 units

// Image processing: These expect a linear blitbuf
void scale_blitbuf(DIST dest_x, DIST dest_y,
    blitbuf *buf1, blitbuf *buf2 = NULL);
void vertical_scale_blitbuf(DIST dest_y, blitbuf *buf1, blitbuf *buf2 = NULL);
void flip_vertical_blitbuf(blitbuf *buf);
void flip_horizontal_blitbuf(blitbuf *buf);
void pixelize(DIST pixfactor, blitbuf *buf);  // Not written yet
void scale_scanline(BYTE *source, BYTE *dest, DIST smap_size,
    DIST dmap_size, DIST dline_size);

// Image Convolution by a 3x3 kernel (linear)
void GREYconvolve_blitbuf(BYTE *kernel, blitbuf *buf);  // Not written yet
void RGBconvolve_blitbuf(BYTE *kernel, blitbuf *buf);   // Not written yet

// Vanilla blits can be any size, anywhere
void vanilla_bitblitX(COORD x, COORD y, blitbuf *buf);
void vanilla_getblitX(COORD x, COORD y, blitbuf *buf);

// Transparent blits must be aligned and 0 is the "transparent" value
void transparent_bitblitX(COORD x, COORD y, blitbuf *buf);

// Aligned blits must have X and XSIZE evenly divisible by 4
void aligned_bitblitX(COORD x, COORD y, blitbuf *buf);
void aligned_getblitX(COORD x, COORD y, blitbuf *buf);

// Planar blits must be aligned and also be ordered in 4-planar fashion
void planar_bitblitX(COORD x, COORD y, blitbuf *buf);
void planar_getblitX(COORD x, COORD y, blitbuf *buf);

// Wide blits must be planar and are assumed to be as wide as the screen
void wide_bitblitX(COORD y, blitbuf *buf);
void wide_getblitX(COORD y, blitbuf *buf);

// PCX functions
void save_blitbufPCX(char *fname, blitbuf *buf);
int  load_blitbufPCX(char *fname, blitbuf *buf);

// RAW file functions (320x200 only)
int  load_blitbufRAW(char *rawname, char *palname, blitbuf *buf);

#endif

