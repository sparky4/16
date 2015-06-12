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

#ifndef VFILE_H
#define VFILE_H

#define vscanf _vscanf

typedef struct
{
  FILE *fp;                           // real file pointer.
  byte s;                             // 0=real file 1=vfile;
  byte v;                             // if vfile, which vfile index
  byte i;                             // which file index in vfile is it?
  byte p;                             // data alignment pad. :)
} VFILE;

struct filestruct
{
  unsigned char fname[84];            // pathname thingo
  int size;                           // size of the file
  int packofs;                        // where the file can be found in PACK
  int curofs;                         // current file offset.
  char extractable;                   // irrelevant to runtime, but...
  char override;                      // should we override?
};

struct mountstruct
{
  char mountname[80];                 // name of VRG packfile.
  FILE *vhandle;                      // Real file-handle of packfile.
  struct filestruct *files;           // File record array.
  int numfiles;                       // number of files in pack.
  int curofs;                         // Current filepointer.
};

extern struct mountstruct pack[3];
extern char filesmounted;

int Exist(char *fname);
VFILE *vopen(char *fname);
void MountVFile(char *fname);
void vread(void *dest, int len, VFILE *f);
void vclose(VFILE *f);
int filesize(VFILE *f);
int vtell(VFILE *f);
void vseek(VFILE *f, int offset, int origin);
void vscanf(VFILE *f, char *format, char *dest);
char vgetc(VFILE *f);
word vgetw(VFILE *f);
void vgets(char *str, int len, VFILE *f);
#endif
