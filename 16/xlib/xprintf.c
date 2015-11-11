#include <stdarg.h>
#include <string.h>
#include "xlib.h"
#include "xrect.h"
#include "xtext.h"


extern unsigned char CharWidth;
extern unsigned char CharHeight;

void x_printf(WORD x, WORD y,WORD ScrnOffs,WORD color,char *ln,...){
  char dlin[100],*dl=dlin;
  char ch;
  va_list ap;

  va_start(ap,ln);
  vsprintf(dlin,ln,ap);
  va_end(ap);

  while(*dl){
      x+=x_char_put(*dl++,x,y,ScrnOffs,color);
  }

}

void x_bgprintf(WORD x, WORD y,WORD ScrnOffs,WORD fgcolor,
  WORD bgcolor, char *ln,...){
  char dlin[100],*dl=dlin;
  char ch;
  va_list ap;

  va_start(ap,ln);
  vsprintf(dlin,ln,ap);
  va_end(ap);

  while(*dl){
      x_rect_fill(x,y,x+x_get_char_width(*dl),y+CharHeight,ScrnOffs,bgcolor);
      x+=x_char_put(*dl++,x,y,ScrnOffs,fgcolor);
  }

}
