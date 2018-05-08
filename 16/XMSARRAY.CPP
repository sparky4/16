//--------------------------------------------------------------------------
//
//      XMSARRAY.CPP: body of XMSarray interface library.
//      Copyright (c) J.English 1993.
//      Author's address: je@unix.brighton.ac.uk
//
//      Permission is granted to use copy and distribute the
//      information contained in this file provided that this
//      copyright notice is retained intact and that any software
//      or other document incorporating this file or parts thereof
//      makes the source code for the library of which this file
//      is a part freely available.
//
//--------------------------------------------------------------------------
//
//      Revision history:
//      1.0     Jun 1993        Initial coding
//      1.01    Sep 1993        Filenames changed from XMS.* to XMSARRAY.*
//      2.0     Nov 1993        Revised to use general purpose XMS class
//
//--------------------------------------------------------------------------

#include "xms.h"
#include "xmsarray.h"
#include <dos.h>
#include <stdlib.h>

//--------------------------------------------------------------------------
//
//      Constants.
//
const int USED      = 1,    // cache line is in use
          DIRTY     = 2;    // cache line has been written to


//--------------------------------------------------------------------------
//
//      Data structures.
//
struct XMSbuffer            // cache structure
{
    int      mask;          // ... mask to extract item number in buffer
    long     where;         // ... offset to start of XMS block
    unsigned width;         // ... unit for XMS transfers in bytes
    char*    cache;         // ... buffer for XMS transfers
    long     tag;           // ... current index held in buffer
    char     flags;         // ... buffer state (in use, dirty)
};

struct XMSheader            // header for XMS block
{
    long size;              // ... block size in bytes
    int  free;              // ... is it a free block?
};

struct XMSmove              // descriptor for XMS moves
{
    long count;             // ... number of bytes to move
    int  srchandle;         // ... source handle (0 for real memory)
    long srcoffset;         // ... source offset (or far pointer)
    int  dsthandle;         // ... destination handle (0 for real memory)
    long dstoffset;         // ... destination offset (or far pointer)
};


//--------------------------------------------------------------------------
//
//      Globals (statics).
//
static XMS* arrayBlock = 0; // block used to hold XMS arrays
static int  arrayCount = 0; // number of allocated XMS arrays
static long XMSsize    = 0; // current allocation size, internal reckoning
                            // (XMS driver rounds up to nearest 4K or 16K)

//--------------------------------------------------------------------------
//
//      Function to return the number of bits required to represent "n",
//      used for rounding up to powers of 2 and generating masks.
//
static int bits (unsigned n)
{
    int b = 0;
    for (long i = 1; i > 0 && i < n; i <<= 1)
        b++;
    return b;
}

//--------------------------------------------------------------------------
//
//      XMSlocate: locate a free block of XMS.
//
//      This is used by XMSalloc to check if there is a block of suitable
//      size which has been returned to the free list.  If not, it returns
//      0 (which is never a legitimate offset) and if so, it divides the
//      block as necessary, marks the allocated block as in use and returns
//      its offset.  XMS blocks are prefaced by an XMSheader giving their
//      size and status (free/in use).  Offsets refer to the first byte
//      of the allocation (i.e. the byte after the header); the size then
//      gives the address of the next block.  Adjacent free blocks are merged
//      while scanning the list.  If the last block is free, it can't be
//      merged with anything and so XMSsize is adjusted to remove it from
//      the internal record of the allocation size.
//
static long XMSlocate (long size)
{
    long p = 0, oldp = 0;
    XMSheader h, oldh = {0,0};

    //--- scan block list from offset 0
    while (p < XMSsize)
    {
        //--- get header of next block
        XMS::copy (&h, (*arrayBlock)[p], sizeof(h));

        //--- merge adjacent free blocks by changing size of previous block
        if (oldh.free && h.free)
        {   oldh.size += h.size + sizeof(h);
            h = oldh, p = oldp;
            XMS::copy ((*arrayBlock)[p], &h, sizeof(h));
        }

        //--- check if block (after possible merging) is big enough
        if (h.free && h.size >= size + sizeof(XMSheader))
            break;

        //--- keep value and offset for block and get offset of next header
        oldh = h, oldp = p;
        p += h.size + sizeof(h);
    }

    //--- no suitable block available: remove last block if it is free
    if (p >= XMSsize)
    {   if (oldh.free)
            XMSsize = oldp;
        return 0;
    }

    //--- divide block if necessary
    if (h.free && h.size >= size + sizeof(XMSheader))
    {
        XMSheader t = {h.size - size - sizeof(t), 1};
        XMS::copy (arrayBlock->at(p+sizeof(h)+size), &t, sizeof(t));
        h.size = size;
    }

    //--- write new block's header and return offset of first usable byte
    h.free = 0;
    XMS::copy (arrayBlock->at(p), &h, sizeof(h));
    return p + sizeof(h);
}

