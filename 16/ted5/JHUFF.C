#include "ted5.h"
#pragma hdrstop

long inlength,outlength;

long counts[256];

unsigned huffbits[256];
unsigned long huffstring[256];

huffnode nodearray[256];	// 256 nodes is worst case

void CountBytes (unsigned char huge *start, long length);
void Huffmanize (void);
void OptimizeNodes (huffnode *table);
long HuffCompress (unsigned char huge *source, long length,
  unsigned char huge *dest);
void HuffExpand (unsigned char huge *source, unsigned char huge *dest,
  long length,huffnode *hufftable);
void RLEWExpand (unsigned huge *source, unsigned huge *dest,long length,
  unsigned rlewtag);
long RLEWCompress (unsigned huge *source, long length, unsigned huge *dest,
  unsigned rlewtag);

/*
=============================================================================

			   COMPRESSION SUBS

=============================================================================
*/


/*
======================
=
= CountBytes
=
= Adds the bytes in the pointed to area to the counts array
= If this is the first segment, make sure counts is zerod
=
======================
*/

void CountBytes (unsigned char huge *start, long length)
{
  long i;

  while (length--)
    counts[*start++]++;
}

/*
=======================
=
= FindLeast
=
= Returns the byte with the lowest counts value
=
=======================
*/

int FindLeast (void)
{
  int i,least;
  long low = 0x7fffffff;

  for (i=0;i<256;i++)
    if (counts[i]<low)
    {
      low = counts[i];
      least = i;
    }

  return least;
}

/*========================================================================*/

/*
==================
=
= TraceNode
=
= A recursive function that follows all leaves of nodearray and fills in
= coding tables huffbits and huffstring.
=
==================
*/

void TraceNode (int nodenum,int numbits,unsigned long bitstring)
{
  unsigned bit0,bit1;

  bit0 = nodearray[nodenum].bit0;
  bit1 = nodearray[nodenum].bit1;

  numbits++;

  if (bit0 <256)
  {
    huffbits[bit0]=numbits;
    huffstring[bit0]=bitstring;		// just added a zero in front
    if (huffbits[bit0]>24 && counts[bit0])
    {
      puts("Error: Huffman bit string went over 32 bits!");
      exit(1);
    }
  }
  else
    TraceNode (bit0-256,numbits,bitstring);

  if (bit1 <256)
  {
    huffbits[bit1]=numbits;
    huffstring[bit1]=bitstring+ (1ul<<(numbits-1));	// add a one in front
    if (huffbits[bit1]>24 && counts[bit1])
    {
      puts("Error: Huffman bit string went over 32 bits!");
      exit(1);
    }
  }
  else
    TraceNode (bit1-256,numbits,bitstring+(1ul<<(numbits-1)));
}

/*
=======================
=
= Huffmanize
=
= Takes the counts array and builds a huffman tree at
= nodearray, then builds a codeing table.
=
=======================
*/

void Huffmanize (void)
{

//
// codes are either bytes if <256 or nodearray numbers+256 if >=256
//
  unsigned value[256],code0,code1;
//
// probablilities are the number of times the code is hit or $ffffffff if
// it is allready part of a higher node
//
  unsigned long prob[256],low,workprob;

  int i,worknode,bitlength;
  unsigned long bitstring;


//
// all possible leaves start out as bytes
//
  for (i=0;i<256;i++)
  {
    value[i]=i;
    prob[i]=counts[i];
  }

//
// start selecting the lowest probable leaves for the ends of the tree
//

  worknode = 0;
  while (1)	// break out of when all codes have been used
  {
    //
    // find the two lowest probability codes
    //

    code0=0xffff;
    low = 0x7ffffffff;
    for (i=0;i<256;i++)
      if (prob[i]<low)
      {
	code0 = i;
	low = prob[i];
      }

    code1=0xffff;
    low = 0x7fffffff;
    for (i=0;i<256;i++)
      if (prob[i]<low && i != code0)
      {
	code1 = i;
	low = prob[i];
      }

    if (code1 == 0xffff)
    {
      if (value[code0]<256)
	Quit("Wierdo huffman error: last code wasn't a node!");
      if (value[code0]-256 != 254)
	Quit("Wierdo huffman error: headnode wasn't 254!");
      break;
    }

    //
    // make code0 into a pointer to work
    // remove code1 (make 0xffffffff prob)
    //
    nodearray[worknode].bit0 = value[code0];
    nodearray[worknode].bit1 = value[code1];

    value[code0] = 256 + worknode;
    prob[code0] += prob[code1];
    prob[code1] = 0xffffffff;
    worknode++;
  }

//
// done with tree, now build table recursively
//

  TraceNode (254,0,0);

}

