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

// Mega kudos to aen for porting that GIF code.

#include <conio.h>
#include "verge.h"

// ========================= PCX Imaging routines ============================

char manufacturer;                     // pcx header
char version;
char encoding;
char bits_per_pixel;
word xmin, ymin;
word xmax, ymax;
word hres, vres;
char palette[48];
char reserved;
char color_planes;
word bytes_per_line;
word palette_type;
char filler[58];

word width, depth;
word bytes;
byte c, run, ss=0;
quad vidoffset, n=0;
VFILE *pcxf;
char *cb,*src;
int i;

// ================================= Code ====================================

void ReadPCXLine(byte *dest)
{ n=0;
  while (n<width)
  {
    run=1;
    c=vgetc(pcxf); //*src++;
    if (c>=0xc0) //(c & 0xc0)==0xc0)
    {
      run=c & 0x3f;
      c=vgetc(pcxf); //*src++;
    }
    while (run--)
      dest[vidoffset+n++]=c;
  }
  vseek(pcxf, bytes_per_line-width, 1);
}

void LoadPCXHeader(char *fname)
{
  if (!(pcxf=vopen(fname))) err("Could not open PCX file %s.",fname);
  vread(&manufacturer,1,pcxf);
  vread(&version,1,pcxf);
  vread(&encoding,1,pcxf);
  vread(&bits_per_pixel,1,pcxf);
  vread(&xmin,2,pcxf);
  vread(&ymin,2,pcxf);
  vread(&xmax,2,pcxf);
  vread(&ymax,2,pcxf);
  vread(&hres,2,pcxf);
  vread(&vres,2,pcxf);
  vread(&palette,48,pcxf);
  vread(&reserved,1,pcxf);
  vread(&color_planes,1,pcxf);
  vread(&bytes_per_line,2,pcxf);
  vread(&palette_type,2,pcxf);
  vread(&filler,58,pcxf);
  //vseek(pcxf,filesize(pcxf)-768,SEEK_SET);
  //vread(&pal,768,pcxf);
  vseek(pcxf,128L,SEEK_SET);
  width=xmax-xmin+1;
  depth=ymax-ymin+1;

  bytes=bytes_per_line;

  //for (i=0; i<768; i++)
  //  pal[i]=pal[i] >> 2;
//  SetPalette((byte *) &pal);

  //i=filesize(pcxf);
  //cb=(char *) valloc(i-896, "LoadPCXHeader:cb", OID_TEMP);
  //vread(cb, i-(128+768+1), pcxf);
}

void LoadPCXHeaderNP(char *fname)
{
  if (!(pcxf=vopen(fname))) err("Could not open PCX file %s.",fname);
  vread(&manufacturer,1,pcxf);
  vread(&version,1,pcxf);
  vread(&encoding,1,pcxf);
  vread(&bits_per_pixel,1,pcxf);
  vread(&xmin,2,pcxf);
  vread(&ymin,2,pcxf);
  vread(&xmax,2,pcxf);
  vread(&ymax,2,pcxf);
  vread(&hres,2,pcxf);
  vread(&vres,2,pcxf);
  vread(&palette,48,pcxf);
  vread(&reserved,1,pcxf);
  vread(&color_planes,1,pcxf);
  vread(&bytes_per_line,2,pcxf);
  vread(&palette_type,2,pcxf);
  vread(&filler,58,pcxf);
  vseek(pcxf,filesize(pcxf)-768,SEEK_SET);
  vread(&pal,768,pcxf);
  vseek(pcxf,128L,SEEK_SET);
  width=xmax-xmin+1;
  depth=ymax-ymin+1;
  bytes=bytes_per_line;

  i=filesize(pcxf);
  cb=(char *) valloc(i-896, "LoadPCXHeaderNP:cb", OID_TEMP);
  vread(cb, i-896, pcxf);
}

void LoadPCX(char *fname, byte *dest)
{
  LoadPCXHeader(fname);

  src=cb;
  for (i=0; i<depth; i++)
  {
    vidoffset=(i*width);
    ReadPCXLine(dest);
  }
  vfree(cb);
  vclose(pcxf);
}

byte *LoadPCXBuf(char *fname)
{ byte *buf=0;

//  LoadPCXHeaderNP(fname);
  LoadPCXHeader(fname);
  buf=(byte *) valloc(width*(depth+1), "LoadPCXBuf:buf", OID_IMAGE);
  //src=cb;
  for (vidoffset=0,i=0; i<depth; i++,vidoffset+=width)
    ReadPCXLine(buf);
  // palette avail?
  if (12==vgetc(pcxf))
  {
    vread(pal,768,pcxf);
    for (i=0; i<768; i++)
      pal[i]>>=2;
  }
  //vfree(cb);
  vclose(pcxf);
  return buf;
}

// ========================= BMP Imaging routines ============================

