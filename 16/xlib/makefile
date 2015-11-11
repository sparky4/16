# makefile MAKE V2.0 or higher


# Set the required model here, either s, c or l (LOWER CASE ONLY)
#

MODEL=h

XLIB_VERSION=06

# Set the compiler: either BCC or TCC
#
CC=wcc

TLIB=wlib

AFLAGS  = /ml /zi /d$(MODEL)
CFLAGS  = -m$(MODEL)
CPPFLAGS= -m$(MODEL)
LDFLAGS = -m$(MODEL)


%.o: %.c
   $(CC) -c $(CFLAGS) $<

%.o: *.asm
   wasm $(AFLAGS) $* \


XLIBOBJS= xprintf.o xpoint.o xline.o xmain.o xpal.o xpbitmap.o \
	 xrect.o xtext.o xcbitmap.o xdetect.o xbmtools.o         \
	 xfileio.o xrletool.o xbezier.o xpbmclip.o xvbitmap.o    \
	 xmakevbm.o xmouse.o xcircle.o xclippbm.o xcomppbm.o     \
	 xpolygon.o xvsync.o xfill.o xcbitm32.o


all: demo1.exe demo2.exe demo3.exe demo4.exe demo5.exe demo6.exe demo7.exe \
     demo8.exe demo9.exe demo10.exe

demo1.exe : demo1.o $(XLIBOBJS) xlib$(XLIB_VERSION)$(MODEL).lib
  $(CC) $(CFLAGS) demo1.o xlib$(XLIB_VERSION)$(MODEL).lib

demo2.exe : demo2.o $(XLIBOBJS) xlib$(XLIB_VERSION)$(MODEL).lib
  $(CC) $(CFLAGS) demo2.o xlib$(XLIB_VERSION)$(MODEL).lib

demo3.exe : demo3.o $(XLIBOBJS) xlib$(XLIB_VERSION)$(MODEL).lib
  $(CC) $(CFLAGS) demo3.o xlib$(XLIB_VERSION)$(MODEL).lib

demo4.exe : demo4.o $(XLIBOBJS) xlib$(XLIB_VERSION)$(MODEL).lib
  $(CC) $(CFLAGS) demo4.o xlib$(XLIB_VERSION)$(MODEL).lib

demo5.exe : demo5.o $(XLIBOBJS) xlib$(XLIB_VERSION)$(MODEL).lib
  $(CC) $(CFLAGS) demo5.o xlib$(XLIB_VERSION)$(MODEL).lib

demo6.exe : demo6.o $(XLIBOBJS) xlib$(XLIB_VERSION)$(MODEL).lib
  $(CC) $(CFLAGS) demo6.o xlib$(XLIB_VERSION)$(MODEL).lib

demo7.exe : demo7.o $(XLIBOBJS) xlib$(XLIB_VERSION)$(MODEL).lib
  $(CC) $(CFLAGS) demo7.o xlib$(XLIB_VERSION)$(MODEL).lib

demo8.exe : demo8.o $(XLIBOBJS) xlib$(XLIB_VERSION)$(MODEL).lib
  $(CC) $(CFLAGS) demo8.o xlib$(XLIB_VERSION)$(MODEL).lib

demo9.exe : demo9.o $(XLIBOBJS) xlib$(XLIB_VERSION)$(MODEL).lib
  $(CC) $(CFLAGS) demo9.o xlib$(XLIB_VERSION)$(MODEL).lib

demo10.exe : demo10.o $(XLIBOBJS) xlib$(XLIB_VERSION)$(MODEL).lib
  $(CC) $(CFLAGS) demo10.o xlib$(XLIB_VERSION)$(MODEL).lib


xlib$(XLIB_VERSION)$(MODEL).lib: $(XLIBOBJS)
  $(TLIB) xlib$(XLIB_VERSION)$(MODEL).lib \
    -+xmain -+xpoint -+xline -+xpal -+xrect -+xtext -+xprintf -+xbezier -+xfill -+xcbitm32
  $(TLIB) xlib$(XLIB_VERSION)$(MODEL).lib \
    -+xpbitmap -+xcbitmap -+xdetect -+xbmtools -+xfileio -+xrletool -+xpbmclip
  $(TLIB) xlib$(XLIB_VERSION)$(MODEL).lib \
    -+xvbitmap -+xmakevbm -+xmouse -+xcircle -+xclippbm -+xcomppbm -+xpolygon -+xvsync

# Object modules

xprintf.o : xprintf.c
xpoint.o  : xpoint.asm   xpoint.inc   xlib.inc  model.inc
xline.o   : xline.asm    xline.inc    xlib.inc  model.inc
xmain.o   : xmain.asm    xmain.inc    xlib.inc  model.inc
xpal.o    : xpal.asm     xpal.inc     xlib.inc  model.inc
xpbitmap.o: xpbitmap.asm xpbitmap.inc xlib.inc  model.inc
xpbmclip.o: xpbmclip.asm xpbmclip.inc xlib.inc  model.inc
xclippbm.o: xclippbm.asm xclippbm.inc xlib.inc  model.inc
xrect.o   : xpal.asm     xpal.inc     xlib.inc  model.inc
xtext.o   : xtext.asm    xtext.inc    xlib.inc  model.inc
xcbitmap.o: xcbitmap.asm xcbitmap.inc xlib.inc  model.inc
xcomppbm.o: xcomppbm.asm xcomppbm.inc xlib.inc  model.inc
xcbitm32.o: xcbitm32.c   
xdetect.o : xdetect.asm  xdetect.inc            model.inc
xbmtools.o: xbmtools.asm xbmtools.inc           model.inc
xfileio.o : xfileio.asm  xfileio.inc            model.inc
xrletool.o: xrletool.asm xrletool.inc           model.inc
xvbitmap.o: xvbitmap.asm xvbitmap.inc xlib.inc  model.inc
xmakevbm.o: xmakevbm.c   xvbitmap.h
xmouse.o  : xmouse.asm                xlib.inc  model.inc
xcircle.o : xcircle.asm  xcircle.inc  xlib.inc  model.inc
xpolygon.o: xpolygon.asm xpolygon.inc xlib.inc  model.inc
xvsync.o  : xvsync.asm   xvsync.inc   xlib.inc  model.inc
xfill.o   : xfill.asm    xfill.inc    xlib.inc  model.inc