/*========================================================================*/


/*
===============
=
= OptimizeNodes
=
= Goes through a huffman table and changes the 256-511 node numbers to the
= actular address of the node.  Must be called before HuffExpand
=
===============
*/

void OptimizeNodes (huffnode *table)
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

/*========================================================================*/

#if 0
/*
======================
=
= HuffCompress
=
= The file must be counted with CountBytes and then Huffmanized first
=
======================
*/

long HuffCompress (unsigned char huge *source, long length,
  unsigned char huge *dest)
{
  long outlength;
  unsigned long string;
  unsigned biton,bits;
  unsigned char byte;

  outlength = biton = 0;

  *(long huge *)dest=0;		// so bits can be or'd on

  while (length--)
  {
    byte = *source++;
    bits = huffbits[byte];
    string = huffstring[byte] << biton;
    *(long huge *)(dest+1)=0;	// so bits can be or'd on
    *(long huge *)dest |= string;
    biton += bits;		// advance this many bits
    dest+= biton/8;
    biton&=7;			// stay under 8 shifts
    outlength+=bits;
  }

  return (outlength+7)/8;
}
#endif


/*========================================================================*/

/*
======================
=
= HuffExpand
=
======================
*/

void HuffExpand (unsigned char huge *source, unsigned char huge *dest,
  long length,huffnode *hufftable)

{
  unsigned bit,byte,node,code;
  unsigned sourceseg,sourceoff,destseg,destoff,endoff;
  huffnode *nodeon,*headptr;

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

asm mov	bx,[headptr]

asm	mov	si,[sourceoff]
asm	mov	di,[destoff]
asm	mov	es,[destseg]
asm	mov	ds,[sourceseg]
asm	mov	ax,[endoff]

asm	mov	ch,[si]				// load first byte
asm	inc	si
asm	mov	cl,1

expandshort:
asm	test	ch,cl			// bit set?
asm	jnz	bit1short
asm	mov	dx,[ss:bx]			// take bit0 path from node
asm	shl	cl,1				// advance to next bit position
asm	jc	newbyteshort
asm	jnc	sourceupshort

bit1short:
asm	mov	dx,[ss:bx+2]		// take bit1 path
asm	shl	cl,1				// advance to next bit position
asm	jnc	sourceupshort

newbyteshort:
asm	mov	ch,[si]				// load next byte
asm	inc	si
asm	mov	cl,1				// back to first bit

sourceupshort:
asm	or	dh,dh				// if dx<256 its a byte, else move node
asm	jz	storebyteshort
asm	mov	bx,dx				// next node = (huffnode *)code
asm	jmp	expandshort

storebyteshort:
asm	mov	[es:di],dl
asm	inc	di					// write a decopmpressed byte out
asm	mov	bx,[headptr]		// back to the head node for next bit

asm	cmp	di,ax				// done?
asm	jne	expandshort
	}
	else
	{

//--------------------------
// expand more than 64k of data
//--------------------------

  length--;

asm mov	bx,[headptr]
asm	mov	cl,1

asm	mov	si,[sourceoff]
asm	mov	di,[destoff]
asm	mov	es,[destseg]
asm	mov	ds,[sourceseg]

asm	lodsb			// load first byte

expand:
asm	test	al,cl		// bit set?
asm	jnz	bit1
asm	mov	dx,[ss:bx]	// take bit0 path from node
asm	jmp	gotcode
bit1:
asm	mov	dx,[ss:bx+2]	// take bit1 path

gotcode:
asm	shl	cl,1		// advance to next bit position
asm	jnc	sourceup
asm	lodsb
asm	cmp	si,0x10		// normalize ds:si
asm  	jb	sinorm
asm	mov	cx,ds
asm	inc	cx
asm	mov	ds,cx
asm	xor	si,si
sinorm:
asm	mov	cl,1		// back to first bit

sourceup:
asm	or	dh,dh		// if dx<256 its a byte, else move node
asm	jz	storebyte
asm	mov	bx,dx		// next node = (huffnode *)code
asm	jmp	expand

storebyte:
asm	mov	[es:di],dl
asm	inc	di		// write a decopmpressed byte out
asm	mov	bx,[headptr]	// back to the head node for next bit

asm	cmp	di,0x10		// normalize es:di
asm  	jb	dinorm
asm	mov	dx,es
asm	inc	dx
asm	mov	es,dx
asm	xor	di,di
dinorm:

asm	sub	[WORD PTR ss:length],1
asm	jnc	expand
asm  	dec	[WORD PTR ss:length+2]
asm	jns	expand		// when length = ffff ffff, done

	}

