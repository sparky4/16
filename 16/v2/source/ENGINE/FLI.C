/*
Copyright (C) 1998 BJ Eirich (aka vecna)
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public Lic
See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

// ported by aen

#define FLI_H
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "verge.h"

// ================================= Data ====================================

#ifndef __WATCOMC__
#define INT_MAX 0xFFFFFFFF
#define INT_MIN 0
#endif

#ifndef MIN
#define MIN(x,y)     (((x) < (y)) ? (x) : (y))
#define MAX(x,y)     (((x) > (y)) ? (x) : (y))
#define MID(x,y,z)   MAX((x), MIN((y), (z)))
#endif

#ifndef ABS
#define ABS(x)       (((x) >= 0) ? (x) : (-(x)))
#endif

#ifndef SGN
#define SGN(x)       (((x) >= 0) ? 1 : -1)
#endif

#define FLI_MAGIC            0xAF11      // file header magic number
#define FLC_MAGIC            0xAF12      // file magic number (Pro)
#define FLI_FRAME_MAGIC      0xF1FA      // frame header magic number
#define FLI_FRAME_PREFIX     0xF100      // FLC's prefix info
#define FLI_FRAME_USELESS    0x00A1      // FLC's garbage frame

typedef struct
{
  int w,h;
  byte *data;
} BITMAP;

#define FLI_OK          0              /* FLI player return values */
#define FLI_EOF         -1
#define FLI_ERROR       -2
#define FLI_NOT_OPEN    -3

#ifdef __WATCOMC__
#define PACKED
#pragma pack(1);
#endif

#ifdef __DJGPP__
#define PACKED __attribute__ ((packed))
#endif

typedef struct
{
  long size            PACKED;
  word type            PACKED;
  word frame_count     PACKED;
  word width           PACKED;
  word height          PACKED;
  word bpp             PACKED;
  word flags           PACKED;
  word speed           PACKED;
  long next_head       PACKED;
  long frames_in_table PACKED;
  char reserved[102]   PACKED;
} FLI_HEADER;

typedef struct
{
  quad size            PACKED;
  word type            PACKED;
  word chunks          PACKED;
  char pad[8]          PACKED;
} FLI_FRAME;

typedef struct
{
  quad size            PACKED;
  word type            PACKED;
} FLI_CHUNK;

#ifdef __WATCOMC__
#pragma pack();
#endif

static int fli_status = FLI_NOT_OPEN;  // current state of the FLI player

int fli_bitmap_width = 0;
int fli_bitmap_height = 0;
byte *fli_bitmap_data = NULL;          // current frame of the FLI
byte fli_palette[768];                   // current palette the FLI is using

int fli_bmp_dirty_from = INT_MAX;      // what part of fli_bitmap is dirty
int fli_bmp_dirty_to = INT_MIN;
int fli_pal_dirty_from = INT_MAX;      // what part of fli_palette is dirty
int fli_pal_dirty_to = INT_MIN;

int fli_frame = 0;                     // current frame number in the FLI
long speed;

static int fli_file = 0;               // the file we are reading

static byte *fli_mem_data = NULL;      // the memory FLI we are playing
static int fli_mem_pos = 0;            // position in the memory FLI

static FLI_HEADER fli_header;          // header structure
static FLI_FRAME frame_header;         // frame header structure

// a block of temporary working memory
byte *_scratch_mem = NULL;
int _scratch_mem_size = 0;

// ================================= Code ====================================

void reset_fli_variables(void);
void close_fli(void);
int  next_fli_frame(int);
int  open_fli(char *);
int  open_memory_fli(byte *);

void _grow_scratch_mem(int size)
{
  if (size > _scratch_mem_size) {
    size = (size+1023) & 0xFFFFFC00;
    _scratch_mem = realloc(_scratch_mem, size);
    _scratch_mem_size = size;
  }
}

void vsync()
{
  while (!(inp(986) & 8));
}

void set_palette_range(byte *pal, int from, int to, int sync)
{
  int i;

  // swap?
  if (to<from) { int t=from; from=to; to=t; }

  // keep valid
  if (from<0) from=0;
  if (to>=256) to=255;

  if (sync) vsync();

  outp(968, from);
  for (i=from; i<=to; i++)
  {
    outp(969, pal[(i*3)+0]);
    outp(969, pal[(i*3)+1]);
    outp(969, pal[(i*3)+2]);
  }
}

static byte *fli_read(byte *buf, int size)
{
  int result;

  if (fli_mem_data)
  {
    if (buf)
      memcpy(buf, fli_mem_data+fli_mem_pos, size);
    else
      buf = fli_mem_data+fli_mem_pos;

    fli_mem_pos += size;
  }
  else
  {
    if (!buf)
    {
      _grow_scratch_mem(size);
      buf = _scratch_mem;
    }

    result = read(fli_file, buf, size);
    if (result != size)
      return NULL;
  }
  return buf;
}