struct RGBQUAD
{
  unsigned char r,g,b,a;
};

struct RGBQUAD bmppalette[256];

// ================================= Code ====================================

byte *LoadBMPBuf(char *fname)
{ byte pad[4], *buf;
  VFILE* bmpfile;
  int bwidth, bheight, y;

  if (!(bmpfile=vopen(fname))) err("Could not open BMP file %s.",fname);
  vseek(bmpfile, 18, SEEK_SET);
  vread(&bwidth, 4, bmpfile); width=bwidth;
  vread(&bheight, 4, bmpfile); depth=bheight;
  vseek(bmpfile, 54, SEEK_SET);
  vread(bmppalette, 1024, bmpfile);
  buf=(byte*) valloc(bwidth*bheight, "LoadBMPBuf:buf", OID_IMAGE);

  for (y=bheight-1; y>=0; y--)
  {
    vread((char *)((int)buf+y*width),width,bmpfile);
    vread(pad,width%4,bmpfile);
  }
  for (y=0; y<256; y++)
  {
    pal[y*3]=bmppalette[y].b >> 2;
    pal[y*3+1]=bmppalette[y].g >> 2;
    pal[y*3+2]=bmppalette[y].r >> 2;
  }
  SetPalette((byte*)pal);
  vclose(bmpfile);
  return buf;
}

// ========================== GIF Imaging routines ===========================

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

typedef signed char s8;
typedef signed short s16;
typedef signed long s32;

typedef struct
{
  u8 bits;
  u8 background;
  u8 * palette;
  u8 * image;
  s16 wide, deep;
} gif_image_info;

typedef struct
{
  char sig[7];
  s16 screenwide, screendeep;
  u8 hflags;
  u8 background;
  u8 aspect;
} gif_header;

typedef struct
{
  s16 top, left;
  s16 wide, deep;
  u8 iflags;
} gif_image_descriptor;

int NO_CODE = -1,// TRUE = 1,
    ERROR_EOF = 0, ERROR_BAD_CODE = 1,
    ERROR_BAD_HEADER = 2, ERROR_BAD_STARTCODE = 3,
    ERROR_BAD_FIRST_CODE = 4, ERROR_BAD_FILE = 5,
    ERROR_NO_IMAGE = 6;

char* gif_error_messages[] =
{
  "Unexpected end of file\n",
  "Bad code\n",
  "Bad gif header\n",
  "Bad symbol size\n",
  "Bad first code\n",
  "Error opening file\n",
  "This file doesn't contain an image\n"
};

// read colour palette, vga palette values are 6 bit numbers
// while gif allows for 8 bit so shift right to get correct colours

u8* gif_read_palette(FILE* fp, s32 bytes)
{
  s32 i = 0;
  u8* block = 0L;
  s32 components = (bytes / 3) * 3;

  block  = (unsigned char *)valloc(components, "gif_read_palette:block", OID_IMAGE);

  for (i = 0; i < components; ++i)
    block[i] = fgetc(fp) >> 2;

  return block;
}

// read a block of bytes into memory
s32 block_mem_read(FILE* fp, u8* buffer, s32 bytes)
{
  s32 status = 0;

  status = fread(buffer, 1, bytes, fp);
  if (status != bytes) return EOF;

  return TRUE;
}

// read a unsigned 16 bit value from file, low byte first; note that this
// is reverse endian-ness (ie. fwrite(&s,1,2,fp); writes high byte first).

s16 read_word_lbf(FILE* fp)
{
  s32 a, b;

  a = fgetc(fp);
  b = fgetc(fp);

  return (b << 8) | a;
}

// read the GIF file header structure into a sequence
gif_header* get_gif_header(FILE* fp)
{
  gif_header* h = 0L;

  h = (gif_header *)valloc(sizeof(gif_header), "get_gif_header:h", OID_IMAGE);

  fread(h->sig, 1, 6, fp);
  h->sig[6] = 0;

  if (strncmp(h->sig, "GIF", 3) != 0)
    return NULL;

  h->screenwide = read_word_lbf(fp); width=h->screenwide;
  h->screendeep = read_word_lbf(fp); depth=h->screendeep;
  h->hflags = fgetc(fp);
  h->background = fgetc(fp);
  h->aspect = fgetc(fp);

  return h;
}

// gif file can contain more than one image,
// each image is preceeded by a header structure
gif_image_descriptor* get_image_descriptor(FILE* fp)
{
  gif_image_descriptor* id = 0L;

  id = (gif_image_descriptor *)valloc(sizeof(gif_image_descriptor), "get_image_descriptor:id", OID_IMAGE);

  id->left = read_word_lbf(fp);
  id->top = read_word_lbf(fp);
  id->wide = read_word_lbf(fp);
  id->deep = read_word_lbf(fp);
  id->iflags = fgetc(fp);

  return id;
}

