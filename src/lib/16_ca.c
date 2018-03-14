/* Project 16 Source Code~
 * Copyright (C) 2012-2018 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
 *
 * This file is part of Project 16.
 *
 * Project 16 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Project 16 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

// ID_CA.C

/*
=============================================================================

Id Software Caching Manager
---------------------------

Must be started BEFORE the memory manager, because it needs to get the headers
loaded into the data segment

=============================================================================
*/

#include "src/lib/16_ca.h"
#pragma hdrstop

#pragma warn -pro
#pragma warn -use

#define THREEBYTEGRSTARTS
//https://github.com/open-watcom/open-watcom-v2/issues/279#issuecomment-244587566 for _seg
//http://www.shikadi.net/moddingwiki/GameMaps_Format for info on the code
/*
=============================================================================

						 LOCAL CONSTANTS

=============================================================================
*/

/*typedef struct
{
  word bit0,bit1;	// 0-255 is a character, > is a pointer to a node
} huffnode;*/


/*typedef struct
{
	unsigned	RLEWtag;
	long		headeroffsets[100];
	byte		tileinfo[];
} mapfiletype;*/


/*
=============================================================================

						 GLOBAL VARIABLES

=============================================================================
*/

/*byte 		_seg	*tinf;
int			mapon;

unsigned	_seg	*mapsegs[3];
maptype		_seg	*mapheaderseg[NUMMAPS];
byte		_seg	*audiosegs[NUMSNDCHUNKS];
void		_seg	*grsegs[NUMCHUNKS];

byte		far	grneeded[NUMCHUNKS];*/

void	(*drawcachebox)		(char *title, unsigned numcache);
void	(*updatecachebox)	(void);
void	(*finishcachebox)	(void);

/*
=============================================================================

						 LOCAL VARIABLES

=============================================================================
*/

/*extern	long	far	CGAhead;
extern	long	far	EGAhead;
extern	byte	CGAdict;
extern	byte	EGAdict;
extern	byte	far	maphead;
extern	byte	mapdict;
extern	byte	far	audiohead;
extern	byte	audiodict;*/

void CA_CannotOpen(char *string, global_game_variables_t *gvar);

/*long		_seg *grstarts;	// array of offsets in egagraph, -1 for sparse
long		_seg *audiostarts;	// array of offsets in audio / audiot

#ifdef GRHEADERLINKED
huffnode	*gvar->ca.grhuffman;
#else
huffnode	gvar->ca.grhuffman[255];
#endif

#ifdef AUDIOHEADERLINKED
huffnode	*audiohuffman;
#else
huffnode	audiohuffman[255];
#endif


int			grhandle;		// handle to EGAGRAPH
int			maphandle;		// handle to MAPTEMP / GAMEMAPS
int			audiohandle;	// handle to AUDIOT / AUDIO
*/
long		chunkcomplen,chunkexplen;
/*
SDMode		oldsoundmode;



void	CAL_DialogDraw (char *title,unsigned numcache);
void	CAL_DialogUpdate (void);
void	CAL_DialogFinish (void);*/
void	CAL_CarmackExpand (unsigned far *source, unsigned far *dest,
		unsigned length);


#ifdef THREEBYTEGRSTARTS
#define FILEPOSSIZE	3
//#define	GRFILEPOS(c) (*(long far *)(((byte far *)grstarts)+(c)*3)&0xffffff)
CASVT GRFILEPOS(int c, global_game_variables_t *gvar)
{
	CASVT value;
	int	offset;

	offset = c*3;

	value = *(CASVT far *)(((byte far *)gvar->ca.grstarts)+offset);

	value &= 0x00ffffffl;

	if (value == 0xffffffl)
		value = -1;

	return value;
};
#else
#define FILEPOSSIZE	4
//#define	GRFILEPOS(c) (gvar->ca.grstarts[c])
CASVT GRFILEPOS(int c, global_game_variables_t *gvar)
{
	return gvar->ca.grstarts[c];
}
#endif

//#define EXTENSION	"hb1"

/*
=============================================================================

					   LOW LEVEL ROUTINES

=============================================================================
*/

/*
============================
=
= CA_OpenDebug / CA_CloseDebug
=
= Opens a binary file with the handle "debughandle"
=
============================
*/
void CA_OpenDebug(global_game_variables_t *gvar)
{
#ifdef __BORLANDC__
	unlink("debug.16b");
	gvar->handle.debughandle = open("debug.16b", O_CREAT | O_WRONLY | O_TEXT);
#endif
#ifdef __WATCOMC__
	unlink("debug.16w");
	gvar->handle.debughandle = open("debug.16w", O_CREAT | O_WRONLY | O_TEXT);
#endif
}

void CA_CloseDebug(global_game_variables_t *gvar)
{
	close(gvar->handle.debughandle);
}



/*
============================
=
= CAL_GetGrChunkLength
=
= Gets the length of an explicit length chunk (not tiles)
= The file pointer is positioned so the compressed data can be read in next.
=
============================
*/

void CAL_GetGrChunkLength (int chunk,global_game_variables_t *gvar)
{
	lseek(gvar->ca.file.grhandle,GRFILEPOS(chunk,gvar),SEEK_SET);
	read(gvar->ca.file.grhandle,&gvar->ca.chunkexplen,sizeof(gvar->ca.chunkexplen));
	gvar->ca.chunkcomplen = GRFILEPOS(chunk+1,gvar)-GRFILEPOS(chunk,gvar)-4;
}


/*
==========================
=
= CA_FarRead
=
= Read from a file to a far pointer
=
==========================
*/

boolean CA_FarRead(int handle, byte far *dest, dword length, global_game_variables_t *gvar)
{
	boolean flag=0;
	if (length>0xfffflu)
		Quit (gvar, "CA_FarRead doesn't support 64K reads yet!");//TODO: EXPAND!!!

	__asm {
		push	ds
		mov	bx,[handle]
		mov	cx,[WORD PTR length]
		mov	dx,[WORD PTR dest]
		mov	ds,[WORD PTR dest+2]
		mov	ah,0x3f				// READ w/handle
		int	21h
		pop	ds
		jnc	good
		mov	errno,ax
		mov	flag,0
		jmp End
#ifdef __BORLANDC__
	}
#endif
good:
#ifdef __BORLANDC__
	__asm {
#endif
		cmp	ax,[WORD PTR length]
		je	done
//		errno = EINVFMT;			// user manager knows this is bad read
		mov	flag,0
		jmp End
#ifdef __BORLANDC__
	}
#endif
done:
#ifdef __BORLANDC__
	__asm {
#endif
		mov	flag,1
#ifdef __BORLANDC__
	}
#endif
End:
#ifdef __WATCOMC__
	}
#endif
	return flag;
}


/*
==========================
=
= CA_FarWrite
=
= Write from a file to a far pointer
=
==========================
*/

boolean CA_FarWrite(int handle, byte far *source, dword length, global_game_variables_t *gvar)
{
	boolean flag=0;
	if (length>0xfffflu)
		Quit (gvar, "CA_FarWrite doesn't support 64K reads yet!");//TODO: EXPAND!!!

	__asm {
		push	ds
		mov	bx,[handle]
		mov	cx,[WORD PTR length]
		mov	dx,[WORD PTR source]
		mov	ds,[WORD PTR source+2]
		mov	ah,0x40			// WRITE w/handle
		int	21h
		pop	ds
		jnc	good
		mov	errno,ax
		mov flag,0
		jmp End
#ifdef __BORLANDC__
	}
#endif
good:
#ifdef __BORLANDC__
	__asm {
#endif
		cmp	ax,[WORD PTR length]
		je	done
//		errno = ENOMEM;				// user manager knows this is bad write
		mov	flag,0
		jmp End
#ifdef __BORLANDC__
	}
#endif
done:
#ifdef __BORLANDC__
	__asm {
#endif
		mov	flag,1
#ifdef __BORLANDC__
	}
#endif
End:
#ifdef __WATCOMC__
	}
