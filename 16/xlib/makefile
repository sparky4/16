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


%.c.obj:
   $(CC) -c $(CFLAGS) $<

%.asm.obj:
   wasm $(AFLAGS) $* \


XLIBOBJS= xprintf.obj xpoint.obj xline.obj xmain.obj xpal.obj xpbitmap.obj \
	 xrect.obj xtext.obj xcbitmap.obj xdetect.obj xbmtools.obj         \
	 xfileio.obj xrletool.obj xbezier.obj xpbmclip.obj xvbitmap.obj    \
	 xmakevbm.obj xmouse.obj xcircle.obj xclippbm.obj xcomppbm.obj     \
	 xpolygon.obj xvsync.obj xfill.obj xcbitm32.obj


all: demo1.exe demo2.exe demo3.exe demo4.exe demo5.exe demo6.exe demo7.exe \
     demo8.exe demo9.exe demo10.exe

demo1.exe : demo1.obj $(XLIBOBJS) xlib$(XLIB_VERSION)$(MODEL).lib
  $(CC) $(CFLAGS) demo1.obj xlib$(XLIB_VERSION)$(MODEL).lib

demo2.exe : demo2.obj $(XLIBOBJS) xlib$(XLIB_VERSION)$(MODEL).lib
  $(CC) $(CFLAGS) demo2.obj xlib$(XLIB_VERSION)$(MODEL).lib

demo3.exe : demo3.obj $(XLIBOBJS) xlib$(XLIB_VERSION)$(MODEL).lib
  $(CC) $(CFLAGS) demo3.obj xlib$(XLIB_VERSION)$(MODEL).lib

demo4.exe : demo4.obj $(XLIBOBJS) xlib$(XLIB_VERSION)$(MODEL).lib
  $(CC) $(CFLAGS) demo4.obj xlib$(XLIB_VERSION)$(MODEL).lib

demo5.exe : demo5.obj $(XLIBOBJS) xlib$(XLIB_VERSION)$(MODEL).lib
  $(CC) $(CFLAGS) demo5.obj xlib$(XLIB_VERSION)$(MODEL).lib

demo6.exe : demo6.obj $(XLIBOBJS) xlib$(XLIB_VERSION)$(MODEL).lib
  $(CC) $(CFLAGS) demo6.obj xlib$(XLIB_VERSION)$(MODEL).lib

demo7.exe : demo7.obj $(XLIBOBJS) xlib$(XLIB_VERSION)$(MODEL).lib
  $(CC) $(CFLAGS) demo7.obj xlib$(XLIB_VERSION)$(MODEL).lib

demo8.exe : demo8.obj $(XLIBOBJS) xlib$(XLIB_VERSION)$(MODEL).lib
  $(CC) $(CFLAGS) demo8.obj xlib$(XLIB_VERSION)$(MODEL).lib

demo9.exe : demo9.obj $(XLIBOBJS) xlib$(XLIB_VERSION)$(MODEL).lib
  $(CC) $(CFLAGS) demo9.obj xlib$(XLIB_VERSION)$(MODEL).lib

demo10.exe : demo10.obj $(XLIBOBJS) xlib$(XLIB_VERSION)$(MODEL).lib
  $(CC) $(CFLAGS) demo10.obj xlib$(XLIB_VERSION)$(MODEL).lib


xlib$(XLIB_VERSION)$(MODEL).lib: $(XLIBOBJS)
  $(TLIB) xlib$(XLIB_VERSION)$(MODEL).lib \
    -+xmain -+xpoint -+xline -+xpal -+xrect -+xtext -+xprintf -+xbezier -+xfill -+xcbitm32
  $(TLIB) xlib$(XLIB_VERSION)$(MODEL).lib \
    -+xpbitmap -+xcbitmap -+xdetect -+xbmtools -+xfileio -+xrletool -+xpbmclip
  $(TLIB) xlib$(XLIB_VERSION)$(MODEL).lib \
    -+xvbitmap -+xmakevbm -+xmouse -+xcircle -+xclippbm -+xcomppbm -+xpolygon -+xvsync

# Object modules

xprintf.obj : xprintf.c
xpoint.obj  : xpoint.asm   xpoint.inc   xlib.inc  model.inc
xline.obj   : xline.asm    xline.inc    xlib.inc  model.inc
xmain.obj   : xmain.asm    xmain.inc    xlib.inc  model.inc
xpal.obj    : xpal.asm     xpal.inc     xlib.inc  model.inc
xpbitmap.obj: xpbitmap.asm xpbitmap.inc xlib.inc  model.inc
xpbmclip.obj: xpbmclip.asm xpbmclip.inc xlib.inc  model.inc
xclippbm.obj: xclippbm.asm xclippbm.inc xlib.inc  model.inc
xrect.obj   : xpal.asm     xpal.inc     xlib.inc  model.inc
xtext.obj   : xtext.asm    xtext.inc    xlib.inc  model.inc
xcbitmap.obj: xcbitmap.asm xcbitmap.inc xlib.inc  model.inc
xcomppbm.obj: xcomppbm.asm xcomppbm.inc xlib.inc  model.inc
xcbitm32.obj: xcbitm32.c   
xdetect.obj : xdetect.asm  xdetect.inc            model.inc
xbmtools.obj: xbmtools.asm xbmtools.inc           model.inc
xfileio.obj : xfileio.asm  xfileio.inc            model.inc
xrletool.obj: xrletool.asm xrletool.inc           model.inc
xvbitmap.obj: xvbitmap.asm xvbitmap.inc xlib.inc  model.inc
xmakevbm.obj: xmakevbm.c   xvbitmap.h
xmouse.obj  : xmouse.asm                xlib.inc  model.inc
xcircle.obj : xcircle.asm  xcircle.inc  xlib.inc  model.inc
xpolygon.obj: xpolygon.asm xpolygon.inc xlib.inc  model.inc
xvsync.obj  : xvsync.asm   xvsync.inc   xlib.inc  model.inc
xfill.obj   : xfill.asm    xfill.inc    xlib.inc  model.inc
