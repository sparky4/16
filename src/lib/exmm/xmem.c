/*
   Written by Alexander J. Russell 1994
 
   Placed in the public Domain by Alec Russell, March 1995
 
   Slightly higher level xms calls than xmem.asm
 
*/
 
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <malloc.h>
 
#include "src\lib\exmm\xmem.h"
 
xms_head_t xms_head={0};  // set handle to zero
 
 
/* ---------------------- alloc_xms() ----------------- February 19,1994 */
short alloc_xms(unsigned short far *size)  // size in 16k blocks
{
   return(XMS_alloc(0, size));
}
 
 
 
/* ---------------------- xms_to_mem() ---------------- February 19,1994 */
short xms_to_mem(unsigned short handle, void far *p, unsigned long off,
unsigned short n)
{
   unsigned short block, boff;
 
   block=off >> XMSBLOCKSHIFT;
   boff=off - (block << XMSBLOCKSHIFT);
 
   return(XMStoMem(handle, block, boff, n, p));
}
 
 
/* ---------------------- mem_to_xms() ---------------- February 19,1994 */
short mem_to_xms(unsigned short handle, void far *p, unsigned long off,
unsigned short n)
{
   unsigned short block, boff;
 
   block=off >> XMSBLOCKSHIFT;
   boff=off - (block << XMSBLOCKSHIFT);
 
   return(MemToXMS(handle, block, boff, n, p));
}
 
 
/* ---------------------- qalloc_xms() -------------------- March 8,1994 */
xms_node_t *qalloc_xms(unsigned long size)
{
   xms_node_t *node=NULL;
   xms_node_t *t1;
 
   if ( size <= xms_head.avail )
      {
      // look for existing node
      t1=xms_head.next;
      while ( t1 )
         {
         if ( t1->used == 0 && t1->size >= size )
            {
            t1->off=0;
            t1->used=1;
            node=t1;
            break;
            }
         else
            t1=t1->next;
         }
 
      if ( node == NULL ) // didn't find existing node
         {
         node=malloc(sizeof(xms_node_t));
         if ( node )
            {
            node->off=0;
            node->used=1;
            node->size=size;
            node->next=NULL;
            node->start=xms_head.next_off;
            xms_head.avail-=size;
            xms_head.next_off+=size;
            if ( xms_head.next == NULL )
               {
               xms_head.next=node;
               }
            else
               {
               t1=xms_head.next;
               while ( t1->next )
                  t1=t1->next;
               t1->next=node;
               }
            }
         else
            printf("out of near mem in qalloc_xms");
         }
      }
   else
      printf("out of xms mem in qalloc size %lu avail %lu", size,
xms_head.avail);
 
   return(node);
}
 
 
/* ---------------------- qfree_xms() --------------------- March 8,1994 */
void qfree_xms(xms_node_t *node)
{
   xms_node_t *t1;
 
   if ( xms_head.next )
      {
      t1=xms_head.next;
      while ( t1 != node && t1 )
         t1=t1->next;
 
      if ( t1 )
         {
         t1->used=0;
         }
      else
         printf("ERROR didn't find node qfree");
      }
   else
      {
      printf("ATTEMPTED to qfree empty list");
      }
}
 
 
 
/* ---------------------- xms_open() ---------------------- March 8,1994 */
xms_node_t *xms_open(char *file)
{
   int i;
   xms_node_t *node=NULL;
   FILE *fp;
   char *buffer;
   unsigned long off;
 
   fp=fopen(file, "rb");
   if ( fp )
      {
      node=qalloc_xms(filelength(fileno(fp)));
      if ( node )
         {
         buffer=malloc(4096);
         if ( buffer )
            {
            off=0l;
            while ( (i=fread(buffer, 1, 4096, fp)) )
               {
               mem_to_xms(xms_head.handle, (char far *)buffer,
off+node->start, i);
               off+=i;
               }
 
            free(buffer);
            }
         else
            printf("out of mem in xms_open 1");
         }
 
      fclose(fp);
      }
   else
      printf("ERROR opening %s in xms_open", file);
 
   return(node);
}
 
 
/* ---------------------- xms_read() ---------------------- March 8,1994 */
short xms_read(void far *buffer, unsigned short n, xms_node_t *node)
{
 
   if ( node->off >= node->size )
      return 0;
 
   if ( n+node->off > node->size )
      n=node->size - node->off;
 
   xms_to_mem(xms_head.handle, buffer, node->start+node->off, n);
   node->off+=n;
 
   return(n);
}
 
 
/* ---------------------- xms_write() ---------------------- March 8,1994 */
short xms_write(void far *buffer, unsigned short n, xms_node_t *node)
{
 
   if ( node->off >= node->size )
      return 0;
 
   if ( n+node->off > node->size )
      n=node->size - node->off;
 
   mem_to_xms(xms_head.handle, buffer, node->start+node->off, n);
   node->off+=n;
 
   return(n);
}
 
 
/* ---------------------- xms_tell() ---------------------- March 8,1994 */
long xms_tell(xms_node_t *node)
{
   return node->off;
}
 
 
 
/* ---------------------- xms_seek() ---------------------- March 8,1994 */
short xms_seek(xms_node_t *node, long off, short whence)
{
   short err=0;
 
   switch ( whence )
      {
      case SEEK_SET:
         if ( off < 0l || off > node->size )
            err=1;
         else
            node->off=off;
         break;
 
      case SEEK_END:
         if ( off > 0l || (node->size + off) < 0l )
            err=1;
         else
            node->off=node->size + off;
         break;
 
      case SEEK_CUR:
         if ( node->off + off < 0l || node->off + off > node->size )
            err=1;
         else
            node->off+=off;
         break;
      }
 
   return(err);
}
 
 
/* ---------------------- xms_close() --------------------- March 8,1994 */
void xms_close(xms_node_t *node)
{
   qfree_xms(node);
}
 
 
 
 
/* ---------------------- init_xms() ---------------------- March 8,1994 */
short init_xms(unsigned short min_blocks)
{
   unsigned short blocks;
 
   blocks=XMSblk_available();
   if ( blocks >= min_blocks )
      {
      memset(&xms_head, 0, sizeof(xms_head_t));
      if ( (xms_head.handle=alloc_xms(&blocks)) )
         {
         printf("blocks minus by = %u", blocks);
         min_blocks-=blocks;
         xms_head.avail=xms_head.total=(unsigned long)min_blocks*XMSBLOCK;
         blocks=min_blocks;
         }
      else
         blocks=0;
      }
   else
      blocks=0;
 
   return(blocks);
}
 
 
/* ---------------------- deinit_xms() -------------------- March 8,1994 */
void deinit_xms(void)
{
   xms_node_t *t1, *t2;
 
   if ( xms_head.handle )
      {
      XMS_dealloc(xms_head.handle);
      if ( xms_head.next )
         {
         t1=xms_head.next;
         t2=t1->next;
         while ( t1 )
            {
            free(t1);
            t1=t2;
            t2=t1->next;
            }
         }
 
      memset(&xms_head, 0, sizeof(xms_head_t));
      }
}
 
 
 
/* --------------------------- end of file ------------------------- */

/*

Not sure how to use this?

call init_xms(x) to allocate a big chunk of xms.
x is in 'blocks' of 16Kb. Pick X big enough to buffer all the files
you want to place in xms.

call xms_open("filename); for each file to be buffered. This copies the file
int xms.

then use xms_read(), xms_write(), and xms_seek() to read the file from
xms instead of disk.

call deinit_xms() just before exit to clean up.

You can also use the lower level calls directly.

*/
