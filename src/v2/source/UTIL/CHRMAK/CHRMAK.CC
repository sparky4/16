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

// CHRMAK.CC
// V2 PCX to CHR converter w/ makefiles
//
// coded by aen
// aen@verge-rpg.com

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>

#include <conio.h>

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned long quad;

static void fputw(word w, FILE *fp) { fwrite(&w,1,2,fp); }
static void fputd(quad d, FILE *fp) { fwrite(&d,1,4,fp); }
static word fgetw(FILE *fp) { word w; fread(&w,1,2,fp); return w; }
quad fgetd(FILE *fp) { quad d; fread(&d,1,4,fp); return d; }

// skips the number of bytes in the file; just seeks past them
static void fskip(int bytes, FILE *fp)
  { fseek(fp,bytes,SEEK_CUR); }

static void fputraw(char *raw, int bytes, FILE *fp)
  { fwrite(raw, 1,bytes, fp); }

// writes a null-terminated string to the file
void fputstrz(char *str, FILE *fp)
  { fputraw(str, strlen(str)+1, fp); }

// writes a string to the file (without the null-terminator), preceeded by
// a quad length marker
static void fputstrn(char *str, FILE *fp)
  { int n=strlen(str)+1; fputd(n,fp); fputraw(str,n,fp); }

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define CHRMAK_VER_MAJ 0
#define CHRMAK_VER_MIN 1
#define CHRMAK_VER_STR "0.1b"
#define CHRMAK_AUTHORS "aen"

// parsing vars
static char *mak_base=0;
static char *makp=0;
static int mak_bytes=0;
static char mak_tok_ident[256];
static char mak_tok_val[256];
static int mak_tok_valn=0;          // numeric val of mak_tok
static int mak_line=0;

// makefile vars
static char chrmak_makefile[256];

static char pcx_name[256];      static int got_pcx_name=0;
static char chr_name[256];      static int got_chr_name=0;
static int frame_w=0;           static int got_frame_w=0;
static int frame_h=0;           static int got_frame_h=0;
static int hot_x=0;             static int got_hot_x=0;
static int hot_y=0;             static int got_hot_y=0;
static int hot_w=0;             static int got_hot_w=0;
static int hot_h=0;             static int got_hot_h=0;
static int per_row=0;           static int got_per_row=0;
static int total_frames=0;      static int got_total_frames=0;
static int lidle=0;             static int got_lidle=0;
static int ridle=0;             static int got_ridle=0;
static int uidle=0;             static int got_uidle=0;
static int didle=0;             static int got_didle=0;
static char lscript[256];       static int got_lscript=0;
static char rscript[256];       static int got_rscript=0;
static char uscript[256];       static int got_uscript=0;
static char dscript[256];       static int got_dscript=0;

void warning(char *message, ...)
{
  static char buffer[256];
  va_list args;

  va_start(args, message);
  vsprintf(buffer, message, args);
  va_end(args);

  printf("%s \n", buffer);
}

static void fatal(char *message, ...)
{
  static char buffer[256];
  va_list args;

  va_start(args, message);
  vsprintf(buffer, message, args);
  va_end(args);

  printf("%s \n", buffer);

  exit(0);
}

//#ifdef __DJGPP__
// Watcom has one of these
static int filelength(int handle)
{
  struct stat fileinfo;
  if (-1 == fstat(handle, &fileinfo))
    fatal("error fstating");
  return fileinfo.st_size;
}
//#endif

static int streq(char *a, char *b)
{
  while (*a)
  {
    if (*a++ != *b++)
      return 0;
  }
  return !*b;
}

static void usage()
{
  printf("usage: chrmak <makefile> \n");
  exit(0);
}

static void banner()
{
  printf("chrmak v%s  by %s \n", CHRMAK_VER_STR, CHRMAK_AUTHORS);
}

static void parse_args(int argc, char *argv[])
{
  if (argc != 2)
    usage();

  strcpy(chrmak_makefile, argv[1]);
}

static void skip_cpp_comment()
{
  makp+=2;
  while (*makp && '\n'!=*makp)
    makp++;
  if (*makp)
    makp++;
}

static void skip_c_comment()
{
  makp+=2;
  while (*makp && ('*'!=makp[0] || '/'!=makp[1]))
  {
    if ('\n'==*makp)
      mak_line++;
    if ('/'==makp[0] && '*'==makp[1])
      skip_c_comment();
    else makp++;
  }
  if (*makp)
    makp+=2;
}

