/* Output from p2c 1.21alpha-07.Dec.93, the Pascal-to-C translator */
/* From input file "adt2play.pas" */


#include <p2c/p2c.h>
/* p2c: adt2play.pas, line 2: Warning: Could not find module DPMI [271] */


#include "dpmi.h"
/* p2c: typconst.inc, line 4: Warning: Mismatched '$' signs [241] */
/* p2c: typconst.inc, line 4: 
 * Warning: Expected a semicolon, found 'ffffffff' [227] */

#ifndef A2PLAYER_H
#include "a2player.h"
#endif

#ifndef TIMERINT_H
#include "timerint.h"
#endif

#ifndef PARSERIO_H
#include "parserio.h"
#endif
/* p2c: adt2play.pas, line 3: 
 * Warning: Could not find module STRINGIO [271] */

#include "stringio.h"
/* p2c: adt2play.pas, line 3: 
 * Warning: Could not find module TXTSCRIO [271] */
#include "txtscrio.h"
/* p2c: unpk_lib.pas, line 1: 
 * Warning: Unrecognized character 015 in file [247] */
/* p2c: unpk_lib.pas, line 2: 
 * Warning: Unrecognized character 015 in file [247] */
/* p2c: unpk_lib.pas, line 3: 
 * Warning: Unrecognized character 015 in file [247] */
/* p2c: unpk_lib.pas, line 4: 
 * Warning: Expected IMPLEMENTATION, found a '/' [227] */
#ifndef UNPK_LIB_H
#include "unpk_lib.h"
#endif


#define _timer_xpos     198
#define _timer_ypos     5
#define _timer_color    1
#define _decay_bar_xpos  10
#define _decay_bar_ypos  140
#define _decay_bar_palette_start  250
#define _progress_xpos  8
#define _progress_ypos  155
#define _progress_color  251
#define _fname_xpos     8
#define _fname_ypos     170
#define _fname_color    255
#define _pos_str_xpos   8
#define _pos_str_ypos   186
#define _pos_str_color  252
/* p2c: adt2play.pas, line 30: Note: Characters >= 128 encountered [281] */
/* p2c: adt2play.pas, line 31: Note: Characters >= 128 encountered [281] */
/* p2c: adt2play.pas, line 32: Note: Characters >= 128 encountered [281] */
/* p2c: adt2play.pas, line 33: Note: Characters >= 128 encountered [281] */
/* p2c: adt2play.pas, line 39: Note: Characters >= 128 encountered [281] */



#define kBkSPC          0xe08
#define kESC            0x11b
#define kENTER          0x1c0d


Static double decay_bar_rise = 10.0, decay_bar_fall = 0.50;
Static boolean adjust_tracks = true, accurate_conv = true,
	       fix_c_note_bug = true;
Static uchar window_top = 8;

Static Char modname[15][40] = {
  "/\264DLiB TR/\264CK3R ][ module", "/\264DLiB TR/\264CK3R ][ G3 module",
  "/\264DLiB TR/\264CK3R ][ tiny module",
  "/\264DLiB TR/\264CK3R ][ G3 tiny module", "Amusic module",
  "XMS-Tracker module", "BoomTracker 4.0 module", "Digital-FM module",
  "HSC AdLib Composer / HSC-Tracker module", "MPU-401 tr\222kk\356r module",
  "Reality ADlib Tracker module", "Scream Tracker 3.x module",
  "FM-Kingtracker module", "Surprise! AdLib Tracker module",
  "Surprise! AdLib Tracker 2.0 module"
};

Static Char songdata_source[256];
Static Char songdata_title[256];
Static uchar load_flag;
Static unsigned short fkey;
Static uchar index_, last_order;
Static SearchRec dirinfo;

Static uchar buf1[sizeof(tVARIABLE_DATA)];
Static uchar buf2[65535L];
Static uchar buf3[65535L];
Static uchar buf4[65535L];
Static uchar temp_screen[8192];
Static short correction;
Static uchar entries, entries2, temp, temp2;
Static unsigned short dos_memavail;
Static Char _ParamStr[256][256];
Static boolean jukebox = false;


Static Void ResetF(f)
FILE **f;
{
  unsigned short fattr;

  strcpy(_debug_str_, "ADT2PLAY.PAS:ResetF_RW");
  GetFAttr(*f, fattr);
/* p2c: adt2play.pas, line 81:
 * Warning: Symbol 'GETFATTR' is not defined [221] */
  if ((fattr & ReadOnly) == ReadOnly) {
    FileMode = 0;
/* p2c: adt2play.pas, line 82:
 * Warning: Symbol 'FILEMODE' is not defined [221] */
  }
/* p2c: adt2play.pas, line 84:
 * Note: Can't interpret name argument in RESET [180] */
  rewind(*f);
}


Static Void BlockReadF(f, data, size, bytes_read)
FILE **f;
Anyptr data;
long size, *bytes_read;
{
  strcpy(_debug_str_, "ADT2PLAY.PAS:BlockReadF");
  *bytes_read = fread(data, 1, size, *f);
  P_ioresult = 0;
  if (false)
    *bytes_read = 0;
}


Static Void SeekF(f, fpos)
FILE **f;
long fpos;
{
  strcpy(_debug_str_, "ADT2PLAY.PAS:SeekF");
  _SETIO(fseek(*f, fpos, 0) == 0, EndOfFile);
}


Static Void CloseF(f)
FILE **f;
{
  strcpy(_debug_str_, "ADT2PLAY.PAS:CloseF");
  if (*f != NULL)
    fclose(*f);
  *f = NULL;
}
/* p2c: adt2play.pas, line 114: 
 * Warning: Expected BEGIN, found 'assembler' [227] */


extern unsigned short min PP((int value, int minimum));

main(argc, argv)
int argc;
Char *argv[];
{
  PASCAL_MAIN(argc, argv);
/* p2c: adt2play.pas, line 115:
 * Warning: Expected BEGIN, found 'asm' [227] */
  exit(EXIT_SUCCESS);
}
/* p2c: adt2play.pas, line 123: 
 * Warning: Junk at end of input file ignored [277] */




/* End. */