#endif
	return flag;
}


/*
==========================
=
= CA_ReadFile
=
= Reads a file into an allready allocated buffer
=
==========================
*/

boolean CA_ReadFile(char *filename, memptr *ptr, global_game_variables_t *gvar)
{
	int handle;
	sdword size;
	//long size;

	if((handle = open(filename,O_RDONLY | O_BINARY, S_IREAD)) == -1)
		return false;

	size = filelength(handle);
#ifdef __DEBUG_CA__
	if(dbg_debugca>0){
		printf("===============================================================================\n");
		printf("		CA_ReadFile\n");
		printf("===============================================================================\n");
		//%04x
		printf("	ptr=%Fp\n", ptr);
		printf("	*ptr=%Fp\n", *ptr);
		printf("	&ptr=%Fp\n", &ptr);
	}
#endif
	if(!CA_FarRead(handle,*ptr,size, gvar))
	{
		close(handle);
		return false;
	}
	close(handle);
	return true;
}


/*
==========================
=
= CA_WriteFile
=
= Writes a file from a memory buffer
=
==========================
*/

boolean CA_WriteFile (char *filename, void far *ptr, long length, global_game_variables_t *gvar)
{
	int handle;
	//sdword size;
	//long size;

	handle = open(filename,O_CREAT | O_BINARY | O_WRONLY,
				S_IREAD | S_IWRITE | S_IFREG);

	if (handle == -1)
		return false;

	if (!CA_FarWrite (handle,ptr,length, gvar))
	{
		close(handle);
		return false;
	}
	close(handle);
	return true;
}



/*
==========================
=
= CA_LoadFile
=
= Allocate space for and load a file
=
==========================
*/

boolean CA_LoadFile(char *filename, memptr *ptr, global_game_variables_t *gvar)
{
	int handle;
	sdword size;
	//long size;

	if((handle = open(filename,O_RDONLY | O_BINARY, S_IREAD)) == -1)
		return false;

	size = filelength(handle);
#ifdef __DEBUG_CA__
	if(dbg_debugca>0){
		printf("===============================================================================\n");
		printf("		CA_LoadFile\n");
		printf("===============================================================================\n");
		//%04x
		printf("	ptr=%Fp\n", ptr);
		printf("	*ptr=%Fp\n", *ptr);
		printf("	&ptr=%Fp\n", &ptr);
	}
#endif
	MM_GetPtr(ptr,size, gvar);
#ifdef __DEBUG_CA__
	if(dbg_debugca>0){
		//%04x
		printf("---------------------------------------\n");
		printf("	ptr=%Fp\n", ptr);
		printf("	*ptr=%Fp\n", *ptr);
		printf("	&ptr=%Fp\n", &ptr);
		printf("-------------------------------------------------------------------------------\n");
	}
#endif
	if(!CA_FarRead(handle,*ptr,size, gvar))
	{
		close(handle);
		return false;
	}
	close(handle);
	return true;
}

/*
============================================================================

		COMPRESSION routines, see JHUFF.C for more

============================================================================
*/



/*
===============
=
= CAL_OptimizeNodes
=
= Goes through a huffman table and changes the 256-511 node numbers to the
= actular address of the node.  Must be called before CAL_HuffExpand
=
===============
*/

void CAL_OptimizeNodes(huffnode *table)
{
  huffnode *node;
  int i;

  node = table;

  for (i=0;i<255;i++)
  {
	if (node->bit0 >= 256)
	  node->bit0 = (unsigned)(table+(node->bit0-256));
	if (node->bit1 >= 256)
	  node->bit1 = (unsigned)(table+(node->bit1-256));
	node++;
  }
}



/*
======================
=
= CAL_HuffExpand
=
= Length is the length of the EXPANDED data
=
======================
*/

void CAL_HuffExpand (byte far *source, byte far *dest,
  long length,huffnode *hufftable)
{
//  unsigned bit,byte,node,code;
  unsigned sourceseg,sourceoff,destseg,destoff,endoff;
	huffnode *headptr;
//  huffnode *nodeon;

	headptr = hufftable+254;	// head node is allways node 254

  source++;	// normalize
  source--;
  dest++;
  dest--;

  sourceseg = FP_SEG(source);
  sourceoff = FP_OFF(source);
  destseg = FP_SEG(dest);
  destoff = FP_OFF(dest);
  endoff = destoff+length;

//
// ds:si source
// es:di dest
// ss:bx node pointer
//

	if (length <0xfff0)
	{

//--------------------------
// expand less than 64k of data
//--------------------------

	__asm {
		mov	bx,[word ptr headptr]

		mov	si,[sourceoff]
		mov	di,[destoff]
		mov	es,[destseg]
		mov	ds,[sourceseg]
		mov	ax,[endoff]

		mov	ch,[si]				// load first byte
		inc	si
		mov	cl,1
#ifdef __BORLANDC__
	}
#endif
expandshort:
#ifdef __BORLANDC__
	__asm {
#endif
		test	ch,cl			// bit set?
		jnz	bit1short
		mov	dx,[ss:bx]			// take bit0 path from node
		shl	cl,1				// advance to next bit position
		jc	newbyteshort
		jnc	sourceupshort
#ifdef __BORLANDC__
	}
#endif
bit1short:
#ifdef __BORLANDC__
	__asm {
#endif
		mov	dx,[ss:bx+2]		// take bit1 path
		shl	cl,1				// advance to next bit position
		jnc	sourceupshort
#ifdef __BORLANDC__
	}
#endif
newbyteshort:
#ifdef __BORLANDC__
	__asm {
#endif
		mov	ch,[si]				// load next byte
		inc	si
		mov	cl,1				// back to first bit
#ifdef __BORLANDC__
	}
#endif
sourceupshort:
#ifdef __BORLANDC__
	__asm {
#endif
		or	dh,dh				// if dx<256 its a byte, else move node
		jz	storebyteshort
		mov	bx,dx				// next node = (huffnode *)code
		jmp	expandshort
#ifdef __BORLANDC__
	}
#endif
storebyteshort:
#ifdef __BORLANDC__
	__asm {
#endif
		mov	[es:di],dl
		inc	di					// write a decopmpressed byte out
		mov	bx,[word ptr headptr]		// back to the head node for next bit

		cmp	di,ax				// done?
		jne	expandshort
	}
	}
	else
	{

//--------------------------
// expand more than 64k of data
//--------------------------

  length--;

	__asm {
		mov	bx,[word ptr headptr]
		mov	cl,1

		mov	si,[sourceoff]
		mov	di,[destoff]
		mov	es,[destseg]
		mov	ds,[sourceseg]

		lodsb			// load first byte
#ifdef __BORLANDC__
	}
#endif
expand:
#ifdef __BORLANDC__
	__asm {
#endif
		test	al,cl		// bit set?
		jnz	bit1
		mov	dx,[ss:bx]	// take bit0 path from node
		jmp	gotcode
#ifdef __BORLANDC__
	}
#endif
bit1:
#ifdef __BORLANDC__
	__asm {
#endif
		mov	dx,[ss:bx+2]	// take bit1 path
#ifdef __BORLANDC__
	}
#endif
gotcode:
#ifdef __BORLANDC__
	__asm {
#endif
		shl	cl,1		// advance to next bit position
		jnc	sourceup
		lodsb
		cmp	si,0x10		// normalize ds:si
		jb	sinorm
		mov	cx,ds
		inc	cx
		mov	ds,cx
		xor	si,si
#ifdef __BORLANDC__
	}
#endif
sinorm:
#ifdef __BORLANDC__
	__asm {
#endif
		mov	cl,1		// back to first bit
#ifdef __BORLANDC__
	}
#endif
sourceup:
#ifdef __BORLANDC__
	__asm {
#endif
		or	dh,dh		// if dx<256 its a byte, else move node
		jz	storebyte
		mov	bx,dx		// next node = (huffnode *)code
		jmp	expand
#ifdef __BORLANDC__
	}
#endif
storebyte:
#ifdef __BORLANDC__
	__asm {
#endif
		mov	[es:di],dl
		inc	di		// write a decopmpressed byte out
		mov	bx,[word ptr headptr]	// back to the head node for next bit

		cmp	di,0x10		// normalize es:di
		jb	dinorm
		mov	dx,es
		inc	dx
		mov	es,dx
		xor	di,di
#ifdef __BORLANDC__
	}
#endif
dinorm:
#ifdef __BORLANDC__
	__asm {
#endif
		sub	[WORD PTR ss:length],1
		jnc	expand
		dec	[WORD PTR ss:length+2]
		jns	expand		// when length = ffff ffff, done
	}
	}

	__asm {
		mov	ax,ss
		mov	ds,ax
	}

}


