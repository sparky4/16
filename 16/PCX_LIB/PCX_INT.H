/* 
 *************************************************************************
 *
 *  PCX_INT.H - PCX_LIB Library Internal Definitions Include File
 *
 *  Version:    1.00B
 *
 *  History:    91/02/14 - Created
 *              91/04/01 - Release 1.00A
 *              91/04/07 - Release 1.00B
 *
 *  Compiler:   Microsoft C V6.0
 *
 *  Author:     Ian Ashdown, P.Eng.
 *              byHeart Software
 *              620 Ballantree Road
 *              West Vancouver, B.C.
 *              Canada V7S 1W3
 *              Tel. (604) 922-6148
 *              Fax. (604) 987-7621
 *
 *  Copyright:  Public Domain
 *
 *************************************************************************
 */

/*      DEFINITIONS                                                     */

#define _PCX_INT_H      1

#ifndef _PCX_EXT_H
#include "PCX_EXT.H"    /* Get external PCX definitions, etc.           */
#endif

#define PCX_HERC        0xff    /* Hercules 720 x 348 monochrome mode   */

#define PCX_COMP_FLAG   0xc0    /* Compressed data flag mask            */
#define PCX_COMP_MASK   0x3f    /* Data repeat count mask               */

#define PCX_PAL_MASK    0x03    /* Palette interpretation mask          */
#define PCX_EPAL_FLAG   0x0c    /* Extended palette flag                */

#define PCX_PAL_SIZE    16      /* File header palette size             */
#define PCX_EPAL_SIZE   256     /* Extended palette size                */

#define PCX_MAXLINESZ   640     /* Maximum PCX line buffer size         */

/* Color graphics adapter color palette macros                          */

#define PCX_CGA_BKGND(x)        (x[0].red >> 4)    /* Background color  */
#define PCX_CGA_BURST(x)        (x[1].red & 0x80)  /* Color burst       */
#define PCX_CGA_SELECT(x)       (x[1].red & 0x40)  /* Palette selector  */
#define PCX_CGA_INTENSITY(x)    (x[1].red & 0x20)  /* Intensity         */

typedef unsigned char BYTE;     /* 8-bit data type                      */
typedef unsigned int WORD;      /* 16-bit data type                     */

/*      STRUCTURE DECLARATIONS                                          */

typedef struct pcx_pal  /* PCX palette array element                    */
{
  BYTE red;             /* Red intensity                                */
  BYTE green;           /* Green intensity                              */
  BYTE blue;            /* Blue intensity                               */
}
PCX_PAL;