static void fli_seek(int offset, int mode)
{
  if (fli_mem_data)
  {
    if (mode == SEEK_CUR)
      fli_mem_pos += offset;
    else
      fli_mem_pos = offset;
  }
  else
    lseek(fli_file, offset, mode);
}

// helpers for reading FLI chunk data
#define READ_BYTE()  ((sz-- > 0)    ? *(((byte *)p)++) : 0)
#define READ_CHAR()  ((sz-- > 0)    ? *(((signed char  *)p)++) : 0)
#define READ_SHORT() (((sz-=2) > 0) ? *(((signed short *)p)++) : 0)

#define READ_BLOCK(pos, size)             \
  {                                       \
    if (sz >= (size))                     \
    {                                     \
      memcpy(pos, p, size);               \
      p += (size);                        \
      sz -= (size);                       \
    }                                     \
    else                                  \
    {                                     \
      char *_p = (char *)(pos);           \
      int _c;                             \
                                          \
      for (_c=0; _c < (size); _c++)       \
        *(_p++) = READ_BYTE();            \
    }                                     \
  }

#define READ_RLE_BYTE(pos, size)          \
  {                                       \
    memset(pos, READ_BYTE(), size);       \
  }

#define READ_RLE_WORD(pos, size)          \
  {                                       \
    short *_p = (short *)(pos);           \
    short _v = READ_SHORT();              \
    int _c;                               \
                                          \
    for (_c=0; _c < (size); _c++)         \
      *(_p++) = _v;                       \
  }

static void do_fli_256_color(byte *p, int sz)
{
  int packets;
  int c, c2;
  int offset;
  int length;

  offset = 0;
  packets = READ_SHORT();
  for (c=0; c<packets; c++)
  {
    offset += READ_BYTE();
    length = READ_BYTE();
    if (length == 0) length = 256;
    for (c2=0; c2<length; c2++)
    {
      fli_palette[((offset+c2)*3)+0] = READ_BYTE() / 4;
      fli_palette[((offset+c2)*3)+1] = READ_BYTE() / 4;
      fli_palette[((offset+c2)*3)+2] = READ_BYTE() / 4;
    }
    fli_pal_dirty_from = MIN(fli_pal_dirty_from, offset);
    fli_pal_dirty_to = MAX(fli_pal_dirty_to, offset+length-1);
    offset += length;
  }
}

static void do_fli_delta(byte *p, int sz)
{
  int lines;
  int packets;
  int size;
  int x, y;

  y = 0;
  lines = READ_SHORT();

  // for each line...
  while (lines-- > 0)
  {
    packets = READ_SHORT();

    while (packets < 0)
    {
      if (packets & 0x4000)
        y -= packets;
      else
        fli_bitmap_data[(y*fli_bitmap_width)+fli_bitmap_width-1]=packets&0xff;
        //fli_bitmap->line[y][fli_bitmap->w-1] = packets & 0xFF;

      packets = READ_SHORT();
    }
    x=0;
    while (packets-- > 0)
    {
      // skip bytes
      x += READ_BYTE();

      size = READ_CHAR();

      // copy size words
      if (size > 0)
      {
        READ_BLOCK(fli_bitmap_data+(y*fli_bitmap_width)+x, size*2);
        //READ_BLOCK(fli_bitmap->line[y]+x, size*2);
        x += size*2;
      }
      // repeat word -size times
      else if (size < 0)
      {
        READ_RLE_WORD(fli_bitmap_data+(y*fli_bitmap_width)+x, -size);
        //READ_RLE_WORD(fli_bitmap->line[y]+x, -size);
        x -= size*2;
      }
    }
    fli_bmp_dirty_from = MIN(fli_bmp_dirty_from, y);
    fli_bmp_dirty_to = MAX(fli_bmp_dirty_to, y);
    y++;
  }
}

static void do_fli_color(byte *p, int sz)
{
  int packets;
  int c, c2;
  int offset;
  int length;

  offset = 0;
  packets = READ_SHORT();

  for (c=0; c<packets; c++)
  {
    offset += READ_BYTE();
    length = READ_BYTE();
    if (length == 0) length = 256;

    for (c2=0; c2<length; c2++)
    {
      fli_palette[((offset+c2)*3)+0] = READ_BYTE();
      fli_palette[((offset+c2)*3)+1] = READ_BYTE();
      fli_palette[((offset+c2)*3)+2] = READ_BYTE();
    }

    fli_pal_dirty_from = MIN(fli_pal_dirty_from, offset);
    fli_pal_dirty_to = MAX(fli_pal_dirty_to, offset+length-1);
    offset += length;
  }
}