/*
======================
=
= CAL_CarmackExpand
=
= Length is the length of the EXPANDED data
=
======================
*/

#define NEARTAG	0xa7
#define FARTAG	0xa8

void CAL_CarmackExpand (unsigned far *source, unsigned far *dest, unsigned length)
{
	unsigned	ch,chhigh,count,offset;
	unsigned	far *copyptr, far *inptr, far *outptr;

	length/=2;

	inptr = source;
	outptr = dest;

	while (length)
	{
		ch = *inptr++;
		chhigh = ch>>8;
		if (chhigh == NEARTAG)
		{
			count = ch&0xff;
			if (!count)
			{				// have to insert a word containing the tag byte
				ch |= *(/*(unsigned char far *)*/inptr)++;
				*outptr++ = ch;
				length--;
			}
			else
			{
				offset = *(/*(unsigned char far *)*/inptr)++;
				copyptr = outptr - offset;
				length -= count;
				while (count--)
					*outptr++ = *copyptr++;
			}
		}
		else if (chhigh == FARTAG)
		{
			count = ch&0xff;
			if (!count)
			{				// have to insert a word containing the tag byte
				ch |= *(/*(unsigned char far *)*/inptr)++;
				*outptr++ = ch;
				length --;
			}
			else
			{
				offset = *inptr++;
				copyptr = dest + offset;
				length -= count;
				while (count--)
					*outptr++ = *copyptr++;
			}
		}
		else
		{
			*outptr++ = ch;
			length --;
		}
	}
}


/*
======================
=
= CA_RLEWcompress
=
======================
*/

long CA_RLEWCompress (unsigned far *source, long length, unsigned far *dest,
  unsigned rlewtag)
{
  long complength;
  unsigned value,count,i;
  unsigned far *start,far *end;

  start = dest;

  end = source + (length+1)/2;

//
// compress it
//
  do
  {
	count = 1;
	value = *source++;
	while (*source == value && source<end)
	{
	  count++;
	  source++;
	}
	if (count>3 || value == rlewtag)
	{
    //
    // send a tag / count / value string
    //
      *dest++ = rlewtag;
      *dest++ = count;
      *dest++ = value;
    }
    else
    {
    //
    // send word without compressing
    //
      for (i=1;i<=count;i++)
	*dest++ = value;
	}

  } while (source<end);

  complength = 2*(dest-start);
  return complength;
}


/*
======================
=
= CA_RLEWexpand
= length is EXPANDED length
=
======================
*/

void CA_RLEWexpand (unsigned far *source, unsigned far *dest,long length,
  unsigned rlewtag)
{
//  unsigned value,count,i;
  unsigned far *end;
  unsigned sourceseg,sourceoff,destseg,destoff,endseg,endoff;


//
// expand it
//
#if 0
  do
  {
	value = *source++;
	if (value != rlewtag)
	//
	// uncompressed
	//
	  *dest++=value;
	else
	{
	//
	// compressed string
	//
	  count = *source++;
	  value = *source++;
	  for (i=1;i<=count;i++)
	*dest++ = value;
	}
  } while (dest<end);
#endif

  end = dest + (length)/2;
  sourceseg = FP_SEG(source);
  sourceoff = FP_OFF(source);
  destseg = FP_SEG(dest);
  destoff = FP_OFF(dest);
  endseg = FP_SEG(end);
  endoff = FP_OFF(end);


//
// ax = source value
// bx = tag value
// cx = repeat counts
// dx = scratch
//
// NOTE: A repeat count that produces 0xfff0 bytes can blow this!
//
	__asm {
		mov	bx,rlewtag
		mov	si,sourceoff
		mov	di,destoff
		mov	es,destseg
		mov	ds,sourceseg
#ifdef __BORLANDC__
	}
#endif
expand:
#ifdef __BORLANDC__
	__asm {
#endif
		lodsw
		cmp	ax,bx
		je	repeat
		stosw
		jmp	next
#ifdef __BORLANDC__
	}
#endif
repeat:
#ifdef __BORLANDC__
	__asm {
#endif
		lodsw
		mov	cx,ax		// repeat count
		lodsw			// repeat value
		rep stosw
#ifdef __BORLANDC__
	}
#endif
next:
#ifdef __BORLANDC__
	__asm {
#endif
		cmp	si,0x10		// normalize ds:si
		jb	sinorm
		mov	ax,si
		shr	ax,1
		shr	ax,1
		shr	ax,1
		shr	ax,1
		mov	dx,ds
		add	dx,ax
		mov	ds,dx
		and	si,0xf
#ifdef __BORLANDC__
	}
#endif
sinorm:
#ifdef __BORLANDC__
	__asm {
#endif
		cmp	di,0x10		// normalize es:di
		jb	dinorm
		mov	ax,di
		shr	ax,1
		shr	ax,1
		shr	ax,1
		shr	ax,1
		mov	dx,es
		add	dx,ax
		mov	es,dx
		and	di,0xf
#ifdef __BORLANDC__
	}
#endif
dinorm:
#ifdef __BORLANDC__
	__asm {
#endif
		cmp     di,ss:endoff
		jne	expand
		mov	ax,es
		cmp	ax,ss:endseg
		jb	expand

		mov	ax,ss
		mov	ds,ax
	}
}


/*
=============================================================================

					 CACHE MANAGER ROUTINES

=============================================================================
*/

/*
======================
=
= CAL_SetupGrFile
=
======================
*/