//--------------------------------------------------------------------------
//
//      XMSalloc: allocate a block of XMS.
//
//      This function tries to allocate "size" bytes of XMS and returns 0
//      if it fails.
//
static long XMSalloc (long size)
{
    //--- check XMS initialised before proceeding
    if (arrayCount++ == 0)
    {   arrayBlock = new XMS (sizeof(XMSheader) + size);
        if (arrayBlock == 0 || !arrayBlock->valid())
        {   delete arrayBlock;
            arrayCount--;
            arrayBlock = 0;
        }
        else
        {   XMSheader h = {0,0};
            XMS::copy (arrayBlock->at(0), &h, sizeof(XMSheader));
        }
    }
    if (arrayBlock == 0)
        return 0;

    //--- try to find a suitable free block in the current allocation
    long pos = XMSlocate (size);
    if (pos != 0)
        return pos;

    //--- try to extend current allocation if none found
    if (arrayBlock->resize (XMSsize + size + sizeof(XMSheader)) != XMS::SUCCESS)
        return 0;

    //--- write the header for the new block at the end of current allocation
    XMSheader h = {size,0};
    XMS::copy (arrayBlock->at(XMSsize), &h, sizeof(h));

    //--- get offset of first usable byte of new block
    pos = XMSsize + sizeof(h);

    //--- increase allocation size to accomodate it and return its offset
    XMSsize += size + sizeof(h);
    return pos;
}


//--------------------------------------------------------------------------
//
//      XMSfree: mark a block of XMS as free.
//
//      This returns a block to the free list by rewriting its header.
//
static void XMSfree (long offset)
{
    if (--arrayCount == 0)
    {   delete arrayBlock;
        return;
    }
    XMSheader h;
    XMS::copy (&h, arrayBlock->at(offset-sizeof(h)), sizeof(h));
    h.free = 1;
    XMS::copy (arrayBlock->at(offset-sizeof(h)), &h, sizeof(h));
}

//--------------------------------------------------------------------------
//
//      XMSarrayBase::XMSarrayBase.
//
//      Constructor for base class XMS, called by constructor for XMSarray.
//      Constructs an XMS array containing the specified number of items of
//      the specified size and also a one-line cache of the specified size.
//
XMSarrayBase::XMSarrayBase (long items, unsigned size, unsigned cachesize)
{
    //--- initial state is "unallocated"
    state = 0;

    //--- create the cache
    buffer = new XMSbuffer;
    if (buffer == 0)
        return;

    //--- set up the item offset mask (items in line - 1)
    buffer->mask = (size > cachesize) ? 0 : 
                   (1 << (bits(cachesize) - bits(size))) - 1;

    //--- calculate number of items needed (1 extra rounded up to nearest line)
    if (items < 0)
        items = 0;
    items += buffer->mask;
    items &= ~buffer->mask;

    //--- calculate cache size in bytes and allocate it
    buffer->width = (buffer->mask + 1) * size;
    buffer->cache = new char [buffer->width];
    if (buffer->cache == 0)
        return;

    //--- try to allocate the XMS block
    buffer->where = XMSalloc (items * size);

    //--- mark cache line as unused
    buffer->tag   = 0;
    buffer->flags = 0;

    //--- record if allocation succeeded
    state = (buffer->where != 0);
}


//--------------------------------------------------------------------------
//
//      XMSarrayBase::XMSarrayBase.
//
//      Copy constructor used by XMSitem to clone a reference to the
//      base array.
//
XMSarrayBase::XMSarrayBase (const XMSarrayBase& base)
{
    state  = base.state;
    buffer = base.buffer;
}

//--------------------------------------------------------------------------
//
//      XMSarrayBase::free.
//
//      Try to free an array.  Check that it's there before doing anything,
//      then free the XMS block and delete the cache.  This is not a written
//      as a destructor so that XMSitem doesn't call it.  Unfortunately
//      Borland C++ won't let you nominate all possible instantiations of
//      a template as friends of a non-template class, i.e.
//          class X { template<class T> friend class Y<T>; ... };
//      gives a compilation error.
//
void XMSarrayBase::free ()
{
    if (buffer == 0)
        return;
    if (buffer->where != 0)
        XMSfree (buffer->where);
    delete buffer->cache;
    delete buffer;
}


//--------------------------------------------------------------------------
//
//      XMSarrayBase::mask.
//
//      Return the item number within the cache line for a given subscript.
//
int XMSarrayBase::mask (int n)
{
    if (!state)
        return 0;
    return n & buffer->mask;
}

//--------------------------------------------------------------------------
//
//      XMSarrayBase::get.
//
//      Get a block from XMS into conventional memory.  If it's not in the
//      cache, load the relevant line (writing back the old line if it was
//      dirty).  Return a pointer to the cached copy of the item.  "Base"
//      is the base address of the line to load and "offset" is the offset
//      into the cache buffer.
//
void* XMSarrayBase::get (long base, int offset)
{
    if (!state)
        return 0;
    if (buffer->tag != base || !(buffer->flags & USED))
    {   if (buffer->flags & DIRTY)
            XMS::copy (arrayBlock->at (buffer->where + buffer->tag),
                       buffer->cache, buffer->width);
        XMS::copy (buffer->cache, arrayBlock->at (buffer->where + base),
                   buffer->width);
        buffer->flags = USED;
        buffer->tag = base;
    }
    return buffer->cache + offset;
}


//--------------------------------------------------------------------------
//
//      XMSarrayBase::put.
//
//      Gets a copy of the relevant cache line into memory and then marks
//      the line as dirty (about to be written to).
//
void* XMSarrayBase::put (long base, int offset)
{
    if (!state)
        return 0;
    void* addr = get (base, offset);
    buffer->flags |= DIRTY;
    return addr;
}
