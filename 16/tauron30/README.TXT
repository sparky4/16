=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
=-                                                                         -=
=-                   Tauron VGA Utilities Version 3.0                      -=
=-                      Released September 20, 1998                        -=
=-                                                                         -=
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
=- Copyright (c) 1997, 1998 by Jeff Morgan  =-= This code is FREE provided -=
=- All Rights Reserved.                     =-= that you put my name some- -=
=-                                          =-= where in your credits.     -=
=- DISCLAIMER:                              =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
=- I assume no responsibility whatsoever for any effect that this package, -=
=- the information contained therein or the use thereof has on you, your   -=
=- sanity, computer, spouse, children, pets or anything else related to    -=
=- you or your existance. No warranty is provided nor implied with this    -=
=- source code.                                                            -=
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

The code in this package will switch video modes on any standard VGA video
card.  This is accomplished by programming the VGA Registers directly.

##############################################################################
####          Mode 07H and Mode 0FH are still being worked on.            ####
##############################################################################


=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
=- Files included in this package: -=                      -=
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=                      -=
=-  Assembly version compiles with TASM 4.1 and TLINK 7.0  -=
=-  C version compiles with Borland C++ 5.01               -=
=-                                                         -=
=-   README.TXT       You are reading it.                  -=
=-   WHATSNEW.TXT     Whats new in this version            -=
=-   USAGE.TXT        How to use these routines.  Also     -=
=-                    contains a complete list of the      -=
=-                    videomodes supported.                -=
=-                                                         -=
=-   MAIN.EXE         Assembly version executable demo     -=
=-   MAINC.EXE        C version executable demo            -=
=-   FEDIT.EXE        Font editor                          -=
=-   FEDIT.TXT        Docs for the font editor             -=
=-                                                         -=
=-   ASM_SRC          The assembly source directory        -=
=-    +->MAIN.ASM     The Main source file                 -=
=-    +->MODES.ASM    The mode switching file              -=
=-    +->CLEAR.ASM    The clear screen routines            -=
=-    +->TESTS.ASM    The videomode tests                  -=
=-    +->DUAL.ASM     The dual font setup and test         -=
=-    +->FONT1.ASM    Font 1 for dual font test            -=
=-    +->FONT2.ASM    Font 2 for dual font test            -=
=-    +->MODES.INC    The includes for each videomode      -=
=-                                                         -=
=-   C_SRC            The C source directory               -=
=-    +->MAINC.CPP    The main source file                 -=
=-    +->MODES.CPP    The mode switching file              -=
=-    +->CLEAR.CPP    The clear screen routines            -=
=-    +->TESTS.CPP    The videomode tests                  -=
=-    +->TAURON.H     The main header file                 -=
=-    +->FONT1.H       Font 1 for dual font test           -=
=-    +->FONT2.H      Font 2 for dual font test            -=
=-    +->MODES_C.INC  The includes for each videomode      -=
=-    +->PALETTE.INC  The standard palette                 -=
=-                                                         -=
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

When running the demo:

   In graphics modes, there is a vertical line every 40 pixels with a longer
   vertical line every 80 pixels.

   I have not yet figured out how to program modes 04H, 05H, or 06H.  Once
   the mode is set, I only clear the mode to a specified color.



The VGA BIOS must be called once at the beginning of the routines so that we
know where the fonts are stored.  Once this information is known we no longer
need the BIOS.

The reason for starting this package is that I am writing an Operating
System. (There are a lot of us out there I know.)  I wanted to be able to
program standard VGA cards for my OS.

This is not a graphics library although there are some clear screen and pixel
routines around.  It only sets the video mode. This works in real mode as
well as protected mode.  To use it in protected mode you will have to modify
it to suit your own needs.

The use of the fonts stored in BIOS ROM is now included.  We are using what is
already built into the VGA BIOS.


E-mail:      kinfira@hotmail.com
Homepage:    http://home.onestop.net/kinfira/