void CAL_SetupGrFile (global_game_variables_t *gvar)
{
	char fname[13];
	int handle;
#if NUMPICS>0
	memptr compseg;
#endif

#ifdef GRHEADERLINKED

	gvar->ca.grhuffman = (huffnode *)&VGAdict;
	grstarts = (long _seg *)FP_SEG(&VGAhead);

	CAL_OptimizeNodes (gvar->ca.grhuffman);

#else

//
// load ???dict.ext (huffman dictionary for graphics files)
//

	strcpy(fname,GDICTNAME);
	strcat(fname,"hb1");

	if ((handle = open(fname,
		 O_RDONLY | O_BINARY, S_IREAD)) == -1)
		CA_CannotOpen(fname,gvar);

	read(handle, &gvar->ca.grhuffman, sizeof(gvar->ca.grhuffman));
	close(handle);
	CAL_OptimizeNodes (gvar->ca.grhuffman);
//
// load the data offsets from ???head.ext
//
	MM_GetPtr (MEMPTRCONV gvar->ca.grstarts,(NUMCHUNKS+1)*FILEPOSSIZE, gvar);

	strcpy(fname,GHEADNAME);
	strcat(fname,"hb1");

	if ((handle = open(fname,
		 O_RDONLY | O_BINARY, S_IREAD)) == -1)
		CA_CannotOpen(fname,gvar);

	CA_FarRead(handle, (memptr)gvar->ca.grstarts, (NUMCHUNKS+1)*FILEPOSSIZE, gvar);

	close(handle);


#endif

//
// Open the graphics file, leaving it open until the game is finished
//
	strcpy(fname,GFILENAME);
	strcat(fname,"hb1");

	gvar->ca.file.grhandle = open(fname, O_RDONLY | O_BINARY);
	if (gvar->ca.file.grhandle == -1)
		CA_CannotOpen(fname,gvar);


//
// load the pic and sprite headers into the arrays in the data segment
//
#if NUMPICS>0
	MM_GetPtr(MEMPTRCONV gvar->video.pictable,NUMPICS*sizeof(pictabletype),gvar);
	CAL_GetGrChunkLength(STRUCTPIC,gvar);		// position file pointer
	printf("CAL_SetupGrFile:\n");
	printf("	gvar->ca.chunkcomplen size is %lu\n", gvar->ca.chunkcomplen);
	MM_GetPtr(MEMPTRANDPERCONV compseg,gvar->ca.chunkcomplen,gvar);								IN_Ack(gvar);
	CA_FarRead (gvar->ca.file.grhandle,compseg,gvar->ca.chunkcomplen,gvar);
	CAL_HuffExpand (compseg, (byte far *)gvar->video.pictable,NUMPICS*sizeof(pictabletype),gvar->ca.grhuffman);
	MM_FreePtr(MEMPTRANDPERCONV compseg,gvar);
#endif

#if 0
	//NUMPICM>0
	MM_GetPtr(MEMPTRCONV picmtable,NUMPICM*sizeof(pictabletype));
	CAL_GetGrChunkLength(STRUCTPICM);		// position file pointer
	MM_GetPtr(&compseg,gvar->ca.chunkcomplen);
	CA_FarRead (gvar->ca.file.grhandle,compseg,gvar->ca.chunkcomplen);
	CAL_HuffExpand (compseg, (byte far *)picmtable,NUMPICS*sizeof(pictabletype),gvar->ca.grhuffman);
	MM_FreePtr(&compseg);
//#endif

//#if NUMSPRITES>0
	MM_GetPtr(MEMPTRCONV spritetable,NUMSPRITES*sizeof(spritetabletype));
	CAL_GetGrChunkLength(STRUCTSPRITE);	// position file pointer
	MM_GetPtr(&compseg,gvar->ca.chunkcomplen);
	CA_FarRead (gvar->ca.file.grhandle,compseg,gvar->ca.chunkcomplen);
	CAL_HuffExpand (compseg, (byte far *)spritetable,NUMSPRITES*sizeof(spritetabletype),gvar->ca.grhuffman);
	MM_FreePtr(&compseg);
#endif

}

//==========================================================================


/*
======================
=
= CAL_SetupMapFile
=
======================
*/

void CAL_SetupMapFile (global_game_variables_t *gvar)
{
#ifndef MAPHEADERLINKED
	int handle;
	long length;
#endif

//
// load maphead.ext (offsets and tileinfo for map file)
//
#ifndef MAPHEADERLINKED
	if ((handle = open("maphead.mph",
		 O_RDONLY | O_BINARY, S_IREAD)) == -1)
		Quit (gvar, "Can't open maphead.mph");
	length = filelength(handle);
	MM_GetPtr (MEMPTRCONV gvar->ca.tinf,length,gvar);
	CA_FarRead(handle, gvar->ca.tinf, length,gvar);
	close(handle);
#else

	gvar->ca.tinf = (byte _seg *)FP_SEG(&maphead);

#endif

//
// open the data file
//
//TODO: multiple files
	if ((gvar->ca.file.maphandle = open("data/test.map",
		 O_RDONLY | O_BINARY, S_IREAD)) == -1)
		Quit (gvar, "Can't open data/test.map!");
/*#ifdef MAPHEADERLINKED
	if ((maphandle = open("GAMEMAPS.16"ENSION,
		 O_RDONLY | O_BINARY, S_IREAD)) == -1)
		Quit (gvar, "Can't open GAMEMAPS.16"ENSION"!");
#else
	if ((maphandle = open("MAPTEMP.16"ENSION,
		 O_RDONLY | O_BINARY, S_IREAD)) == -1)
		Quit (gvar, "Can't open MAPTEMP.16"ENSION"!");
#endif*/
}

//==========================================================================


/*
======================
=
= CAL_SetupAudioFile
=
======================
*/

/*void CAL_SetupAudioFile (void)
{
	int handle;
	long length;

//
// load maphead.ext (offsets and tileinfo for map file)
//
#ifndef AUDIOHEADERLINKED
	if ((handle = open("AUDIOHED.16",
		 O_RDONLY | O_BINARY, S_IREAD)) == -1)
		Quit (gvar, "Can't open AUDIOHED.16""!");
	length = filelength(handle);
	MM_GetPtr (MEMPTRCONV audiostarts,length);
	CA_FarRead(handle, (byte far *)audiostarts, length);
	close(handle);
#else
	audiohuffman = (huffnode *)&audiodict;
	CAL_OptimizeNodes (audiohuffman);
	audiostarts = (long _seg *)FP_SEG(&audiohead);
#endif

//
// open the data file
//
#ifndef AUDIOHEADERLINKED
	if ((audiohandle = open("AUDIOT.16",
		 O_RDONLY | O_BINARY, S_IREAD)) == -1)
		Quit (gvar, "Can't open AUDIOT.16""!");
#else
	if ((audiohandle = open("AUDIO.16",
		 O_RDONLY | O_BINARY, S_IREAD)) == -1)
		Quit (gvar, "Can't open AUDIO.16""!");
#endif
}*/

//==========================================================================


/*
======================
=
= CA_Startup
=
= Open all files and load in headers
=
======================
*/

void CA_Startup(global_game_variables_t *gvar)
{
#ifdef PROFILE
#ifdef __BORLANDC__
	unlink("profile.16b");
	gvar->handle.profilehandle = open("profile.16b", O_CREAT | O_WRONLY | O_TEXT);
#endif
#ifdef __WATCOMC__
	unlink("profile.16w");
	gvar->handle.profilehandle = open("profile.16w", O_CREAT | O_WRONLY | O_TEXT);
#endif
#endif//profile

#ifdef SHOWMEMINFO
#ifdef __BORLANDC__
	unlink("meminfo.16b");
	gvar->handle.showmemhandle = open("meminfo.16b", O_CREAT | O_WRONLY | O_TEXT);
#endif
#ifdef __WATCOMC__
	unlink("meminfo.16w");
	gvar->handle.showmemhandle = open("meminfo.16w", O_CREAT | O_WRONLY | O_TEXT);
#endif
#endif


#ifndef NOMAPS
	CAL_SetupMapFile (gvar);
#endif
#ifndef NOGRAPHICS
	CAL_SetupGrFile (gvar);
#endif
#ifndef NOAUDIO
	CAL_SetupMapFile (gvar);
#endif

	gvar->ca.mapon = -1;
	gvar->ca.ca_levelbit = 1;
	gvar->ca.ca_levelnum = 0;

/*	drawcachebox	= CAL_DialogDraw;
	updatecachebox  = CAL_DialogUpdate;
	finishcachebox	= CAL_DialogFinish;*/
}

//==========================================================================


/*
======================
=
= CA_Shutdown
=
= Closes all files
=
======================
*/

void CA_Shutdown(global_game_variables_t *gvar)
{
#ifdef PROFILE
	close(gvar->handle.profilehandle);
#endif
#ifdef SHOWMEMINFO
 	close(gvar->handle.showmemhandle);
#endif

	close(gvar->ca.file.maphandle);
	close(gvar->ca.file.grhandle);
	close(gvar->ca.file.audiohandle);
}

