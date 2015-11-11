/*-----------------------------------------------------------------------
;
; XRLETOOL - header file
;
;
;
; ****** XLIB - Mode X graphics library                ****************
; ******                                               ****************
; ****** Written By Themie Gouthas                     ****************
;
; egg@dstos3.dsto.gov.au
; teg@bart.dsto.gov.au
;
;-----------------------------------------------------------------------*/

#ifndef _XRLETOOL_H_
#define _XRLETOOL_H_

extern unsigned int RLE_last_buff_offs;

/* FUNCTIONS =========================================================== */

extern unsigned int x_buff_RLEncode( /* Compress data buffer               */
	 char far * source_buff,
	 char far * dest_buff,
	 unsigned int count);

extern unsigned int x_buff_RLE_size( /* Calculate buffer's compressed size */
	 char far * source_buff,
	 unsigned int count);


extern unsigned int x_buff_RLDecode( /* Uncompress data buffer             */
	 char far * source_buff,
	 char far * dest_buff);

extern int x_file_RLEncode( /* Compress data buffer to file                */
	 int handle,
	 char far * src_buff,
	 unsigned int count);

extern int x_file_RLDecode( /* Uncompress data file                        */
	 int file_handle,
	 char far * dest_buff);


#endif


