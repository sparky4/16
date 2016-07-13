/*************************************************************************

DEMO 2

Demonstrates the speed difference between compiled bitmap,  conventional
masked planar bitmap, and video bitmap blits.

**************************************************************************/



#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <ctype.h>
#include <malloc.h>
#include <dos.h>
#include <dir.h>
#include <string.h>

#include "xlib.h"
#include  "xtext.h"
#include  "xrect.h"

/* Macro to make pointer parameters model independent */
#define FARPTR(x) (MK_FP(FP_SEG(x),FP_OFF(x)))

char *swidth[10]={"0","1","2","3","4","5","6","7","8","9"};
char far *fonts[20];
char names[20][20];
int i,fcount=0;
    char c;

typedef struct {
  int  dummy;
  char height;
  char width;
} header;

void load_user_fonts(){
  FILE *f;
  long len;
  struct ffblk ffblock;

  if(findfirst("*.fnt",&ffblock,0)!=0){
    printf("No Fonts found in current directory!\n");
    exit(0);
  };
  do {
    printf("Loading font \"%s\"...\n",ffblock.ff_name);
    strncpy(names[fcount],ffblock.ff_name,14);

    f=fopen(ffblock.ff_name,"rb");

    fseek(f,0,SEEK_END);
    len=ftell(f);
    fseek(f,0,SEEK_SET);

    fonts[fcount] = farmalloc(len);
    if (!fonts[fcount]){
      printf("Out of memory");
      if (fcount!=0){
	printf("- No more fonts can be loaded\n");
	fclose(f);
	goto NoMoreFonts;
      } else printf("\n");

      exit(0);
    };

    for (i=0;i<len;i++){
      fread(&c,1,1,f);
      *(fonts[fcount]+i)=c;
    }

    fclose(f);
    fcount++;

  } while (!findnext(&ffblock));

NoMoreFonts:
  printf("\n Press 'v' to view, any other key to quit\n");
  c=getch();
  if (c!='V' && c!='v'){
    x_text_mode();
    exit(0);
  }

  x_text_init();
  x_set_mode(X_MODE_320x240,0);
  x_register_userfont(fonts[0]);
  x_set_font(2);
}

		 //......................................//
char *text[30] = {"EXTRACT: Stephen King's \"SALEM'S LOT\" ",
		  "",
		  "The memory rose up in almost total    ",
		  "sensory reference, and for the moment ",
		  "of its totality he was paralyzed. He  ",
		  "could even smell the plaster and the  ",
		  "wild odour of nesting animals. It     ",
		  "seemed to him that the plain varnished",
		  "door of Matt Burke's guest room stood ",
		  "between him and all the secrets of    ",
		  "Hell. Then he twisted the knob and    ",
		  "pushed the door handle inwards...     ",
		  "",
		  "ABCDEFGHIJKLMNOPQRSTUVWXYZ            ",
		  "abcdefghijklmnopqrstuvwxyz 0123456789 ",
		  "~!@#$%^&*()_+|`-=\\{}[]:\";'<>?,./    ",
		  NULL};





void main(){
    int textline;
    int strindex;
    int height;
    load_user_fonts();



    for (i=0;i<fcount;i++){
      x_set_font(FONT_8x8);
      x_rect_fill(0, 0, 319, 239, 0, 0);
      x_line(0,9,319,9,14,0);
      x_line(0,ScrnPhysicalHeight-10,319,ScrnPhysicalHeight-10,14,0);

      x_bgprintf(0,0,0,14,0,"Font \"%s\" H=%d,W=%s",names[i],
	       (int)*(fonts[i]+2),
	       (*(fonts[i]+3)==0)?"Variable":swidth[*(fonts[i]+3)]);
      x_bgprintf(0,ScrnPhysicalHeight-8,0,14,0,
	  "Press a key for next font...");

      x_register_userfont(fonts[i]);
      x_set_font(FONT_USER);

      height=(int)*(fonts[i]+2)+1;
      textline=12;
      strindex=0;
      while(text[strindex]){
       x_printf(0,textline,0,14,text[strindex++]);
       textline+=height;
      }

      getch();
    }

    x_text_mode();
}