//===========================================================================

/*
======================
=
= CA_CacheAudioChunk
=
======================
*/
/*++++
void CA_CacheAudioChunk (int chunk)
{
	long	pos,compressed;
#ifdef AUDIOHEADERLINKED
	long	expanded;
	memptr	bigbufferseg;
	byte	far *source;
#endif

	if (audiosegs[chunk])
	{
		MM_SetPurge (MEMPTRCONV audiosegs[chunk],0);
		return;							// allready in memory
	}

// MDM begin - (GAMERS EDGE)
//
	if (!FindFile("AUDIO.16",NULL,2))
		Quit (gvar, "CA_CacheAudioChunk(): Can't find audio files.");
//
// MDM end

//
// load the chunk into a buffer, either the miscbuffer if it fits, or allocate
// a larger buffer
//
	pos = audiostarts[chunk];
	compressed = audiostarts[chunk+1]-pos;

	lseek(audiohandle,pos,SEEK_SET);

#ifndef AUDIOHEADERLINKED

	MM_GetPtr (MEMPTRCONV audiosegs[chunk],compressed);
	if (mmerror)
		return;

	CA_FarRead(audiohandle,audiosegs[chunk],compressed);

#else

	if (compressed<=BUFFERSIZE)
	{
		CA_FarRead(audiohandle,bufferseg,compressed);
		source = bufferseg;
	}
	else
	{
		MM_GetPtr(&bigbufferseg,compressed);
		if (mmerror)
			return;
		MM_SetLock (&bigbufferseg,true);
		CA_FarRead(audiohandle,bigbufferseg,compressed);
		source = bigbufferseg;
	}

	expanded = *(long far *)source;
	source += 4;			// skip over length
	MM_GetPtr (MEMPTRCONV audiosegs[chunk],expanded);
	if (mmerror)
		goto done;
	CAL_HuffExpand (source,audiosegs[chunk],expanded,audiohuffman);

done:
	if (compressed>BUFFERSIZE)
		MM_FreePtr(&bigbufferseg);
#endif
}*/

//===========================================================================

/*
======================
=
= CA_LoadAllSounds
=
= Purges all sounds, then loads all new ones (mode switch)
=
======================
*/
/*++++
void CA_LoadAllSounds (void)
{
	unsigned	start,i;

	switch (oldsoundmode)
	{
	case sdm_Off:
		goto cachein;
	case sdm_PC:
		start = STARTPCSOUNDS;
		break;
	case sdm_AdLib:
		start = STARTADLIBSOUNDS;
		break;
	}

	for (i=0;i<NUMSOUNDS;i++,start++)
		if (audiosegs[start])
			MM_SetPurge (MEMPTRCONV audiosegs[start],3);		// make purgable

cachein:

	switch (SoundMode)
	{
	case sdm_Off:
		return;
	case sdm_PC:
		start = STARTPCSOUNDS;
		break;
	case sdm_AdLib:
		start = STARTADLIBSOUNDS;
		break;
	}

	for (i=0;i<NUMSOUNDS;i++,start++)
		CA_CacheAudioChunk (start);

	oldsoundmode = SoundMode;
}*/

//===========================================================================

//????#if GRMODE == EGAGR

/*
======================
=
= CAL_ShiftSprite
=
= Make a shifted (one byte wider) copy of a sprite into another area
=
======================
*/
/*++++
unsigned	static	sheight,swidth;
boolean static dothemask;

void CAL_ShiftSprite (unsigned segment,unsigned source,unsigned dest,
	unsigned width, unsigned height, unsigned pixshift, boolean domask, global_game_variables_t *gvar)
{

	sheight = height;		// because we are going to reassign bp
	swidth = width;
	dothemask = domask;

	__asm {
		mov	ax,[segment]
		mov	ds,ax		// source and dest are in same segment, and all local

		mov	bx,[source]
		mov	di,[dest]

		mov	bp,[pixshift]
		shl	bp,1
		mov	bp,WORD PTR [shifttabletable+bp]	// bp holds pointer to shift table
//		mov	bp,WORD PTR [gvar->video.shifttabletable+bp]	// bp holds pointer to shift table

		cmp	[ss:dothemask],0
		je		skipmask

//
// table shift the mask
//
		mov	dx,[ss:sheight]
#ifdef __BORLANDC__
	}
#endif
domaskrow:
#ifdef __BORLANDC__
	__asm {
#endif
		mov	BYTE PTR [di],255	// 0xff first byte
		mov	cx,ss:[swidth]
#ifdef __BORLANDC__
	}
#endif
domaskbyte:
#ifdef __BORLANDC__
	__asm {
#endif
		mov	al,[bx]				// source
		not	al
		inc	bx					// next source byte
		xor	ah,ah
		shl	ax,1
		mov	si,ax
		mov	ax,[bp+si]			// table shift into two bytes
		not	ax
		and	[di],al				// and with first byte
		inc	di
		mov	[di],ah				// replace next byte

		loop	domaskbyte

		inc	di					// the last shifted byte has 1s in it
		dec	dx
		jnz	domaskrow
#ifdef __BORLANDC__
	}
#endif
skipmask:
#ifdef __BORLANDC__
	__asm {
#endif
//
// table shift the data
//
		mov	dx,ss:[sheight]
		shl	dx,1
		shl	dx,1				// four planes of data
#ifdef __BORLANDC__
	}
#endif
dodatarow:
#ifdef __BORLANDC__
	__asm {
#endif
		mov	BYTE PTR [di],0		// 0 first byte
		mov	cx,ss:[swidth]
#ifdef __BORLANDC__
	}
#endif
dodatabyte:
#ifdef __BORLANDC__
	__asm {
#endif
		mov	al,[bx]				// source
		inc	bx					// next source byte
		xor	ah,ah
		shl	ax,1
		mov	si,ax
		mov	ax,[bp+si]			// table shift into two bytes
		or	[di],al				// or with first byte
		inc	di
		mov	[di],ah				// replace next byte

		loop	dodatabyte

		inc	di					// the last shifted byte has 0s in it
		dec	dx
		jnz	dodatarow

//
// done
//

		mov	ax,ss				// restore data segment
		mov	ds,ax
	}

}

#endif
*/
//===========================================================================

