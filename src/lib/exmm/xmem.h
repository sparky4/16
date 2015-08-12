#if !defined(_XMEM_H)
#define _XMEM_H
 
typedef struct xms_node
   {
   long start, size, off;
   short used;
   struct xms_node *next;
   }
xms_node_t;
 
typedef struct
   {
   int handle;
   unsigned long total;
   unsigned long avail;
   unsigned long next_off;
   xms_node_t *next;
   }
xms_head_t;
 
#define XMSBLOCK 16384u
#define XMSBLOCKSHIFT 14
 
extern void LSHL( unsigned long far *SHLnumber, unsigned short n );
 
extern unsigned short XMS_available( void );
 
extern unsigned short XMSblk_available( void );
 
extern short XMS_alloc(unsigned short rsrvd,
                       unsigned short far *size
                      );
 
extern short XMS_dealloc(unsigned short Hdl );
 
extern short XMStoMem(unsigned short Handle,   // XMS handle returned by
//XMS_alloc()
                      unsigned short blk,      // which 16k block to copy to
                      unsigned short blkAdr,   // offset within 16k block
                      unsigned short Bytes,    // bytes to copy
                      void   far *memAdr
                     );
 
extern short MemToXMS(unsigned short Handle,
                      unsigned short blk,
                      unsigned short blkAdr,
                      unsigned short Bytes,
                      void   far *memAdr
                     );
 
// call these for ease
short alloc_xms(unsigned short far *size);  // size in 16k blocks
// NOTE size is changed to the amount block size was altered by!
// normaly this is zero
 
short xms_to_mem(unsigned short handle, void far *p, unsigned long off,
unsigned short n);
short mem_to_xms(unsigned short handle, void far *p, unsigned long off,
unsigned short n);
void deinit_xms(void);
short init_xms(unsigned short min_blocks);
void qfree_xms(xms_node_t *node);
xms_node_t *qalloc_xms(unsigned long size);
xms_node_t *xms_open(char *file);
short xms_read(void far *buffer, unsigned short n, xms_node_t *node);
short xms_write(void far *buffer, unsigned short n, xms_node_t *node);
long xms_tell(xms_node_t *node);
short xms_seek(xms_node_t *node, long off, short whence);
void xms_close(xms_node_t *node);
 
 
extern xms_head_t xms_head;
 
 
 
#endif
 
/* ---------------------------------- end of file --------------------- */