Drop me a line if you find this package useful.

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
=-= Credits:  =-=                                                         =-=
=-=-=-=-=-=-=-=-=                                                         =-=
=-= A great book: "Programmer's Guide to the EGA, VGA, and Super VGA      =-=
=-= Cards" 3rd Edition   Richard F. Ferraro   ISBN 0-201-62490-7          =-=
=-= Addison-Wesley Publishing Company.                                    =-=
=-=                                                                       =-=
=-= A very excellent book: "Programmer's Guide to the EGA & VGA Cards"    =-=
=-= Richard F. Ferraro   ISBN 0-201-12692-3                               =-=
=-= Addison-Wesley Publishing Company.                                    =-=
=-=                                                                       =-=
=-= Anthony A.J. Williams for writing MODES.ASM, which I used to help     =-=
=-= write the Text Mode routine.  I specifically used the SetFont         =-=
=-= procedure which was instrumental in getting the code for Mode 03h     =-=
=-= working.                                                              =-=
=-=                                                                       =-=
=-= Boone (boone@ucsd.edu) for the docs on programming the VGA Registers. =-=
=-=                                                                       =-=
=-= I also used a really great program called TWEAK to get some of the    =-=
=-= values for the non-standard modes.                                    =-=
=-=                                                                       =-=
=-= Denthor of ASPHYXIA for his trainer on Chain-4 Mode.                  =-=
=-=                                                                       =-=
=-= Robert Schmidt <robert@stud.unit.no> for the great doc in PCGPE 1.0   =-=
=-= about Mode X.  This helped a great deal.                              =-=
=-=                                                                       =-=
=-= Michael Abrash for his super excellent VGA articles on everything,    =-=
=-= plus all of his books and public domain code.                         =-=
=-=                                                                       =-=
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


=-=-=-=-=-=-=-=-=-=-=-=
=- Revision History: -=
=-=-=-=-=-=-=-=-=-=-=-=
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Version 3.0 released September 20, 1998
   I added 2 new text modes, 40x43 and 40x50.  Also I have added a new dual
   font test which uses 2 of the VGA's 8 font maps.


=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Version 2.1 released July 5, 1998
   The only change is that in the C version, the routine to read fronts from
   BIOS is now working.  This routine reads a font from the BIOS and places
   it in memory for use in a textmode.


=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Version 2.0 released June 22, 1998
    I have added more videomodes.  I have also written a version of the
    source in C.  So the routines are available in Assembly and C.  I have
    also written some documentation for the font editor.  So now the only
    modes that are not implemented are the monochrome modes which will still
    be worked on.


Tauron VGA Utilities
Version 1.0 released June 14, 1998
    I changed the name because that other was a little too long.  B->
    I have packaged in a small font editor.  I have also rearranged the mode
    numbers and added a few more video modes that will be implemented.


=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Source code for switching videomodes without using BIOS
Version 4.1 released May 28, 1998

New in this version:
    We call a BIOS function to find out where in memory the fonts are stored.
    Since this is different on every cards' VGA BIOS, it is absolutely
    necessary so that in text modes, the proper fonts are displayed.


=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Version 4.0 released March 5, 1998

New in this version:
    All VGA Modes are supported! Plus Chain-4 Mode, ModeX, ModeA,
    ModeB and ModeC.  Modes 07H and 0FH are still being worked on.

    Use of Fonts stored in ROM.
    Custom Font data stored in system memory is no longer needed.


=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Version 3.0 released January 2, 1998
    Includes Modes 03h, 10h, 12h, and 13h plus Chain-4 Mode, ModeX, ModeA,
    ModeB and ModeC.

New in this version: Mode 10h (640x350x16), Mode C (640x400x16)


=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Version 2.5 not released
    Includes Modes 03h, 12h, and 13h plus Chain-4 Mode, ModeX, ModeA, and
    ModeB.

New in this version: VGA register data takes up less space.


=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Version 2.1 released Nov. 21, 1997
    Includes Modes 03h, 12h, and 13h plus Chain-4 Mode, ModeX, ModeA, and
    ModeB.


=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Version 2.0 released Nov. 10, 1997
    Includes Modes 03h, 12h, and 13h plus Chain-4 Mode and ModeX.


=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Version 1.5 released late Oct. 1997
    Includes Modes 03h, 12h, and 13h.  Fully functional


=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Version 1.1 released in Oct. 12, 1997
   Includes Mode 13h, Mode 03h finally works. Realized that I needed to reset
   the text mode font.


=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
Version 1.0 released in Oct. 7, 1997
   Included Mode 13h, Mode 03h partially. (Did not have setfont routine)


=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