/*
======================
=
= CAL_CacheSprite
=
= Generate shifts and set up sprite structure for a given sprite
=
======================
*/
/*++++
void CAL_CacheSprite (int chunk, byte far *compressed, global_game_variables_t *gvar)
{
	int i;
	unsigned shiftstarts[5];
	unsigned smallplane,bigplane,expanded;
	spritetabletype far *spr;
	spritetype _seg *dest;

#if 0
//GRMODE == CGAGR
//
// CGA has no pel panning, so shifts are never needed
//
	spr = &gvar->video.spritetable[chunk-STARTSPRITES];
	smallplane = spr->width*spr->height;
	MM_GetPtr (&grsegs[chunk],smallplane*2+MAXSHIFTS*6);
	if (mmerror)
		return;
	dest = (spritetype _seg *)grsegs[chunk];
	dest->sourceoffset[0] = MAXSHIFTS*6;	// start data after 3 unsigned tables
	dest->planesize[0] = smallplane;
	dest->width[0] = spr->width;

//
// expand the unshifted shape
//
	CAL_HuffExpand (compressed, &dest->data[0],smallplane*2,gvar->ca.grhuffman);

#endif


//#if GRMODE == EGAGR

//
// calculate sizes
//
	spr = &gvar->video.spritetable[chunk-STARTSPRITES];
	smallplane = spr->width*spr->height;
	bigplane = (spr->width+1)*spr->height;

	shiftstarts[0] = MAXSHIFTS*6;	// start data after 3 unsigned tables
	shiftstarts[1] = shiftstarts[0] + smallplane*5;	// 5 planes in a sprite
	shiftstarts[2] = shiftstarts[1] + bigplane*5;
	shiftstarts[3] = shiftstarts[2] + bigplane*5;
	shiftstarts[4] = shiftstarts[3] + bigplane*5;	// nothing ever put here

	expanded = shiftstarts[spr->shifts];
	MM_GetPtr (MEMPTRCONV gvar->ca.grsegs[chunk],expanded, gvar);
	if (gvar->mm.mmerror)
		return;
	dest = (spritetype _seg *)gvar->ca.grsegs[chunk];

//
// expand the unshifted shape
//
	CAL_HuffExpand (compressed, &dest->data[0],smallplane*5,gvar->ca.grhuffman);

//
// make the shifts!
//
	switch (spr->shifts)
	{
	case	1:
		for (i=0;i<4;i++)
		{
			dest->sourceoffset[i] = shiftstarts[0];
			dest->planesize[i] = smallplane;
			dest->width[i] = spr->width;
		}
		break;

	case	2:
		for (i=0;i<2;i++)
		{
			dest->sourceoffset[i] = shiftstarts[0];
			dest->planesize[i] = smallplane;
			dest->width[i] = spr->width;
		}
		for (i=2;i<4;i++)
		{
			dest->sourceoffset[i] = shiftstarts[1];
			dest->planesize[i] = bigplane;
			dest->width[i] = spr->width+1;
		}
		CAL_ShiftSprite ((unsigned)gvar->ca.grsegs[chunk],dest->sourceoffset[0],
			dest->sourceoffset[2],spr->width,spr->height,4,true,gvar);
		break;

	case	4:
		dest->sourceoffset[0] = shiftstarts[0];
		dest->planesize[0] = smallplane;
		dest->width[0] = spr->width;

		dest->sourceoffset[1] = shiftstarts[1];
		dest->planesize[1] = bigplane;
		dest->width[1] = spr->width+1;
		CAL_ShiftSprite ((unsigned)gvar->ca.grsegs[chunk],dest->sourceoffset[0],
			dest->sourceoffset[1],spr->width,spr->height,2,true,gvar);

		dest->sourceoffset[2] = shiftstarts[2];
		dest->planesize[2] = bigplane;
		dest->width[2] = spr->width+1;
		CAL_ShiftSprite ((unsigned)gvar->ca.grsegs[chunk],dest->sourceoffset[0],
			dest->sourceoffset[2],spr->width,spr->height,4,true,gvar);

		dest->sourceoffset[3] = shiftstarts[3];
		dest->planesize[3] = bigplane;
		dest->width[3] = spr->width+1;
		CAL_ShiftSprite ((unsigned)gvar->ca.grsegs[chunk],dest->sourceoffset[0],
			dest->sourceoffset[3],spr->width,spr->height,6,true,gvar);

		break;

	default:
		Quit (gvar, "CAL_CacheSprite: Bad shifts number!");
	}

//#endif
}*/

//===========================================================================


/*
======================
=
= CAL_ExpandGrChunk
=
= Does whatever is needed with a pointer to a compressed chunk
=
======================
*/
/*++++
void CAL_ExpandGrChunk (int chunk, byte far *source)
{
	long	expanded;


	if (chunk >= STARTTILE8 && chunk < STARTEXTERNS)
	{
	//
	// expanded sizes of tile8/16/32 are implicit
	//

#if GRMODE == EGAGR
#define BLOCK		32
#define MASKBLOCK	40
#endif

#if GRMODE == CGAGR
#define BLOCK		16
#define MASKBLOCK	32
#endif

		if (chunk<STARTTILE8M)			// tile 8s are all in one chunk!
			expanded = BLOCK*NUMTILE8;
		else if (chunk<STARTTILE16)
			expanded = MASKBLOCK*NUMTILE8M;
		else if (chunk<STARTTILE16M)	// all other tiles are one/chunk
			expanded = BLOCK*4;
		else if (chunk<STARTTILE32)
			expanded = MASKBLOCK*4;
		else if (chunk<STARTTILE32M)
			expanded = BLOCK*16;
		else
			expanded = MASKBLOCK*16;
	}
	else
	{
	//
	// everything else has an explicit size longword
	//
		expanded = *(long far *)source;
		source += 4;			// skip over length
	}

//
// allocate final space, decompress it, and free bigbuffer
// Sprites need to have shifts made and various other junk
//
	if (chunk>=STARTSPRITES && chunk< STARTTILE8)
		CAL_CacheSprite(chunk,source);
	else
	{
		MM_GetPtr (&grsegs[chunk],expanded);
		if (mmerror)
			return;
		CAL_HuffExpand (source,grsegs[chunk],expanded,gvar->ca.grhuffman);
	}
}
*/

/*
======================
=
= CAL_ReadGrChunk
=
= Gets a chunk off disk, optimizing reads to general buffer
=
======================
*/
/*++++
void CAL_ReadGrChunk (int chunk)
{
	long	pos,compressed;
	memptr	bigbufferseg;
	byte	far *source;
	int		next;

//
// load the chunk into a buffer, either the miscbuffer if it fits, or allocate
// a larger buffer
//
	pos = GRFILEPOS(chunk,gvar);
	if (pos<0)							// $FFFFFFFF start is a sparse tile
	  return;

	next = chunk +1;
	while (GRFILEPOS(next,gvar) == -1)		// skip past any sparse tiles
		next++;

	compressed = GRFILEPOS(next,gvar)-pos;

	lseek(gvar->ca.file.grhandle,pos,SEEK_SET);

	if (compressed<=BUFFERSIZE)
	{
		CA_FarRead(gvar->ca.file.grhandle,bufferseg,compressed);
		source = bufferseg;
	}
	else
	{
		MM_GetPtr(&bigbufferseg,compressed);
		if (mmerror)
			return;
		MM_SetLock (&bigbufferseg,true);
		CA_FarRead(gvar->ca.file.grhandle,bigbufferseg,compressed);
		source = bigbufferseg;
	}

	CAL_ExpandGrChunk (chunk,source);

	if (compressed>BUFFERSIZE)
		MM_FreePtr(&bigbufferseg);
}
*/
/*
======================
=
= CA_CacheGrChunk
=
= Makes sure a given chunk is in memory, loadiing it if needed
=
======================
*/
/*++++
void CA_CacheGrChunk (int chunk)
{
	long	pos,compressed;
	memptr	bigbufferseg;
	byte	far *source;
	int		next;

	gvar->ca.grneeded[chunk] |= ca_levelbit;		// make sure it doesn't get removed
	if (grsegs[chunk])
	{
		MM_SetPurge (&grsegs[chunk],0);
		return;							// allready in memory
	}

// MDM begin - (GAMERS EDGE)
//
	if (!FindFile("EGAGRAPH.16",NULL,2))
		Quit (gvar, "CA_CacheGrChunk(): Can't find graphics files.");
//
// MDM end

//
// load the chunk into a buffer, either the miscbuffer if it fits, or allocate
// a larger buffer
//
	pos = GRFILEPOS(chunk,gvar);
	if (pos<0)							// $FFFFFFFF start is a sparse tile
	  return;

	next = chunk +1;
	while (GRFILEPOS(next) == -1)		// skip past any sparse tiles
		next++;

	compressed = GRFILEPOS(next,gvar)-pos;

	lseek(gvar->ca.file.grhandle,pos,SEEK_SET);

	if (compressed<=BUFFERSIZE)
	{
		CA_FarRead(gvar->ca.file.grhandle,bufferseg,compressed);
		source = bufferseg;
	}
	else
	{
		MM_GetPtr(&bigbufferseg,compressed);
		MM_SetLock (&bigbufferseg,true);
		CA_FarRead(gvar->ca.file.grhandle,bigbufferseg,compressed);
		source = bigbufferseg;
	}

	CAL_ExpandGrChunk (chunk,source);

	if (compressed>BUFFERSIZE)
		MM_FreePtr (MEMPTRCONV bigbufferseg);
}
*/


