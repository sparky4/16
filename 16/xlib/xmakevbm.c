/*----------------------------------------------------------------------
; MODULE XMAKEVBM
;
; Implements function to generate a Video bitmap (VBM) from a linear bitmap
; (LBM)
;
; Compile with Tasm.
; C callable.
;
;
; Based on "CreateMaskedAlignedImage" published in Dr Dobbs Journal
; by Michael Abrash (Jul - Aug 1991)
;
; ****** XLIB - Mode X graphics library                ****************
; ******                                               ****************
; ****** Written By Themie Gouthas                     ****************
; ****** Aeronautical Research Laboratory              ****************
; ****** Defence Science and Technology Organisation   ****************
; ****** Australia                                     ****************
;
; egg@dstos3.dsto.gov.au
; teg@bart.dsto.gov.au
;-----------------------------------------------------------------------*/
/*
  Here is an outline of the XLIB image formats


  PLANAR BITMAPS
  --------------

  Planar bitmaps as used by these functions have the following structure:

  BYTE 0                 The bitmap width in bytes (4 pixel groups) range 1..255
  BYTE 1                 The bitmap height in rows range 1..255
  BYTE 2..n1             The plane 0 pixels width*height bytes
  BYTE n1..n2            The plane 1 pixels width*height bytes
  BYTE n2..n3            The plane 2 pixels width*height bytes
  BYTE n3..n4            The plane 3 pixels width*height bytes

  LINEAR BITMAPS
  --------------

  Linear bitmaps have the following structure:

  BYTE 0                 The bitmap width in pixels  range 1..255
  BYTE 1                 The bitmap height in rows   range 1..255
  BYTE 2..n              The width*height bytes of the bitmap


  VIDEO BITMAPS
  -------------

      WORD  0   Size          Total size of this VBM structure in bytes
      WORD  1   ImageWidth    Width in bytes of the image (for all alignments)
      WORD  2   ImageHeight   Height in scan lines of the image

      WORD  3 Alignment 0  ImagePtr   Offset in VidRAM of this aligned image
   +--WORD  4              MaskPtr    Offset (within this structure's DS) of
   |   .			       alignment masks
   |   .
   |   .
   |  WORD  9 Alignment 3  ImagePtr   Offset in VidRAM of this aligned image
  +|--WORD 10              MaskPtr    Offset (within this structure's DS) of
  ||   	        		       alignment masks
  ||
  |+->BYTE 21 (WORD 11)                -------+-- Image masks for alignment 0
  |   .                                       |
  |   .                                       |
  |   BYTE  21 + ImageWidth*ImageHeight  -----+
  |
  |   .
  |   . (similaly for alignments 1 - 2 )
  |   .
  |
  +-->BYTE  21 + 3*ImageWidth*ImageHeight + 1-+-- Image masks for alignment 3
      .                                       |
      .                                       |
      BYTE  21 + 4*(ImageWidth*ImageHeight) --+

      .
      .
      << Similarly for alignments 2 and 3 >>
      .
      .
      BYTE 21 + 4*(ImageWidth*ImageHeight)
  -------------

  (And dont forget the corresponding data in video ram)


*/



#include <alloc.h>
#include <dos.h>

/* function to store the linear bitmap in the required video RAM offset */
/* and in the required alignment                                        */

extern unsigned int x_store_vbm_image(unsigned int, int, char far *);


/* Alignment structures, 4 of which make up the header section of the */
/* video bitmap                                                       */

typedef struct {
  unsigned int  size;
  unsigned int ImageWidth;
  unsigned int ImageHeight;
  struct {
     unsigned int  ImagePtr;
     unsigned int  MaskPtr;
  } alignments[4];
} alignment_header;

/* Structure to extract width/height frol LBM (linear bit map) */

typedef struct {
  unsigned char width;
  unsigned char height;
} lbm_header;


/*************************************************************************/
/*                                                                       */
/* Generates all four possible mode X image/mask alignments, stores      */
/* image alignments in display memory, allocates memory for and generates*/
/* mask alignments, and fills out a VBM aligned masked image structure.  */
/* Each non-zero byte in source bitmap corresponds to image pixel to be  */
/* drawn.                                                                */
/* On success returns a far pointer to the new VBM structure otherwise   */
/* it returns NULL                                                       */
/*                                                                       */
/* Source Language: C                                                    */
/*                                                                       */
/* Parameters:                                                           */
/*    lbm        pointer to linear bitmap                                */
/*    vramStart  contains the next available video offset which is       */
/*               also updated after calling this function                */
/*                                                                       */
/*************************************************************************/

char far *x_make_vbm(char far *lbm, unsigned int *VramStart)
{

  lbm_header far       *lbm_headr;
  alignment_header far *vbm_headr;
  char far             *vbm_mask_ptr,*p;
  char far             *lbm_pixel_ptr;
  int                  align,BitNum,TempImageWidth;
  unsigned int         TempWidth,TempHeight,TempSize,MaskSize,VramOffs,MaskSpace=0;
  int                  scanline;
  unsigned char        MaskTemp;

  VramOffs  = *VramStart;
  lbm_headr = (lbm_header far *) lbm;

  TempWidth     = (lbm_headr->width+3)/4+1;
  TempHeight    = lbm_headr->height;
  TempSize      = TempWidth*TempHeight;

  vbm_headr = (alignment_header far *) farmalloc(22+TempSize*4);
  if (!vbm_headr) return NULL;

  MaskSpace=22;

  vbm_headr->ImageWidth  = TempWidth;
  vbm_headr->ImageHeight = TempHeight;
  vbm_headr->size        = 22+TempSize*4;
  for (align=0;align<4;align++){
    vbm_headr->alignments[align].ImagePtr = VramOffs;
    x_store_vbm_image(VramOffs,align,lbm);
    MaskSpace+=TempSize;
    VramOffs+=TempSize;
  }


  vbm_mask_ptr = (char far *)vbm_headr+22;

  for (align=0;align<4;align++){
    lbm_pixel_ptr = lbm + 2;
    vbm_headr->alignments[align].MaskPtr = FP_OFF(vbm_mask_ptr);
    for (scanline=0;scanline<TempHeight;scanline++){
      BitNum=align;
      MaskTemp=0;
      TempImageWidth=lbm_headr->width;
      do {
	MaskTemp |= (*lbm_pixel_ptr++ !=0) << BitNum;
	if (++BitNum > 3) {
	  *vbm_mask_ptr++=MaskTemp;
	  MaskTemp=BitNum=0;
	}
      } while (--TempImageWidth);
      *vbm_mask_ptr++=(BitNum != 0)?MaskTemp:0;
   }

 }

 *VramStart=VramOffs;
 return (char far *) vbm_headr;
}


