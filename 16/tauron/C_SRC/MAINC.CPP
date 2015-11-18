//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//=-                                                                         -=
//=-                   Tauron VGA Utilities Version 3.0                      -=
//=-                      Released September 20, 1998                        -=
//=-                                                                         -=
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//=- Copyright (c) 1997, 1998 by Jeff Morgan  =-= This code is FREE provided -=
//=- All Rights Reserved.                     =-= that you put my name some- -=
//=-                                          =-= where in your credits.     -=
//=- DISCLAIMER:                              =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//=- I assume no responsibility whatsoever for any effect that this package, -=
//=- the information contained therein or the use thereof has on you, your   -=
//=- sanity, computer, spouse, children, pets or anything else related to    -=
//=- you or your existance. No warranty is provided nor implied with this    -=
//=- source code.                                                            -=
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include "tauron.h"

void MainScreen()
{
   clrscr();
   printf("Tauron VGA Utilities v3.0\n"
          "Copyright (c) 1998 Jeffrey Morgan.  All Rights Reserved.\n");
   textcolor(12);
   cprintf("Modes with a ! are not implemented      Modes with a % are unchained\r\n");
   textcolor(7);
   printf("Select a video mode to test:\n"
          "Key              Videomode          Key              Videomode\n"
          "================================================================================"
          " 0) 00H, 01H -- 40 x 25 x 16         %F) Mode D   -- 360 x 200 x 256\n"
          " 1) Mode L   -- 40 x 43 x 16         %G) Mode E   -- 360 x 240 x 256\n"
          " 2) Mode M   -- 40 x 50 x 16         %H) Mode F   -- 360 x 350 x 256\n"
          "!3) 07H      -- 80 x 25 x 2          %I) Mode G   -- 360 x 400 x 256\n"
          " 4) 02H, 03H -- 80 x 25 x 16         %J) Mode H   -- 360 x 480 x 256\n"
          " 5) Mode J   -- 80 x 43 x 16          K) 06H      -- 640 x 200 x 2\n"
          " 6) Mode K   -- 80 x 50 x 16         !L) 0FH      -- 640 x 350 x 2\n"
          " 7) 04H, 05H -- 320 x 200 x 4         M) 11H      -- 640 x 480 x 2\n"
          " 8) 0DH      -- 320 x 200 x 16        N) 0EH      -- 640 x 200 x 16\n"
          " 9) 13H      -- 320 x 200 x 256       O) 10H      -- 640 x 350 x 16\n"
          "%A) Chain 4  -- 320 x 200 x 256       P) Mode I   -- 640 x 400 x 16\n"
          "%B) Mode X   -- 320 x 240 x 256       Q) 12H      -- 640 x 480 x 16\n"
          "%C) Mode A   -- 320 x 350 x 256\n"
          "%D) Mode B   -- 320 x 400 x 256       R) Dual Font Test\n"
          "%E) Mode C   -- 320 x 480 x 256\n"
          "===============================================================================\n"
          "Hit 'ESC' to exit.  Press a key [0-9 or A-O]>");
}

void SetMode03h()
{
   asm MOV AX,0003H
   asm INT 10H
}

void main()
{
   int ok, key;
   ok = 1;
   MainScreen();
   while (ok)
   {
      if( (key = getch()) == 0 )
         key = 256 * getch();
      switch( key )
      {
         case Escape : ok = 0; break;

         case '0'          :
            SetVideoMode(MODE00H); TextTest(); getch();
            SetMode03h(); MainScreen(); break;

         case '1':
            SetVideoMode(MODE_L); TextTest(); getch();
            SetMode03h(); MainScreen(); break;

         case '2':
            SetVideoMode(MODE_M); TextTest(); getch();
            SetMode03h(); MainScreen(); break;

         case '4'          :
            SetVideoMode(MODE03H); TextTest(); getch();
            SetMode03h(); MainScreen(); break;

         case '5':
            SetVideoMode(MODE_J); TextTest(); getch();
            SetMode03h(); MainScreen(); break;

         case '6':
            SetVideoMode(MODE_K); TextTest(); getch();
            SetMode03h(); MainScreen(); break;

         case '7':
            SetVideoMode(MODE04H); Clear04H(); getch();
            SetMode03h(); MainScreen(); break;

         case '8'          :
            SetVideoMode(MODE0DH); Clear0DH(12); /*ModeTest();*/ getch();
            SetMode03h(); MainScreen(); break;

         case '9':
            SetVideoMode(MODE13H); ModeTest(); getch();
            SetMode03h(); MainScreen(); break;

         case 'a': case 'A':
            SetVideoMode(CHAIN4); ModeTest(); getch();
            SetMode03h(); MainScreen(); break;

         case 'b': case 'B':
            SetVideoMode(MODE_X); ModeTest(); getch();
            SetMode03h(); MainScreen(); break;

         case 'c': case 'C':
            SetVideoMode(MODE_A); ModeTest(); getch();
            SetMode03h(); MainScreen(); break;

         case 'd': case 'D':
            SetVideoMode(MODE_B); ModeTest(); getch();
            SetMode03h(); MainScreen(); break;

         case 'e': case 'E':
            SetVideoMode(MODE_C); ModeTest(); getch();
            SetMode03h(); MainScreen(); break;

         case 'f': case 'F':
            SetVideoMode(MODE_D); ModeTest(); getch();
            SetMode03h(); MainScreen(); break;

         case 'g': case 'G':
            SetVideoMode(MODE_E); ModeTest(); getch();
            SetMode03h(); MainScreen(); break;

         case 'h': case 'H':
            SetVideoMode(MODE_F); ModeTest(); getch();
            SetMode03h(); MainScreen(); break;

         case 'i': case 'I':
            SetVideoMode(MODE_G); ModeTest(); getch();
            SetMode03h(); MainScreen(); break;

         case 'j': case 'J':
            SetVideoMode(MODE_H); ModeTest(); getch();
            SetMode03h(); MainScreen(); break;

         case 'k': case 'K':
            SetVideoMode(MODE06H); Clear06H(); getch();
            SetMode03h(); MainScreen(); break;

         case 'm': case 'M':
            SetVideoMode(MODE11H); ModeTest(); getch();
            SetMode03h(); MainScreen(); break;

         case 'n': case 'N':
            SetVideoMode(MODE0EH); ModeTest(); getch();
            SetMode03h(); MainScreen(); break;

         case 'o': case 'O':
            SetVideoMode(MODE10H); ModeTest(); getch();
            SetMode03h(); MainScreen(); break;

         case 'p': case 'P':
            SetVideoMode(MODE_I); ModeTest(); getch();
            SetMode03h(); MainScreen(); break;

         case 'q': case 'Q':
            SetVideoMode(MODE12H); ModeTest(); getch();
            SetMode03h(); MainScreen(); break;

         case 'r': case 'R':
            SetVideoMode(MODE03H); DualTest();
            SetMode03h(); MainScreen(); break;

         default     : break;
      }
   }
   asm MOV AX,0003h
   asm int 10h
}