asm	mov	ax,ss
asm	mov	ds,ax

}


//----------------- replaced by John C. ------------------------------------
#if 0
{
  unsigned bit,byte,node,code;
  unsigned sourceseg,sourceoff,destseg,destoff,endseg,endoff;
  huffnode *nodeon,*headptr;

  nodeon = headptr = hufftable+254;	// head node is allways node 254

  bit = 1;
  byte = *source++;

  while (length)
  {
    if (byte&bit)
      code = nodeon->bit1;
    else
      code = nodeon->bit0;

    bit<<=1;
    if (bit==256)
    {
      bit=1;
      byte = *source++;
    }

    if (code<256)
    {
      *dest++=code;
      nodeon=headptr;
      length--;
    }
    else
      nodeon = (huffnode *)code;
  }


#if 0

  source++;	// normalize
  source--;
  dest++;
  dest--;

  sourceseg = FP_SEG(source);
  sourceoff = FP_OFF(source);
  destseg = FP_SEG(dest);
  destoff = FP_OFF(dest);

  length--;
//
// al = source byte
// cl = bit in source (1,2,4,8,...)
// dx = code
//
// ds:si source
// es:di dest
// ss:bx node pointer
//

asm     mov	bx,headptr
asm	mov	cl,1

asm	mov	si,sourceoff
asm	mov	di,destoff
asm	mov	es,destseg
asm	mov	ds,sourceseg

asm	lodsb			// load first byte

expand:
asm	test	al,cl		// bit set?
asm	jnz	bit1
asm	mov	dx,ss:bx	// take bit0 path from node
asm	jmp	gotcode
bit1:
asm	mov	dx,ss:bx+2	// take bit1 path

gotcode:
asm	shl	cl,1		// advance to next bit position
asm	jnc	sourceup
asm	lodsb
asm	cmp	si,0x10		// normalize ds:si
asm  	jb	sinorm
asm	mov	cx,ds
asm	inc	cx
asm	mov	ds,cx
asm	xor	si,si
sinorm:
asm	mov	cl,1		// back to first bit

sourceup:
asm	or	dh,dh		// if dx<256 its a byte, else move node
asm	jz	storebyte
asm	mov	bx,dx		// next node = (huffnode *)code
asm	jmp	expand

storebyte:
asm	mov	[es:di],dl
asm	inc	di		// write a decopmpressed byte out
asm	mov	bx,headptr	// back to the head node for next bit

asm	cmp	di,0x10		// normalize es:di
asm  	jb	dinorm
asm	mov	dx,es
asm	inc	dx
asm	mov	es,dx
asm	xor	di,di
dinorm:

asm	sub	WORD PTR ss:length,1
asm	jnc	expand
asm  	dec	WORD PTR ss:length+2
asm	jns	expand		// when length = ffff ffff, done

asm	mov	ax,ss
asm	mov	ds,ax

#endif

}
#endif
//---------------------------------------------------------------------------

/*========================================================================*/

/*
======================
=
= RLEWcompress
=
======================
*/