static void parse_whitespace()
{
  do {
    if (!*makp)
      return;
    if (isspace(*makp))
    {
      while (*makp && isspace(*makp))
      {
        if ('\n'==*makp)
          mak_line++;
        makp++;
      }
      continue;
    }
    if ('/'==makp[0] && '/'==makp[1])
      { skip_cpp_comment(); continue; }
    if ('/'==makp[0] && '*'==makp[1])
      { skip_c_comment(); continue; }
    break;
  } while (1);
}

static void grab_ident()
{
  char *t=mak_tok_ident;
  while (isalnum(*makp) || '_'==*makp)
    *t++=*makp++;
  *t=0;
  strlwr(mak_tok_ident);
}

static void grab_val()
{
  char *t=mak_tok_val;
  while (isalnum(*makp) || '_'==*makp)
    *t++=*makp++;
  *t=0;
  mak_tok_valn=atoi(mak_tok_val);
}

static int ident_is(char *id) { return streq(mak_tok_ident,id); }

static void do_assign()
{
  if (ident_is("pcx_name"))
    { strcpy(pcx_name, mak_tok_val); got_pcx_name=1;            return; }
  else if (ident_is("chr_name"))
    { strcpy(chr_name, mak_tok_val); got_chr_name=1;            return; }
  else if (ident_is("frame_w"))
    { frame_w=mak_tok_valn;          got_frame_w=1;             return; }
  else if (ident_is("frame_h"))
    { frame_h=mak_tok_valn;          got_frame_h=1;             return; }
  else if (ident_is("hot_x"))
    { hot_x=mak_tok_valn;            got_hot_x=1;               return; }
  else if (ident_is("hot_y"))
    { hot_y=mak_tok_valn;            got_hot_y=1;               return; }
  else if (ident_is("hot_w"))
    { hot_w=mak_tok_valn;            got_hot_w=1;               return; }
  else if (ident_is("hot_h"))
    { hot_h=mak_tok_valn;            got_hot_h=1;               return; }
  else if (ident_is("per_row"))
    { per_row=mak_tok_valn;          got_per_row=1;             return; }
  else if (ident_is("total_frames"))
    { total_frames=mak_tok_valn;     got_total_frames=1;        return; }
  else if (ident_is("lidle"))
    { lidle=mak_tok_valn;            got_lidle=1;               return; }
  else if (ident_is("ridle"))
    { ridle=mak_tok_valn;            got_ridle=1;               return; }
  else if (ident_is("uidle"))
    { uidle=mak_tok_valn;            got_uidle=1;               return; }
  else if (ident_is("didle"))
    { didle=mak_tok_valn;            got_didle=1;               return; }
  else if (ident_is("lscript"))
    { strcpy(lscript, mak_tok_val);  got_lscript=1;             return; }
  else if (ident_is("rscript"))
    { strcpy(rscript, mak_tok_val);  got_rscript=1;             return; }
  else if (ident_is("uscript"))
    { strcpy(uscript, mak_tok_val);  got_uscript=1;             return; }
  else if (ident_is("dscript"))
    { strcpy(dscript, mak_tok_val);  got_dscript=1;             return; }

  fatal("%s: unknown ident '%s' on line %i",
    chrmak_makefile, mak_tok_ident, mak_line);
}

static int parse_assign()
{
  int last_line=0; // helper for error detection

  parse_whitespace();
  if (!*makp)
    return 1;
  else if (isalpha(*makp))
  {
    grab_ident();      // get ident
    last_line=mak_line;

    parse_whitespace(); // expect
    if ('=' != *makp++)
      fatal("%s: expected = on line %i", chrmak_makefile, last_line);

    parse_whitespace(); // get val
    grab_val();
    last_line=mak_line;

    do_assign();

    parse_whitespace(); // expect
    if (';' != *makp++)
      fatal("%s: expected ; on line %i", chrmak_makefile, last_line);
    else while (';'==*makp) makp++;

    return 0;
  }
  fatal("%s: expected ident, got '%c' on line %i",
    chrmak_makefile, *makp, mak_line);
  return 1;
}