//==========================================================================

/*
======================
=
= CA_CacheMap
=
======================
*/

void CA_CacheMap (global_game_variables_t *gvar)
{
	long	pos,compressed;
	int		plane;
	memptr	*dest,bigbufferseg;
	unsigned	size;
	unsigned	far	*source;
#ifdef MAPHEADERLINKED
	memptr	buffer2seg;
	long	expanded;
#endif


//
// free up memory from last map
//
	if (gvar->ca.mapon>-1 && gvar->ca.mapheaderseg[gvar->ca.mapon])
		MM_SetPurge ((MEMPTRCONV gvar->ca.mapheaderseg[(gvar->ca.mapon)]), 3, gvar);
	for (plane=0;plane<MAPPLANES;plane++)
		if (gvar->ca.mapsegs[plane])
			MM_FreePtr (MEMPTRCONV gvar->ca.mapsegs[plane], gvar);

	gvar->ca.mapon = gvar->ca.mapnum;


//
// load map header
// The header will be cached if it is still around
//
	if (!gvar->ca.mapheaderseg[gvar->ca.mapnum])
	{
		pos = ((mapfiletype	_seg *)gvar->ca.tinf)->headeroffsets[gvar->ca.mapnum];
		if (pos<0)						// $FFFFFFFF start is a sparse map
			Quit (gvar, "CA_CacheMap: Tried to load a non existent map!");

		MM_GetPtr(MEMPTRCONV gvar->ca.mapheaderseg[gvar->ca.mapnum],sizeof(maptype), gvar);
		lseek(gvar->ca.file.maphandle,pos,SEEK_SET);
#ifdef MAPHEADERLINKED
//#if BUFFERSIZE < sizeof(maptype)
//The general buffer size is too small!
//#endif
		//
		// load in, then unhuffman to the destination
		//
		CA_FarRead (gvar->ca.file.maphandle,gvar->mm.bufferseg,((mapfiletype	_seg *)gvar->ca.tinf)->headersize[gvar->ca.mapnum], gvar);
		CAL_HuffExpand ((byte huge *)bufferseg,
			(byte huge *)gvar->ca.mapheaderseg[gvar->ca.mapnum],sizeof(maptype),maphuffman, gvar);
#else
		CA_FarRead (gvar->ca.file.maphandle,(memptr)gvar->ca.mapheaderseg[gvar->ca.mapnum],sizeof(maptype), gvar);
#endif
	}
	else
		MM_SetPurge (MEMPTRCONV gvar->ca.mapheaderseg[gvar->ca.mapnum], 0, gvar);

//
// load the planes in
// If a plane's pointer still exists it will be overwritten (levels are
// allways reloaded, never cached)
//

	size = gvar->ca.mapheaderseg[gvar->ca.mapnum]->width * gvar->ca.mapheaderseg[gvar->ca.mapnum]->height * 2;

	for (plane = 0; plane<MAPPLANES; plane++)
	{
		pos = gvar->ca.mapheaderseg[gvar->ca.mapnum]->planestart[plane];
		compressed = gvar->ca.mapheaderseg[gvar->ca.mapnum]->planelength[plane];

		if (!compressed)
			continue;		// the plane is not used in this game

		dest = MEMPTRCONV gvar->ca.mapsegs[plane];
		MM_GetPtr(dest,size, gvar);

		lseek(gvar->ca.file.maphandle,pos,SEEK_SET);
		if (compressed<=BUFFERSIZE)
			source = gvar->mm.bufferseg;
		else
		{
			MM_GetPtr(MEMPTRCONV bigbufferseg,compressed, gvar);
			MM_SetLock (MEMPTRCONV bigbufferseg,true, gvar);
			source = bigbufferseg;
		}

		CA_FarRead(gvar->ca.file.maphandle,(byte far *)source,compressed, gvar);
#ifdef MAPHEADERLINKED
		//
		// unhuffman, then unRLEW
		// The huffman'd chunk has a two byte expanded length first
		// The resulting RLEW chunk also does, even though it's not really
		// needed
		//
		expanded = *source;
		source++;
		MM_GetPtr (&buffer2seg,expanded, gvar);
		CAL_CarmackExpand (source, (unsigned far *)buffer2seg,expanded);
		CA_RLEWexpand (((unsigned far *)buffer2seg)+1,*dest,size,
		((mapfiletype _seg *)tinf)->RLEWtag);
		MM_FreePtr (&buffer2seg);

#else
		//
		// unRLEW, skipping expanded length
		//
		CA_RLEWexpand (source+1, *dest,size,
		((mapfiletype _seg *)gvar->ca.tinf)->RLEWtag);
#endif

		if (compressed>BUFFERSIZE)
			MM_FreePtr(MEMPTRCONV bigbufferseg, gvar);
	}
}

//===========================================================================

/*
======================
=
= CA_UpLevel
=
= Goes up a bit level in the needed lists and clears it out.
= Everything is made purgable
=
======================
*/

void CA_UpLevel (global_game_variables_t *gvar)
{
	if (gvar->ca.ca_levelnum==7)
		printf("CA_UpLevel: Up past level 7!");

	gvar->ca.ca_levelbit<<=1;
	gvar->ca.ca_levelnum++;
}

//===========================================================================

/*
======================
=
= CA_DownLevel
=
= Goes down a bit level in the needed lists and recaches
= everything from the lower level
=
======================
*/

void CA_DownLevel (global_game_variables_t *gvar)
{
	if (!gvar->ca.ca_levelnum)
		printf("CA_DownLevel: Down past level 0!");
	gvar->ca.ca_levelbit>>=1;
	gvar->ca.ca_levelnum--;
	////++++++++++++++++++++++++++++++++++++++++++CA_CacheMarks(NULL);
}

//===========================================================================

/*
======================
=
= CA_ClearMarks
=
= Clears out all the marks at the current level
=
======================
*/

void CA_ClearMarks (global_game_variables_t *gvar)
{
	int i;

	for (i=0;i<NUMCHUNKS;i++)
		gvar->ca.grneeded[i]&=~gvar->ca.ca_levelbit;
}

//===========================================================================

/*
======================
=
= CA_ClearAllMarks
=
= Clears out all the marks on all the levels
=
======================
*/

void CA_ClearAllMarks (global_game_variables_t *gvar)
{
	_fmemset (gvar->ca.grneeded,0,sizeof(gvar->ca.grneeded));
	gvar->ca.ca_levelbit = 1;
	gvar->ca.ca_levelnum = 0;
}

//===========================================================================

/*
======================
=
= CA_FreeGraphics
=
======================
*/

void CA_SetGrPurge (global_game_variables_t *gvar)
{
	int i;

//
// free graphics
//
	CA_ClearMarks (gvar);

	for (i=0;i<NUMCHUNKS;i++)
		if (gvar->ca.grsegs[i])
			MM_SetPurge (MEMPTRCONV gvar->ca.grsegs[i],3, gvar);
}


/*
======================
=
= CA_SetAllPurge
=
= Make everything possible purgable
=
======================
*/