static u16 word_mask_table[] =
{
  0x0000, 0x0001, 0x0003, 0x0007,
  0x000F, 0x001F, 0x003F, 0x007F,
  0x00FF, 0x01FF, 0x03FF, 0x07FF,
  0x0FFF, 0x1FFF, 0x3FFF, 0x7FFF
};

static u8 inc_table[] = { 8,8,4,2,0 };
static u8 start_table[] = { 0,4,2,1,0 };

// enables me to use indices as per EUPHORiA (ie. converts to C's 0 base)
#define eui(i) ((i)-1)

// unpack an LZW compressed image
// returns a sequence containing screen display lines of the image
u8* unpack_image(FILE* fp, s32 start_code_size, u32 width, u32 depth, u32 flags)
{
  u8* buffer;
  u8* line_buffer;

  u16 first_code_stack[4096];
  u16 last_code_stack[4096];
  u16 code_stack[4096];

  s32 bits_left;
  s32 clear_code;
  s32 code_size;
  s32 code_size2;
  s32 next_code;
  s32 this_code;
  s32 old_token;
  s32 current_code;
  s32 old_code;
  s32 block_size=0;
  s32 line;
  s32 a_byte;
  s32 pass;
  s32 u;

  u8 b[256]; // read buffer; for block reads
  u8* p; // current byte in read buffer
  u8* q; // last byte in read buffer + 1

  line_buffer = (u8 *)valloc(width, "unpack_image:line_buffer", OID_TEMP);
  buffer      = (u8 *)valloc(width * depth, "unpack_image:buffer", OID_IMAGE);

  a_byte = 0;
  line = 0;
  pass = 0;
  bits_left = 8;

  if (start_code_size < 2 || start_code_size > 8)
    err("\nERROR_BAD_STARTCODE"); // bad symbol size

  p = b;
  q = b;

  clear_code = 1 << start_code_size; //pow(2, start_code_size);
  next_code = clear_code + 2;
  code_size = start_code_size + 1;
  code_size2 = 1 << code_size; //pow(2, code_size);
  old_code = NO_CODE;
  old_token = NO_CODE;

  while (1)
  {
    if (bits_left == 8)
    {
      ++p;
      if (p >= q)
      {
        block_size = fgetc(fp);
        if (block_mem_read(fp, b, block_size) == EOF)
          err("\nERROR_EOF");
        p = b;
        q = b + block_size;
      }
      bits_left = 0;
    }

    this_code = *p;
    current_code = code_size + bits_left;

    if (current_code <= 8)
    {
      *p = *p >> code_size;
      bits_left = current_code;
    }
    else
    {
      ++p;
      if (p >= q)
      {
        block_size = fgetc(fp);
        if (block_mem_read(fp, b, block_size) == EOF)
          err("\nERROR_EOF");
        p = b;
        q = b + block_size;
      }

      this_code |= (*p << (8 - bits_left));

      if (current_code <= 16)
      {
        bits_left = current_code - 8;
        *p = *p >> bits_left;
      }
      else
      {
        if (++p >= q)
        {
          block_size = fgetc(fp);
          if (block_mem_read(fp, b, block_size) == EOF)
            err("\nERROR_EOF");
          p = b;
          q = b + block_size;
        }

        this_code |= (*p << (16 - bits_left));

        bits_left = current_code - 16;
        *p = *p >> bits_left;
      }
    }

    this_code &= word_mask_table[code_size];
    current_code = this_code;

    if (this_code == (clear_code+1) || block_size == 0)
      break;
    if (this_code > next_code)
      err("\nERROR_BAD_CODE");

    if (this_code == clear_code)
    {
      next_code = clear_code + 2;
      code_size = start_code_size + 1;
      code_size2 = 1 << code_size; //pow(2, code_size);
      old_code = NO_CODE;
      old_token = NO_CODE;
    }
    else
    {
      u = 1;
      if (this_code == next_code)
      {
        if (old_code == NO_CODE)
          err("\nERROR_BAD_FIRST_CODE");

        first_code_stack[eui(u)] = old_token;
        u++;
        this_code = old_code;
      }

      while (this_code >= clear_code)
      {
        first_code_stack[eui(u)] = last_code_stack[eui(this_code)];
        u++;
        this_code = code_stack[eui(this_code)];
      }

      old_token = this_code;
      while (1)
      {
        line_buffer[a_byte] = this_code;
        a_byte++;
        if (a_byte >= width)
        {
          // full image line so add it into screen image
          memcpy(buffer + (line * width), line_buffer, width);

          a_byte = 0;
          if (flags & 0x40)
          {
            line += inc_table[pass];
            if (line >= depth)
            {
              pass++;
              line = start_table[pass];
            }
          }
          else
          {
            line++;
          }
        }

        // no more bytes on stack
        if (u == 1) break;

        u--;
        this_code = first_code_stack[eui(u)];
      }

      if (next_code < 4096 && old_code != NO_CODE)
      {
        code_stack[eui(next_code)] = old_code;
        last_code_stack[eui(next_code)] = old_token;
        next_code++;
        if (next_code >= code_size2 && code_size < 12)
        {
          code_size++;
          code_size2 = 1 << code_size; //pow(2, code_size);
        }
      }

      old_code = current_code;
    }
  }

  // completed reading the image so return it
  free(b);
  return buffer;
}

