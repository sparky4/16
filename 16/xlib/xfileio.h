/*-----------------------------------------------------------------------
;
; XFILEIO - header file
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
;  Terminology & notes:
;         VRAM ==   Video RAM
;         SRAM ==   System RAM
;         X coordinates are in pixels unless explicitly stated
;
;-----------------------------------------------------------------------*/

#ifndef _XFILEIO_H_
#define _XFILEIO_H_

#define F_RDONLY  0
#define F_WRONLY  1
#define F_RDWR    2

#define SEEK_START 0
#define SEEK_CURR  1
#define SEEK_END   2

#define FILE_ERR -1

#define FIO_INVALID_METHOD  1
#define FIO_FILE_NOT_FOUND  2
#define FIO_PATH_NOT_FOUND  3
#define FIO_HANDLE_UNAVAIL  4
#define FIO_ACCESS_DENIED   5
#define FIO_INVALID_HANDLE  6

extern int file_err;

/* FUNCTIONS =========================================================== */

#ifdef __cplusplus
extern "C" {
#endif


 int f_open(              /* Open a file returning its handle */
	 char * filename,
	 char access);

 int f_close(             /* Close a file  */
	 int handle);

 int f_read(              /* Read a block of data from a file */
	 int handle,
	 char * buffer,
	 int count);

 int f_write(             /* Write a block of data to a file */
	 int handle,
	 char * buffer,
	 int count);

 int f_readfar(              /* Read a block of data from a file */
	 int handle,
	 char far * buffer,
	 int count);

 int f_writefar(             /* Write a block of data to a file */
	 int handle,
	 char far * buffer,
	 int count);

 long int f_seek(         /* Position the file pointer */
	 int handle,
	 long int position,
	 char method_code);

 long int f_tell(         /* return position the file pointer */
	 int handle);

 long int f_filelength(   /* Return the length of the file */
	 int handle);


#ifdef __cplusplus
}
#endif


#endif