void CA_SetAllPurge (global_game_variables_t *gvar)
{
	int i;

	CA_ClearMarks (gvar);

//
// free cursor sprite and background save
//
	//____VW_FreeCursor ();

//
// free map headers and map planes
//
	for (i=0;i<NUMMAPS;i++)
		if (gvar->ca.mapheaderseg[i])
			MM_SetPurge (MEMPTRCONV gvar->ca.mapheaderseg[i],3, gvar);

	for (i=0;i<3;i++)
		if (gvar->ca.mapsegs[i])
			MM_FreePtr (MEMPTRCONV gvar->ca.mapsegs[i], gvar);

//
// free sounds
//
	for (i=0;i<NUMSNDCHUNKS;i++)
		if (gvar->ca.audiosegs[i])
			MM_SetPurge (MEMPTRCONV gvar->ca.audiosegs[i],3, gvar);

//
// free graphics
//
	CA_SetGrPurge (gvar);
}


//===========================================================================


/*
======================
=
= CAL_DialogDraw
=
======================
*/
/*
#define NUMBARS	(17l*8)
#define BARSTEP	8

unsigned	thx,thy,lastx;
long		barx,barstep;

void	CAL_DialogDraw (char *title,unsigned numcache)
{
	unsigned	homex,homey,x;

	barstep = (NUMBARS<<16)/numcache;

//
// draw dialog window (masked tiles 12 - 20 are window borders)
//
	US_CenterWindow (20,8);
	homex = PrintX;
	homey = PrintY;

	US_CPrint ("Loading");
	fontcolor = F_SECONDCOLOR;
	US_CPrint (title);
	fontcolor = F_BLACK;

//
// draw thermometer bar
//
	thx = homex + 8;
	thy = homey + 32;
	VWB_DrawTile8(thx,thy,0);		// CAT3D numbers
	VWB_DrawTile8(thx,thy+8,3);
	VWB_DrawTile8(thx,thy+16,6);
	VWB_DrawTile8(thx+17*8,thy,2);
	VWB_DrawTile8(thx+17*8,thy+8,5);
	VWB_DrawTile8(thx+17*8,thy+16,8);
	for (x=thx+8;x<thx+17*8;x+=8)
	{
		VWB_DrawTile8(x,thy,1);
		VWB_DrawTile8(x,thy+8,4);
		VWB_DrawTile8(x,thy+16,7);
	}

	thx += 4;		// first line location
	thy += 5;
	barx = (long)thx<<16;
	lastx = thx;

	VW_UpdateScreen();
}
*/

/*
======================
=
= CAL_DialogUpdate
=
======================
*/
/*
void	CAL_DialogUpdate (void)
{
	unsigned	x,xh;

	barx+=barstep;
	xh = barx>>16;
	if (xh - lastx > BARSTEP)
	{
		for (x=lastx;x<=xh;x++)
#if GRMODE == EGAGR
			VWB_Vlin (thy,thy+13,x,14);
#endif
#if GRMODE == CGAGR
			VWB_Vlin (thy,thy+13,x,SECONDCOLOR);
#endif
		lastx = xh;
		VW_UpdateScreen();
	}
}*/

/*
======================
=
= CAL_DialogFinish
=
======================
*/
/*
void	CAL_DialogFinish (void)
{
	unsigned	x,xh;

	xh = thx + NUMBARS;
	for (x=lastx;x<=xh;x++)
#if GRMODE == EGAGR
		VWB_Vlin (thy,thy+13,x,14);
#endif
#if GRMODE == CGAGR
		VWB_Vlin (thy,thy+13,x,SECONDCOLOR);
#endif
	VW_UpdateScreen();

}*/

//===========================================================================

/*
======================
=
= CA_CacheMarks
=
======================
*/
#define MAXEMPTYREAD	1024
/*++++ segments
void CAL_CacheMarks (char *title, global_game_variables_t *gvar)
{
	boolean dialog;
	int 	i,next,numcache;
	long	pos,endpos,nextpos,nextendpos,compressed;
	long	bufferstart,bufferend;	// file position of general buffer
	byte	far *source;
	memptr	bigbufferseg;

	dialog = (title!=NULL);

	numcache = 0;
//
// go through and make everything not needed purgable
//
	for (i=0;i<NUMCHUNKS;i++)
		if (gvar->ca.grneeded[i]&(gvar->ca.ca_levelbit))//if (grneeded[i]&ca_levelbit)
		{
			if (gvar->ca.grsegs[i])					// its allready in memory, make
				MM_SetPurge(gvar->ca.grsegs[i],0,gvar);	// sure it stays there!
			else
				numcache++;
		}
		else
		{
			if (gvar->ca.grsegs[i])					// not needed, so make it purgeable
				MM_SetPurge(gvar->ca.grsegs[i],3,gvar);
		}

	if (!numcache)			// nothing to cache!
		return;

// MDM begin - (GAMERS EDGE)
//
//????	if (!FindFile("EGAGRAPH.16",NULL,2))
//????		Quit (gvar, "CA_CacheMarks(): Can't find graphics files.");
//
// MDM end

	if (dialog)
	{
#ifdef PROFILE
		write(profilehandle,title,strlen(title));
		write(profilehandle,"\n",1);
#endif
		if (drawcachebox)
			drawcachebox(title,numcache);
	}

//
// go through and load in anything still needed
//
	bufferstart = bufferend = 0;		// nothing good in buffer now

	for (i=0;i<NUMCHUNKS;i++)
		if ( (gvar->ca.grneeded[i]&(gvar->ca.ca_levelbit)) && !gvar->ca.grsegs[i])
		{
//
// update thermometer
//
			if (dialog && updatecachebox)
				updatecachebox ();

			pos = GRFILEPOS(i,gvar);
			if (pos<0)
				continue;

			next = i +1;
			while (GRFILEPOS(next) == -1)		// skip past any sparse tiles
				next++;

			compressed = GRFILEPOS(next,gvar)-pos;
			endpos = pos+compressed;

			if (compressed<=BUFFERSIZE)
			{
				if (bufferstart<=pos
				&& bufferend>= endpos)
				{
				// data is allready in buffer
					source = (byte _seg *)bufferseg+(pos-bufferstart);
				}
				else
				{
				// load buffer with a new block from disk
				// try to get as many of the needed blocks in as possible
					while ( next < NUMCHUNKS )
					{
						while (next < NUMCHUNKS &&
						!(gvar->video.grneeded[next]&ca_levelbit && !grsegs[next]))
							next++;
						if (next == NUMCHUNKS)
							continue;

						nextpos = GRFILEPOS(next,gvar);
						while (GRFILEPOS(++next,gvar) == -1)	// skip past any sparse tiles
							;
						nextendpos = GRFILEPOS(next,gvar);
						if (nextpos - endpos <= MAXEMPTYREAD
						&& nextendpos-pos <= BUFFERSIZE)
							endpos = nextendpos;
						else
							next = NUMCHUNKS;			// read pos to posend
					}

					lseek(gvar->ca.file.grhandle,pos,SEEK_SET);
					CA_FarRead(gvar->ca.file.grhandle,(gvar->mm.bufferseg),endpos-pos,gvar);
					bufferstart = pos;
					bufferend = endpos;
					source = bufferseg;
				}
			}
			else
			{
			// big chunk, allocate temporary buffer
				MM_GetPtr(&bigbufferseg,compressed,gvar);
				if (mmerror)
					return;
				MM_SetLock (&bigbufferseg,true);
				lseek(gvar->ca.file.grhandle,pos,SEEK_SET);
				CA_FarRead(gvar->ca.file.grhandle,bigbufferseg,compressed,gvar);
				source = bigbufferseg;
			}

			CAL_ExpandGrChunk (i,source);
			if (mmerror)
				return;

			if (compressed>BUFFERSIZE)
				MM_FreePtr(&bigbufferseg);

		}

//
// finish up any thermometer remnants
//
		if (dialog && finishcachebox)
			finishcachebox();
}//*/

void CA_CannotOpen(char *string, global_game_variables_t *gvar)
{
 char str[30];

 strcpy(str,"Can't open ");
 strcat(str,string);
 strcat(str,"!\n");
 Quit (gvar, str);
}