typedef struct pcx_hdr  /* PCX file header (Version 5)                  */
{
  BYTE pcx_id;          /* Always 0x0a for PCX files                    */
  BYTE version;         /* Version number                               */
  BYTE encoding;        /* 1 = PCX run length encoding                  */
  BYTE bppixel;         /* Number of bits/pixel per color plane         */
  WORD xul;             /* X-position of upper left corner              */
  WORD yul;             /* Y-position of upper left corner              */
  WORD xlr;             /* X-position of lower right corner             */
  WORD ylr;             /* Y-position of lower right corner             */
  WORD horz_res;        /* Horizontal resolution                        */
  WORD vert_res;        /* Vertical resolution                          */
  PCX_PAL palette[PCX_PAL_SIZE];        /* Hardware R-G-B palette       */
  BYTE reserved;        /* Unused in Version 5                          */
  BYTE nplanes;         /* Number of color planes                       */
  WORD bppscan;         /* Number of bytes per plane scan line          */
  WORD palette_type;    /* Palette interpretation                       */
  WORD scrn_width;      /* Horizontal screen size in pixels             */
  WORD scrn_height;     /* Vertical screen size in pixels               */
  BYTE filler[54];      /* Padding to fill out 128-byte header          */

  /* Notes:                                                             */
  /*                                                                    */
  /*   1.  The "version" member may be one of the following:            */
  /*                                                                    */
  /*         0 - PC Paintbrush Version 2.5                              */
  /*         2 - PC Paintbrush Version 2.8 (with palette information)   */
  /*         3 - PC Paintbrush Version 2.8 (w/o palette information)    */
  /*         4 - PC Paintbrush for Windows (PC Paintbrush Plus for      */
  /*             Windows and Windows 3.0 Paintbrush use Version 5)      */
  /*         5 - PC Paintbrush 3.0 and greater (including PC Paintbrush */
  /*             Plus and Publisher's Paintbrush)                       */
  /*                                                                    */
  /*   2.  ZSoft Corporation has reserved the right to change the       */
  /*       encoding method in future versions for better image          */
  /*       compression performance.  The "encoding" member value may    */
  /*       change accordingly.                                          */
  /*                                                                    */
  /*   3.  The value of the "bppixel" member depends on the type of     */
  /*       video display adapter and its video mode.  Typical values    */
  /*       are 1, 2, 4 and 8.                                           */
  /*                                                                    */
  /*   4.  The "xul", "yul", "xlr" and "ylr" members are zero-based and */
  /*       and inclusive values indicating the position of the image on */
  /*       the screen.  The display functions can ignore this           */
  /*       information if desired.                                      */
  /*                                                                    */
  /*   5.  The "horz_res" and "vert_res" members refer to the "dots per */
  /*       inch" resolution of the scanning device used to create the   */
  /*       image.  For images created on video display adapters, these  */
  /*       values typically refer to the horizontal and vertical        */
  /*       resolutions in pixels (e.g. - 640 x 350 for an EGA display). */
  /*                                                                    */
  /*       The display function ignore these members, as some programs  */
  /*       programs do not bother to initialize them when creating PCX  */
  /*       image files.                                                 */
  /*                                                                    */
  /*   6.  The "palette" member is typically left uninitialized if an   */
  /*       extended 256-color palette is appended to the PCX image      */
  /*       file.                                                        */
  /*                                                                    */
  /*   7.  The "reserved" member used to contain the MS-DOS video mode  */
  /*       that the PCX image was intended to be displayed under.  This */
  /*       member is ignored in Version 5.  ZSoft recommends that it be */
  /*       set to zero.                                                 */
  /*                                                                    */
  /*   8.  The value of the "nplanes" member depends on the type of     */
  /*       video display adapter and its video mode.  Typical values    */
  /*       are 1, 2, 3 and 4.                                           */
  /*                                                                    */
  /*   9.  The value of the "bppscan" member should be an even number   */
  /*       (for compatibility with some existing commercial programs.)  */
  /*       It indicates the number of bytes required to buffer a        */
  /*       decoded scan line for one color plane.                       */
  /*                                                                    */
  /*  10.  The "palette_type" member indicates whether the palette      */
  /*       represents a color or grayscale palette.  It is a bit-mapped */
  /*       variable (only the lowest two bits are significant; the      */
  /*       others are ignored) with two possible values:                */
  /*                                                                    */
  /*         0x01 - color or black & white                              */
  /*         0x02 - grayscale                                           */
  /*                                                                    */
  /*       If "grayscale" is indicated, the file color palette must be  */
  /*       set to shades of gray.  The file decoding functions can then */
  /*       either use this palette or assume a default grayscale        */
  /*       palette if necessary.                                        */
  /*                                                                    */
  /*       PC Paintbrush IV and IV Plus ignore this member.             */
  /*                                                                    */
  /*  11.  The "scrn_width" and "scrn_height" members were added for    */
  /*       PC Paintbrush IV Version 1.02 and IV Plus Version 1.0.  They */
  /*       may not be initialized in some older Version 5 PCX files, or */
  /*       the "scrn_width" member may be initialized with the screen   */
  /*       height and the "scrn_height" member uninitialized.  ZSoft    */
  /*       recommends that this information be ignored.                 */
  /*                                                                    */
  /*  12.  ZSoft recommends that the "filler" bytes be set to zero.     */
}
PCX_HDR;

typedef struct pcx_workblk      /* PCX image file workblock             */
{
  /* File header                                                        */

  FILE *fp;                     /* PCX image file pointer               */
  PCX_HDR header;               /* PCX image file header                */
  PCX_PAL *palettep;            /* Color palette pointer                */
  BOOL epal_flag;               /* Extended color palette flag          */

  /* Image manipulation variables                                       */

  int num_bytes;                /* Number of bytes to display           */
  int mask;                     /* Unseen pixels mask                   */
  unsigned long page_offset;    /* Display page address offset          */

  /* Image manipulation function pointer                                */

  void (*pcx_funcp)(struct pcx_workblk *, unsigned char _far *, int);
}
PCX_WORKBLK;

/* FUNCTIONS PROTOTYPES                                                 */

extern BOOL pcx_close(PCX_WORKBLK *);

extern PCX_WORKBLK *pcx_open(char *, BOOL);