static void do_fli_lc(byte *p, int sz)
{
  int lines;
  int packets;
  int size;
  int x, y;

  y = READ_SHORT();
  lines = READ_SHORT();
  fli_bmp_dirty_from = MIN(fli_bmp_dirty_from, y);
  fli_bmp_dirty_to = MAX(fli_bmp_dirty_to, y+lines-1);

  // for each line...
  while (lines-- > 0)
  {
    packets = READ_BYTE();
    x = 0;
    while (packets-- > 0)
    {
      x += READ_BYTE();
      size = READ_CHAR();

      // copy size bytes
      if (size > 0)
      {
        READ_BLOCK(fli_bitmap_data+(y*fli_bitmap_width)+x, size);
        //READ_BLOCK(fli_bitmap->line[y]+x, size);
        x += size;
      }
      else if (size < 0)
      {
        READ_RLE_BYTE(fli_bitmap_data+(y*fli_bitmap_width)+x, -size);
        //READ_RLE_BYTE(fli_bitmap->line[y]+x, -size);
        x -= size;
      }
    }
    y++;
  }
}

static void do_fli_black(byte *p, int sz)
{
  memset(fli_bitmap_data, 0, fli_bitmap_width*fli_bitmap_height);
  //clear(fli_bitmap);

  fli_bmp_dirty_from = 0;
  fli_bmp_dirty_to = fli_bitmap_height-1;
}

static void do_fli_brun(byte *p, int sz)
{
  int packets;
  int size;
  int x, y;

  // for each line...
  for (y=0; y<fli_bitmap_height; y++)
  {
    packets = READ_BYTE();
    x = 0;
    while (packets-- > 0)
    {
      size = READ_CHAR();
      if (size < 0)
      {
        READ_BLOCK(fli_bitmap_data+(y*fli_bitmap_width)+x, -size);
        //READ_BLOCK(fli_bitmap->line[y]+x, -size);
        x -= size;
      }
      // repeat byte size times
      else if (size > 0)
      {
        READ_RLE_BYTE(fli_bitmap_data+(y*fli_bitmap_width)+x, size);
        //READ_RLE_BYTE(fli_bitmap->line[y]+x, size);
        x += size;
      }
    }
  }
  fli_bmp_dirty_from = 0;
  fli_bmp_dirty_to = fli_bitmap_height-1;
}

static void do_fli_copy(byte *p, int sz)
{
  READ_BLOCK(fli_bitmap_data, fli_bitmap_width * fli_bitmap_height);
  //READ_BLOCK(fli_bitmap->dat, fli_bitmap->w * fli_bitmap->h);

  fli_bmp_dirty_from = 0;
  fli_bmp_dirty_to = fli_bitmap_height-1;
}

static void read_frame()
{
  byte *p;
  FLI_CHUNK *chunk;
  int c, sz;

  if (fli_status != FLI_OK)
    return;

get_another_frame:
  if (!fli_read((byte *)&frame_header, sizeof(FLI_FRAME)))
  {
    fli_status = FLI_ERROR;
    return;
  }

  if ((frame_header.type == FLI_FRAME_PREFIX)
  || (frame_header.type == FLI_FRAME_USELESS))
  {
    fli_seek(frame_header.size-sizeof(FLI_FRAME), SEEK_CUR);
    fli_frame++;

    goto get_another_frame;
  }

  if (frame_header.type != FLI_FRAME_MAGIC)
  {
    fli_status = FLI_ERROR;
    return;
  }

  if (frame_header.size == sizeof(FLI_FRAME))
  {
    fli_frame++;
    return;
  }
  p = fli_read(NULL, frame_header.size-sizeof(FLI_FRAME));
  if (!p)
  {
    fli_status = FLI_ERROR;
    return;
  }
  for (c=0; c<frame_header.chunks; c++)
  {
    chunk = (FLI_CHUNK *)p;
    sz = chunk->size - sizeof(FLI_CHUNK);
    p += sizeof(FLI_CHUNK);
    switch (chunk->type)
    {
      case 4:  do_fli_256_color(p, sz); break;
      case 7:  do_fli_delta(p, sz); break;
      case 11: do_fli_color(p, sz); break;
      case 12: do_fli_lc(p, sz); break;
      case 13: do_fli_black(p, sz); break;
      case 15: do_fli_brun(p, sz); break;
      case 16: do_fli_copy(p, sz); break;
      default: err("Corrupt FLI chunk.");
    }
    p = ((byte *)chunk) + chunk->size;
  }
  fli_frame++;
}

