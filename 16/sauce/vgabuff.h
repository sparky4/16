#ifndef vgabuff_h
#define vgabuff_h

//allocates 320 * height bytes for buff
int vbinit(int height);

//frees space held by buff
vbclose();

/********** dbtovga *****************************
* function to copy from memory to vga
* dest = location in vga to start copying to
* size = source size / 4 (size mod 4 must = 0)
************************************************/
void dbtovga(unsigned char far* dest, unsigned char far* source, int size);
#pragma aux dbtovga=     \
    "rep movsd"  \
    parm [es di] [ds si] [cx];

/************* getpcximage ***********************************
*  function reads a 320x200x256 pcx file and outputs to screen
*  if plt = 0 then the palette is not read
*  returns 0 if file unopened or screen not allocated
*************************************************************/
int getpcximage(char fname[], int plt);

/***************************************************************
*  new pcx functions for single graphics file created 10/12/97
*  still use whole 320x200 for each sprite/tile - change soon!
*  note:  no error checking, yet
*  functions should be moved to own file
***************************************************************/

//opens pcx file and reads first 320x200 image, no palette
//returns 0 if file not found, 1 otherwise
int pcxopen(char fname[]);

//reads next 320x200 image in file opened by pcxopen
void pcxfadvance();

//reads the pallette into the vga registers and closes the file
void pcxclose();


#endif
