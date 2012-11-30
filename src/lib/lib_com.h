#ifndef _LIBCOM_H_
#define _LIBCOM_H_
#include <dos.h>
#include <stdio.h>
#include <conio.h> // just for wait
#include <time.h> // just for wait
#include <i86.h>

typedef unsigned char byte;

void wait(clock_t wait);

#endif/*_LIBCOM_H_*/