static int do_play_fli(BITMAP *bmp, int loop, int (*callback)())
{
  int ret;

  ret = next_fli_frame(loop);
  while (ret == FLI_OK)
  {
    if (fli_pal_dirty_from <= fli_pal_dirty_to)
      set_palette_range(fli_palette, fli_pal_dirty_from, fli_pal_dirty_to, 1);

    if (fli_bmp_dirty_from <= fli_bmp_dirty_to)
    {
      int i,to=fli_bmp_dirty_to;
      if (to>=bmp->h) to=bmp->h-1;

//      vsync();
      for (i=fli_bmp_dirty_from; i<=to; i++)
      {
        memcpy(bmp->data+(i*bmp->w), fli_bitmap_data+(i*fli_bitmap_width),
          (fli_bitmap_width>bmp->w) ? bmp->w : fli_bitmap_width);
      }

      //blit(
      //  fli_bitmap, bmp, 0, fli_bmp_dirty_from, 0, fli_bmp_dirty_from,
      //  fli_bitmap->w, 1+fli_bmp_dirty_to-fli_bmp_dirty_from);
    }

    reset_fli_variables();

    if (callback)
    {
      ret = (*callback)();
      if (ret != FLI_OK)
        break;
    }
    ret = next_fli_frame(loop);
    while (timer_count<speed) ShowPage();
    timer_count=0;
  }

  close_fli();

  return (ret == FLI_EOF) ? FLI_OK : ret;
}

int play_fli(char *filename, BITMAP *bmp, int loop, int (*callback)())
{
  if (open_fli(filename) != FLI_OK)
    return FLI_ERROR;

  return do_play_fli(bmp, loop, callback);
}

int play_memory_fli(byte *fli_data, BITMAP *bmp, int loop, int (*callback)())
{
  if (open_memory_fli(fli_data) != FLI_OK)
    return FLI_ERROR;

  return do_play_fli(bmp, loop, callback);
}

static int do_open_fli()
{
  // read the header
  if (!fli_read((byte *)&fli_header, sizeof(FLI_HEADER)))
  {
    close_fli();
    return FLI_ERROR;
  }

  // check magic numbers
  if (((fli_header.bpp != 8) && (fli_header.bpp != 0))
  || ((fli_header.type != FLI_MAGIC) && (fli_header.type != FLC_MAGIC)))
  {
    close_fli();
    return FLI_ERROR;
  }

  if (fli_header.width == 0)
    fli_header.width = 320;

  if (fli_header.height == 0)
    fli_header.height = 200;

  // create the frame bitmap
  fli_bitmap_width=fli_header.width;
  fli_bitmap_height=fli_header.height;
  fli_bitmap_data=(byte *)malloc(fli_bitmap_width*fli_bitmap_height);
  if (!fli_bitmap_data)
  {
    close_fli();
    return FLI_ERROR;
  }

  reset_fli_variables();
  fli_frame = 0;
  fli_status = FLI_OK;

  if (fli_header.type == FLI_MAGIC)
    speed = (long)fli_header.speed * 100 / 70;
  else
    speed = (long)fli_header.speed / 10;

  if (speed == 0)
    speed = 100;

  return fli_status;
}

int open_fli(char *filename)
{
  if (fli_status != FLI_NOT_OPEN)
    return FLI_ERROR;
  fli_file = open(filename, O_RDONLY | O_BINARY, S_IRUSR | S_IWUSR);
  if (fli_file < 0)
  {
    fli_file = 0;
    return FLI_ERROR;
  }
  return do_open_fli();
}

int open_memory_fli(byte *fli_data)
{
  if (fli_status != FLI_NOT_OPEN)
    return FLI_ERROR;
  fli_mem_data = fli_data;
  fli_mem_pos = 0;
  return do_open_fli();
}

void close_fli()
{
  //remove_int(fli_timer_callback);

  if (fli_file)
  {
    close(fli_file);
    fli_file = 0;
  }

  if (fli_bitmap_data)
  {
    free(fli_bitmap_data);
    fli_bitmap_data = NULL;
  }

  fli_mem_data = NULL;
  fli_mem_pos = 0;
  reset_fli_variables();
  fli_status = FLI_NOT_OPEN;
}

int next_fli_frame(int loop)
{
  if (fli_status != FLI_OK)
    return fli_status;

  //fli_timer--;

  // end of file? should we loop?
  if (fli_frame >= fli_header.frame_count)
  {
    if (loop)
    {
      fli_seek(sizeof(FLI_HEADER), SEEK_SET);
      fli_frame = 0;
    }
    else
    {
      fli_status = FLI_EOF;
      return fli_status;
    }
  }
  read_frame();
  return fli_status;
}

void reset_fli_variables()
{
  fli_bmp_dirty_from = INT_MAX;
  fli_bmp_dirty_to = INT_MIN;
  fli_pal_dirty_from = INT_MAX;
  fli_pal_dirty_to = INT_MIN;
}