long RLEWCompress (unsigned huge *source, long length, unsigned huge *dest,
  unsigned rlewtag)
{
  long complength;
  unsigned value,count,i;
  unsigned huge *start,huge *end;

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
= RLEWexpand
=
======================
*/

void RLEWExpand (unsigned huge *source, unsigned huge *dest,long length,
  unsigned rlewtag)
{
  unsigned value,count,i;
  unsigned huge *end;
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

asm	mov	bx,rlewtag
asm	mov	si,sourceoff
asm	mov	di,destoff
asm	mov	es,destseg
asm	mov	ds,sourceseg

expand:
asm	lodsw
asm	cmp	ax,bx
asm	je	repeat
asm	stosw
asm	jmp	next

repeat:
asm	lodsw
asm	mov	cx,ax		// repeat count
asm	lodsw			// repeat value
asm	rep stosw

next:

asm	cmp	si,0x10		// normalize ds:si
asm  	jb	sinorm
asm	mov	ax,si
asm	shr	ax,1
asm	shr	ax,1
asm	shr	ax,1
asm	shr	ax,1
asm	mov	dx,ds
asm	add	dx,ax
asm	mov	ds,dx
asm	and	si,0xf
sinorm:
asm	cmp	di,0x10		// normalize es:di
asm  	jb	dinorm
asm	mov	ax,di
asm	shr	ax,1
asm	shr	ax,1
asm	shr	ax,1
asm	shr	ax,1
asm	mov	dx,es
asm	add	dx,ax
asm	mov	es,dx
asm	and	di,0xf
dinorm:

asm	cmp     di,ss:endoff
asm	jne	expand
asm	mov	ax,es
asm	cmp	ax,ss:endseg
asm	jb	expand

asm	mov	ax,ss
asm	mov	ds,ax

}


/*
======================
=
= RLEBcompress
=
======================
*/

long RLEBCompress (unsigned char huge *source, long length,
  unsigned char huge *dest, unsigned char rlebtag)
{
  long complength;
  unsigned char value,count;
  unsigned i;
  unsigned char huge *start,huge *end;

  start = dest;

  end = source+length;

//
// compress it
//
  do
  {
    count = 1;
    value = *source++;
    while (*source == value && source<end && count<255)
    {
      count++;
      source++;
    }
    if (count>3 || value == rlebtag)
    {
    //
    // send a tag / count / value string
    //
      *dest++ = rlebtag;
      *dest++ = count;
      *dest++ = value;
    }
    else
    {
    //
    // send byte without compressing
    //
      for (i=1;i<=count;i++)
	*dest++ = value;
    }

  } while (source<end);

  complength = dest-start;
  return complength;
}



/*
======================
=
= RLEBExpand
=
======================
*/

void RLEBExpand (unsigned char huge *source, unsigned char huge *dest,
  long length, unsigned char rlebtag)
{
  unsigned char value,count;
  unsigned i;
  unsigned sourceseg,sourceoff,destseg,destoff,endseg,endoff;
  unsigned char huge *end;


//
// expand it
//
#if 0
  do
  {
    value = *source++;
    if (value != rlebtag)
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


  end = dest + (length);
  sourceseg = FP_SEG(source);
  sourceoff = FP_OFF(source);
  destseg = FP_SEG(dest);
  destoff = FP_OFF(dest);
  endseg = FP_SEG(end);
  endoff = FP_OFF(end);


//
// al = source value
// bl = tag value
// cl = repeat counts
// dx = scratch
//
// NOTE: A repeat count that produces 0xfff0 bytes can blow this!
//

asm	mov	bx,WORD PTR rlebtag
asm	mov	si,sourceoff
asm	mov	di,destoff
asm	mov	es,destseg
asm	mov	ds,sourceseg
asm	xor	ch,ch

expand:
asm	lodsb
asm	cmp	al,bl
asm	je	repeat
asm	stosb
asm	jmp	next

repeat:
asm	lodsb
asm	mov	cl,al		// repeat count
asm	lodsb			// repeat value
asm	rep stosb

next:

asm	cmp	si,0x10		// normalize ds:si
asm  	jb	sinorm
asm	mov	ax,si
asm	shr	ax,1
asm	shr	ax,1
asm	shr	ax,1
asm	shr	ax,1
asm	mov	dx,ds
asm	add	dx,ax
asm	mov	ds,dx
asm	and	si,0xf
sinorm:
asm	cmp	di,0x10		// normalize es:di
asm  	jb	dinorm
asm	mov	ax,di
asm	shr	ax,1
asm	shr	ax,1
asm	shr	ax,1
asm	shr	ax,1
asm	mov	dx,es
asm	add	dx,ax
asm	mov	es,dx
asm	and	di,0xf
dinorm:

asm	cmp     di,ss:endoff
asm	jne	expand
asm	mov	ax,es
asm	cmp	ax,ss:endseg
asm	jb	expand

asm	mov	ax,ss
asm	mov	ds,ax


}



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


#if 1
/*
==================
=
= CarmackCompress
=
= Compress a string of words
=
==================
*/

#define NEARTAG	0xa7
#define FARTAG	0xa8

long CarmackCompress (unsigned far *source,long length,
	unsigned far *dest)
{
	unsigned	ch,chhigh;
	unsigned	far *instart, far *inptr, far *inscan,
				far *stopscan, far *outptr;
	unsigned	far *bestscan, beststring;
	unsigned	dist,maxstring,string,outlength;
	unsigned	nearrepeats,farrepeats;
	unsigned	dups,count;
	unsigned	newwords;

//
// this compression method produces a stream of words
// If the words high byte is NEARTAG or FARTAG, the low byte is a word
// copy count from the a position specified by either the next byte
// or the next word, respectively.  A copy count of 0 means to insert the
// next byte as the low byte of the tag into the output
//


//
// set up
//
	instart = inptr = source;
	outptr = dest;

	outlength = 0;
	length = (length+1)/2;

	nearrepeats = farrepeats = dups = 0;
	count = 10;
	newwords = 0;
//
// compress
//
	do
	{
		ch = *inptr;

//
// scan from start for patterns that match current data
//
		beststring = 0;
		for (inscan = instart; inscan<inptr; inscan++)
		{
			if (*inscan != ch)
				continue;

			maxstring = inptr-inscan;
			if (maxstring > length)
				maxstring = length;
			if (maxstring > 255)
				maxstring = 255;

			string = 1;
			while (*(inscan+string) == *(inptr+string) && string<maxstring)
				string++;

			if (string >= beststring)
			{
				beststring = string;
				bestscan = inscan;
			}
		}

		if (beststring > 1 && inptr-bestscan <= 255)
		{
		// near string
			*outptr++ = beststring + (NEARTAG*256);
			*((unsigned char far *)outptr)++ = inptr-bestscan;
			outlength += 3;
			nearrepeats++;
			inptr += beststring;
			length -= beststring;
		}
		else if (beststring > 2)
		{
		// far string
			*outptr++ = beststring + (FARTAG*256);
			*outptr++ = bestscan-instart;
			outlength += 4;
			farrepeats++;
			inptr += beststring;
			length -= beststring;
		}
		else							// no compression
		{
			chhigh = ch>>8;
			if (chhigh == NEARTAG || chhigh == FARTAG)
			{
			// special case of encountering a
			// tag word in the data stream
			// send a length of 0, and follow it with the real low byte
				*outptr++ = ch & 0xff00;
				*((unsigned char far *)outptr)++ = ch&0xff;
				outlength += 3;
				dups++;
			}
			else
			{
				*outptr++ = ch;
				outlength += 2;
			}
			inptr++;
			length--;
			newwords++;
		}

		if (!--count)
		{
			static char cc[2]="-";
			cc[0]^='+'^'-';
			print(cc);
			sx--;

			count = 10;

		 if (keydown[1])
		 {
		  while(keydown[1]);
		  return 0;
		 }
		}
		if (length<0)
			Quit ("Length < 0!");
	} while (length);
	#if 0
	printf ("%u near patterns\n",nearrepeats);
	printf ("%u far patterns\n",farrepeats);
	printf ("%u new words\n",newwords);
	printf ("%u dups\n\n",dups);
	#endif
	return outlength;
}
#else



/*
==================
=
= CarmackCompress
=
= Compress a string of words
=
==================
*/

#define NEARTAG	0xa7
#define FARTAG	0xa8

long CarmackCompress (unsigned far *source,long length,
	unsigned far *dest)
{
	unsigned	wch,chhigh;
	unsigned	inptrx;
	unsigned	far *instart, far *inptr, far *inscan,
				far *stopscan, far *outptr;
	unsigned	far *bestscan, beststring;
	unsigned	dist,maxstring,string,outlength;
	unsigned	nearrepeats,farrepeats;
	unsigned	dups,count;
	unsigned	newwords;

//
// this compression method produces a stream of words
// If the words high byte is NEARTAG or FARTAG, the low byte is a word
// copy count from the a position specified by either the next byte
// or the next word, respectively.  A copy count of 0 means to insert the
// next byte as the low byte of the tag into the output
//


//
// set up
//
	instart = inptr = bestscan = source;
	outptr = dest;

	outlength = 0;
	length = (length+1)/2;

	nearrepeats = farrepeats = dups = 0;
	count = 10;
	newwords = 0;
//
// compress
//
	do
	{
		wch = *inptr;
		inptrx = FP_OFF(inptr)+2;	// convienient for cmpsw

//
// scan from start for patterns that match current data
//
//================

//
// ax:
// bx: beststring
// cx: repeat counts
// dx: holding spot for inscan repeat count
// si: inptr
// di: inscan
//
asm	xor	bx,bx					// beststring = 0

asm	les	di,[instart]			// start looking for an identical string
								// at the start of uncompressed text
asm	mov	ax,es
asm	mov	ds,ax					// both DS and ES point to uncompressed data

asm	mov	cx,WORD PTR [inptr]
asm	sub	cx,di
asm	shr	cx,1					// cx = words between inscan and inptr

search:
asm	mov	ax,[wch]				// current uncompressed word
asm	repne	scasw				// search from DI for up to CX words for a
								// first char match
asm	jcxz	done				// no more to search

asm	mov	dx,cx					// save off remaining repeat count
								// the number in CX is the remaining words
								// to search for a long pattern
asm	mov	si,[inptrx]				// SI is the current source data
								// DI is the match in previously compressed
								// data
asm	mov	ax,di					// save off DI so we can back up after scan
asm	repe	cmpsw				// decrement CX for each additional match
asm	jne	ended
asm	dec	cx						// the search ended because of CX, not bad match
ended:
asm	mov	di,ax					// back to original spot
asm	mov	ax,dx
asm	sub	ax,cx					// AX is the total number of matching words
asm	cmp	ax,bx					// more chars than beststring?
asm	jb	next
asm	mov	bx,ax
asm	mov	WORD PTR [bestscan],di	// bestscan is start of the best match+2

next:
asm	mov	cx,dx					// restore the remaining count
asm	jmp	search

done:
asm	mov	ax,ss
asm	mov	ds,ax
asm	mov	[beststring],bx			// save out the register variable
asm	dec	bx
asm	sub	WORD PTR [bestscan],2	// scasw incremented past start
//================

		if (beststring>length)
			beststring = length;

		if (beststring > 1 && inptr-bestscan <= 255)
		{
		// near string
			*outptr++ = beststring + (NEARTAG*256);
			*((unsigned char far *)outptr)++ = inptr-bestscan;
			outlength += 3;
			nearrepeats++;
			inptr += beststring;
			length -= beststring;
		}
		else if (beststring > 2)
		{
		// far string
			*outptr++ = beststring + (FARTAG*256);
			*outptr++ = bestscan-instart;
			outlength += 4;
			farrepeats++;
			inptr += beststring;
			length -= beststring;
		}
		else							// no compression
		{
			chhigh = wch>>8;
			if (chhigh == NEARTAG || chhigh == FARTAG)
			{
			// special case of encountering a
			// tag word in the data stream
			// send a length of 0, and follow it with the real low byte
				*outptr++ = wch & 0xff00;
				*((unsigned char far *)outptr)++ = wch&0xff;
				outlength += 3;
				dups++;
			}
			else
			{
				*outptr++ = wch;
				outlength += 2;
			}
			inptr++;
			length--;
			newwords++;
		}

		if (!--count)
		{
		 static char cc[2]="-";
		 cc[0]^='+'^'-';
		 print(cc);
		 sx--;

		 count = 10;

		 if (keydown[1])
		 {
		  while(keydown[1]);
		  return 0;
		 }
		}
	} while (length);
#if 0
	printf ("\r%u near patterns\n",nearrepeats);
	printf ("%u far patterns\n",farrepeats);
	printf ("%u new words\n",newwords);
	printf ("%u dups\n\n",dups);
#endif
	return outlength;
}
#endif


