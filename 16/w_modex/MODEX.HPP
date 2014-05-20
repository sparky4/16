#ifndef MODEX_HPP
    #define MODEX_HPP

#include "xtypes.hpp"

// Some defines
#define PLANE_0         0x0102
#define PLANE_1         0x0202
#define PLANE_2         0x0402
#define PLANE_3         0x0802
#define ALL_PLANES      0x0F02
#define READ_PLANE_0    0x0004
#define READ_PLANE_1    0x0104
#define READ_PLANE_2    0x0204
#define READ_PLANE_3    0x0304


// External Variables needed by graphics routines
extern unsigned short width, height, widthBytes, num_pages;
extern unsigned short activeStart, visibleStart, pageSize;
extern unsigned char write_plane, read_plane;
extern unsigned char *RowsX[600];
extern unsigned char line_head[4];
extern unsigned char line_tail[4];
extern unsigned short plane_mask[4];
extern unsigned short read_mask[4];
extern unsigned short text_mask[16];
extern unsigned short page_offset[5];
extern unsigned short page_mask_high[5];
extern unsigned short page_mask_low[5];


// Return to text mode
void set80x25(void);

// Set various ModeX resolutions
void set256x224x256_X(void);
void set256x240x256_X(void);
void set256x256x256_X(void);
void set256x480x256_X(void);
void set320x200x256_X(void);
void set320x240x256_X(void);
void set320x400x256_X(void);
void set320x480x256_X(void);
void set360x200x256_X(void);
void set360x240x256_X(void);
void set360x270x256_X(void);
void set360x360x256_X(void);
void set360x400x256_X(void);
void set360x480x256_X(void);
void set376x282x256_X(void);
void set376x564x256_X(void);
void set400x300x256_X(void);
void set400x600x256_X(void);

COORD get_xres(void);
COORD get_yres(void);

void set_write_plane(unsigned short int plane_mask);
void set_read_plane(unsigned short int plane_mask);

#endif

