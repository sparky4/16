#ifndef _DOSLIB_COMM_H_
#define _DOSLIB_COMM_H_
#include "src\lib\lib_com.h"

/* Maximum number of scan codes on keyboard controllers */
#define NUM_SCANCODES   128

#ifdef __cplusplus      /* Functions must be declared C style */
extern "C" {
#endif
void interrupt far newkb(void);
//extern void interrupt (far *oldkb)(void);
void setkb(int vq);
int keyp(byte c);

/* Define macro */
//#define kepn(c) key[c & 0x7F]

#ifdef __cplusplus
}
#endif

#endif/*_DOSLIB_COMM_H_*/