// skip the extension blocks as we are only after the image
void skip_extension(FILE* fp)
{
  s32 n;
  char temp[256];

  n = fgetc(fp); // throwaway extension function code
  n = fgetc(fp); // get length of block

  while (n > 0 && n != EOF)
  {
    // throwaway block
    fread(temp, 1, n, fp);

    n = fgetc(fp); // get length of next block
  }
}

// unpack the GIF file
// returns ImageInfo sequence containing image and image data
gif_image_info* unpack_gif(char* filename)
{
  VFILE *f;
  FILE* fp;
  s32 c, b;
  gif_header* h = 0L;
  gif_image_info* ii = 0L;
  gif_image_descriptor* id = 0L;
  u8* local_palette = 0L;

  ii = (gif_image_info *)valloc(sizeof(gif_image_info), "unpack_gif:ii", OID_IMAGE);

  f = vopen(filename);
  if (!f) err("Could not open GIF file %s.",filename);
  fp = f->fp;
  if (!fp) err("\nBad filename");

  // file starts with the Logical Screen Descriptor structure
  h = get_gif_header(fp);

  // Size of Global Color Table
  ii->bits = (h->hflags & 7) + 1;
  ii->background = h->background;

  // get Global colour palette if there is one
  if (h->hflags & 0x80) // is flags bit 8 set?
  {
    c = 3 << ii->bits; // size of global colour map
    ii->palette = gif_read_palette(fp, c);

/*    outp(968, 0);
    // enforce global color map
    for (s32 i=0; i < 768; ++i)
      outp(969, ii->palette[i]); */
    memcpy(pal, ii->palette, 768);
  }

  c = fgetc(fp);

  while (c == 0x2c || c == 0x21 || c == 0)
  {
    // image separator so unpack the image
    if (c == 0x2c)
    {
      id = get_image_descriptor(fp); // get the Image Descriptor
      // if there is a local Color Table then overwrite the global table
      if (id->iflags & 0x80)
      {
        ii->bits = (id->iflags & 7) + 1;
        b = 3 << ii->bits;
        if (local_palette)
          free(local_palette);
        local_palette = gif_read_palette(fp, b);
      }

      c = fgetc(fp); // get the LZW Minimum Code Size
      ii->image = unpack_image(fp, c, id->wide, id->deep, id->iflags);
      vclose(f);

      // error reading image
      if (!ii->image)
        err("\nerror reading image data");

      ii->wide = id->wide;
      ii->deep = id->deep;

      // return imagedata
      return ii;
    }
    // extension introducer
    else if (c == 0x21)
    {
      skip_extension(fp); // throw the extension away
    }

    c = fgetc(fp);
  }

  // no image?
  return NULL;
}

byte *LoadGIFBuf(char *fname)
{
  gif_image_info *ii=0;
  byte *t;

  ii=unpack_gif(fname);
  width=ii->wide;
  depth=ii->deep;
  t=ii->image;
  free(ii);
  return t;
}

// ====================== Universal Imaging Interface ========================

byte imagetype;

// ================================= Code ====================================

void DetermineFileType(char *fname)
{
  strlwr(fname);
  if (!strcmp(fname+(strlen(fname)-3),"pcx")) imagetype=0;
  if (!strcmp(fname+(strlen(fname)-3),"gif")) imagetype=1;
  if (!strcmp(fname+(strlen(fname)-3),"bmp")) imagetype=2;
}

void VLoadImage(char *fname, byte *dest)
{
  imagetype=255;
  DetermineFileType(fname);
  switch (imagetype)
  {
    case 0: LoadPCX(fname, dest); break;
    case 1: err("VLoadImage() for GIF not supported; use VLoadImageBuf()"); break;
    case 2: err("VLoadImage() for BMP not supported; use VLoadImageBuf()"); break;
    default: err("%s: Unrecognized image type.",fname);
  }
}

byte *VLoadImageBuf(char *fname)
{
  imagetype=255;
  DetermineFileType(fname);
  switch (imagetype)
  {
    case 0: return LoadPCXBuf(fname); break;
    case 1: return LoadGIFBuf(fname); break;
    case 2: return LoadBMPBuf(fname); break;
    default: err("%s: Unrecognized image type.",fname);
  }
  return 0;
}