static void check_needs()
{
  if (!got_pcx_name)     fatal("%s: pcx_name missing", chrmak_makefile);
  if (!got_chr_name)     fatal("%s: chr_name missing", chrmak_makefile);
  if (!got_frame_w)      fatal("%s: frame_w missing", chrmak_makefile);
  if (!got_frame_h)      fatal("%s: frame_h missing", chrmak_makefile);
  if (!got_hot_x)        fatal("%s: hot_x missing", chrmak_makefile);
  if (!got_hot_y)        fatal("%s: hot_y missing", chrmak_makefile);
  if (!got_hot_w)        fatal("%s: hot_w missing", chrmak_makefile);
  if (!got_hot_h)        fatal("%s: hot_h missing", chrmak_makefile);
  if (!got_per_row)      fatal("%s: per_row missing", chrmak_makefile);
  if (!got_total_frames) fatal("%s: total_frames missing", chrmak_makefile);
  if (!got_lidle)        fatal("%s: lidle missing", chrmak_makefile);
  if (!got_ridle)        fatal("%s: ridle missing", chrmak_makefile);
  if (!got_uidle)        fatal("%s: uidle missing", chrmak_makefile);
  if (!got_didle)        fatal("%s: didle missing", chrmak_makefile);
  if (!got_lscript)      fatal("%s: lscript missing", chrmak_makefile);
  if (!got_rscript)      fatal("%s: rscript missing", chrmak_makefile);
  if (!got_uscript)      fatal("%s: uscript missing", chrmak_makefile);
  if (!got_dscript)      fatal("%s: dscript missing", chrmak_makefile);
}

static void parse_makefile()
{
  FILE *fp=0;

  printf("ú parsing %s \r", chrmak_makefile);
  fflush(stdout);

  fp=fopen(chrmak_makefile, "rb");
  if (!fp) fatal("unable to open %s", chrmak_makefile);

  mak_bytes=filelength(fileno(fp));     // calc bytes
  mak_base=new char [mak_bytes+1];      // alloc room
  fread(mak_base,1,mak_bytes,fp);       // read bytes
  mak_base[mak_bytes]=0;                // null-term
  makp=mak_base;                        // setup cur byte ptr
  mak_line=1;

  while (!parse_assign())
    ;

  // got everything we need?
  check_needs();

  delete[]mak_base; mak_base=0;
  makp=0;

  fclose(fp);

  printf("û \n");
  fflush(stdout);
}

////////////////////////////////////////////////////////////////////////////
// PCX & COMPRESS STUFF ////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

static FILE *chrfp=0;
static FILE *pcxfp=0;
static byte *virscr=0;

int manufacturer=0;                     // pcx header
int version=0;
int encoding=0;
int bpp=0;
int xmin=0,ymin=0;
int xmax=0,ymax=0;
int hres=0,vres=0;
char palette[48];
int reserved=0;
int color_planes=0;
int bpl=0;
int palette_type=0;
char filler[58];
byte pal[768];

int image_width=0,image_depth=0;
quad vidoffset=0;

void LoadPCXHeader()
{
  manufacturer  = fgetc(pcxfp); // always 10
  version       = fgetc(pcxfp); // should be 5?
  encoding      = fgetc(pcxfp); // always 1
  bpp           = fgetc(pcxfp); // bits per pixel

  xmin          = fgetw(pcxfp); // grab window
  ymin          = fgetw(pcxfp);
  xmax          = fgetw(pcxfp);
  ymax          = fgetw(pcxfp);
  image_width   = xmax-xmin+1;  // calc dims
  image_depth   = ymax-ymin+1;

  hres          = fgetw(pcxfp);
  vres          = fgetw(pcxfp);

  fread(palette,1,48,pcxfp);    // ega color map

  reserved      = fgetc(pcxfp);
  color_planes  = fgetc(pcxfp); // should be 1

  bpl           = fgetw(pcxfp); // bytes per line
  palette_type  = fgetw(pcxfp);

  fread(filler,1,58,pcxfp);     // nothing important here
}

void ReadPCXLine(byte *dest)
{
  int i=0,c=0,n=0,run=0;

  // decode a row
  for (n=0; n<image_width; n+=run)
  {
    // assume single pixel
    run=1;
    c=fgetc(pcxfp);
    // is this a run?
    if ((c&192)==192)
    {
      // calc run and grab color
      run=c&63;
      c=fgetc(pcxfp);
    }
    // now replicate run bytes
    for (i=0; i<run; i++)
      dest[vidoffset+n+i]=(byte)c;
  }
  // skip row padding, if any
  fskip(bpl-image_width,pcxfp);
}

void LoadPCX(char *filename)
{
  int i=0;

  // open file
  if (pcxfp) fclose(pcxfp);
  pcxfp=fopen(filename,"rb");
  if (!pcxfp)
  { printf("unable to open %s", filename);
    exit(0);
  }

  // get info
  LoadPCXHeader();

  // allocate image data
  if (virscr) { free(virscr); virscr=0; }
  virscr=(byte *)malloc(image_width*image_depth);

  // decompress image data
  for (i=0; i<image_depth; i++)
  { vidoffset=i*image_width;
    ReadPCXLine(virscr);
  }

  // pal exists?
  if (fgetc(pcxfp) == 12)
    fread(pal,1,768,pcxfp);

  // done
  fclose(pcxfp);
}

static int bufsize=0;
static byte *csrc=0, *cb=0;

