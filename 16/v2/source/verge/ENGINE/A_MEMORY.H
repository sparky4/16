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

#ifndef MEMORY_H
#define MEMORY_H

extern void *valloc(int amount, char *desc, int owner);
extern void *qvalloc(int amount);
extern void qvfree(void *ptr);
extern int  TotalBytesAllocated(void);
extern int  FindChunk(void *pointer);
extern void FreeChunk(int i);
extern int  vfree(void *pointer);
extern void FreeByOwner(int owner);
extern void MemReport(void);
extern int  ChunkIntegrity(int i);
extern void CheckCorruption(void);

// Standardized OID (Owner ID) values

#define OID_TEMP    0
#define OID_VFILE   1
#define OID_IMAGE   2
#define OID_MISC    3
#define OID_MAP     4
#define OID_VC      5

#endif // MEMORY_H