static void emitc(byte b) { *csrc++=b, bufsize++; }

static void Compress(byte *p, int len)
{
  int i=0;
  byte byt=0,samect=0;

  csrc=cb;
  i=0; bufsize=0;
  do
  {
    byt=p[i++];
    samect=1;
    while (samect<254 && i<len && byt==p[i])
      { samect++; i++; }

    if (samect==2 && byt != 0xFF) { emitc(byt); }
    if (samect==3 && byt != 0xFF) { emitc(byt); emitc(byt); }
    if (samect>3 || byt == 0xFF)
    {
      emitc(0xFF);
      emitc(samect);
    }
    emitc(byt);

  } while (i<len);
}

static byte *raw=0,*r=0;

static void dumpframe(int x, int y)
{
  byte *src=&virscr[(y*image_width)+x];

  for (y=0; y<frame_h; y++)
  {
    for (x=0; x<frame_w; x++)
      *r++=src[x];
    src+=image_width;
  }
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

static void WriteFrames(char *filename, FILE *dump)
{
  int i=0,j=0;
  int raw_bytes=0;

  printf("  ú reading image %s \r", pcx_name);
  fflush(stdout);

  LoadPCX(filename);

  printf("  û \n");
  fflush(stdout);

  printf("  ú collecting frames \r");
  fflush(stdout);

  // setup bufs for raw data/compression
  raw_bytes = total_frames * (frame_w*frame_h);
  cb  = (byte *)malloc(raw_bytes);
  raw = (byte *)malloc(raw_bytes); r=raw;

  // dump all frames in row major byte order into 'raw'
  for (i=0; i<10; i++)
    for (j=0; j<per_row; j++)
    {
      if ((i*per_row)+j<total_frames)
        dumpframe(1+(j*(frame_w+1)), 1+(i*(frame_h+1)));
    }

  printf("  û \n");
  fflush(stdout);

  printf("  ú compressing \r");
  fflush(stdout);

  // smush the sucker
  Compress(raw, raw_bytes);

  fputd(bufsize,dump);           // compressed data bytes
  fputraw((char *)cb,bufsize,dump);      // compressed data

  printf("  û \n");
  fflush(stdout);

  // cleanup bufs
  free(raw);
  free(cb);
}

static void WriteInfo(FILE *dump)
{
  fputc(2,  dump);              // version
  fputw((word)frame_w, dump);         // frame dims
  fputw((word)frame_h, dump);
  fputw((word)hot_x, dump);           // hotspot coords
  fputw((word)hot_y, dump);
  fputw((word)hot_w, dump);           // hotspot dims
  fputw((word)hot_h, dump);
  fputw((word)total_frames, dump);    // total frames
}

static void WriteScripts(FILE *fp)
{
  fputd(lidle,fp);
  fputd(ridle,fp);
  fputd(uidle,fp);
  fputd(didle,fp);

  fputstrn(lscript,fp);
  fputstrn(rscript,fp);
  fputstrn(uscript,fp);
  fputstrn(dscript,fp);
}

static void write_chr()
{
  strcat(pcx_name, ".pcx");
  strcat(chr_name, ".chr");

  /*
  // DEBUG
  printf(" \n");
  printf("pcx_name: %s \n", pcx_name);
  printf("chr_name: %s \n", chr_name);
  printf("frame_w: %i \n", frame_w);
  printf("frame_h: %i \n", frame_h);
  printf("hot_x: %i \n", hot_x);
  printf("hot_y: %i \n", hot_y);
  printf("hot_w: %i \n", hot_w);
  printf("hot_h: %i \n", hot_h);
  printf("per_row: %i \n", per_row);
  printf("total_frames: %i \n", total_frames);
  printf("lidle: %i \n", lidle);
  printf("ridle: %i \n", ridle);
  printf("uidle: %i \n", uidle);
  printf("didle: %i \n", didle);
  printf("lscript: %s \n", lscript);
  printf("rscript: %s \n", rscript);
  printf("uscript: %s \n", uscript);
  printf("dscript: %s \n", dscript);
  fflush(stdout);
  getch();
  */

  printf("> dumping %s \n", chr_name);
  fflush(stdout);

  chrfp=fopen(chr_name, "wb");
  if (!chrfp) fatal("unable to open %s", chr_name);

  WriteInfo(chrfp);
  WriteFrames(pcx_name,chrfp);
  WriteScripts(chrfp);

  fclose(chrfp);

  printf("complete! \n");
  fflush(stdout);
}

int main(int argc, char *argv[])
{
  banner();

  parse_args(argc, argv);
  parse_makefile();

  write_chr();

  return 0;
}
