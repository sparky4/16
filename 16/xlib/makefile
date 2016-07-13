!ifdef __LINUX__
REMOVECOMMAND=rm -f
COPYCOMMAND=cp -f
DIRSEP=/
OBJ=o
!else
REMOVECOMMAND=del
COPYCOMMAND=copy /y
DIRSEP=\
OBJ=obj
!endif

# Set the required model here, either s, c or l (LOWER CASE ONLY)
#
MODEL=h

XLIB_VERSION=06w

# Set the compiler: either BCC or TCC
#
CC=wcl
LIB=wlib

AFLAGS  = -0 -m$(MODEL)
CFLAGS  = -0 -mh
CPPFLAGS= -0 -m$(MODEL)
LDFLAGS = -0 -m$(MODEL)

#%.$(OBJ): %.c
#	$(CC) -c $(CFLAGS) $<

# $*$(OBJ).asm#: $*.asm#
# 	wasm $(AFLAGS) $* \

EXEC=demo1.exe demo2.exe demo3.exe demo4.exe demo5.exe demo6.exe demo7.exe demo8.exe# demo9.exe demo10.exe
XLIBOBJS= xprintf.$(OBJ) xpoint.$(OBJ) xline.$(OBJ) xmain.$(OBJ) xpal.$(OBJ) xpbitmap.$(OBJ) xrect.$(OBJ) xtext.$(OBJ) xcbitmap.$(OBJ) xdetect.$(OBJ) xbmtools.$(OBJ) xfileio.$(OBJ) xrletool.$(OBJ) xbezier.$(OBJ) xpbmclip.$(OBJ) xvbitmap.$(OBJ) xmakevbm.$(OBJ) xmouse.$(OBJ) xcircle.$(OBJ) xclippbm.$(OBJ) xcomppbm.$(OBJ) xpolygon.$(OBJ) xvsync.$(OBJ) xfill.$(OBJ) xcbitm32.$(OBJ)

all: $(XLIBOBJS) $(EXEC)

demo1.exe : demo1.$(OBJ) $(XLIBOBJS) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib
	$(CC) $(CFLAGS) demo1.$(OBJ) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib

demo2.exe : demo2.$(OBJ) $(XLIBOBJS) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib
	$(CC) $(CFLAGS) demo2.$(OBJ) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib

demo3.exe : demo3.$(OBJ) $(XLIBOBJS) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib
	$(CC) $(CFLAGS) demo3.$(OBJ) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib

demo4.exe : demo4.$(OBJ) $(XLIBOBJS) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib
	$(CC) $(CFLAGS) demo4.$(OBJ) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib

demo5.exe : demo5.$(OBJ) $(XLIBOBJS) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib
	$(CC) $(CFLAGS) demo5.$(OBJ) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib

demo6.exe : demo6.$(OBJ) $(XLIBOBJS) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib
	$(CC) $(CFLAGS) demo6.$(OBJ) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib

demo7.exe : demo7.$(OBJ) $(XLIBOBJS) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib
	$(CC) $(CFLAGS) demo7.$(OBJ) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib

demo8.exe : demo8.$(OBJ) $(XLIBOBJS) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib
	$(CC) $(CFLAGS) demo8.$(OBJ) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib

demo9.exe : demo9.$(OBJ) $(XLIBOBJS) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib
	$(CC) $(CFLAGS) demo9.$(OBJ) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib

demo10.exe : demo10.$(OBJ) $(XLIBOBJS) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib
	$(CC) $(CFLAGS) demo10.$(OBJ) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib

#
#	executable objects
#
demo1.$(OBJ) : demo1.c
	$(CC) -c $(CFLAGS) demo1.c
demo2.$(OBJ) : demo2.c
	$(CC) -c $(CFLAGS) demo2.c
demo3.$(OBJ) : demo3.c
	$(CC) -c $(CFLAGS) demo3.c
demo4.$(OBJ) : demo4.c
	$(CC) -c $(CFLAGS) demo4.c
demo5.$(OBJ) : demo5.c
	$(CC) -c $(CFLAGS) demo5.c
demo6.$(OBJ) : demo6.c
	$(CC) -c $(CFLAGS) demo6.c
demo7.$(OBJ) : demo7.c
	$(CC) -c $(CFLAGS) demo7.c
demo8.$(OBJ) : demo8.c
	$(CC) -c $(CFLAGS) demo8.c
# demo9.$(OBJ) : demo9.c
# 	$(CC) -c $(CFLAGS) demo9.c
# demo10.$(OBJ) : demo10.c
# 	$(CC) -c $(CFLAGS) demo10.c

#$(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib: $(XLIBOBJS)
#	$(LIB) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib \
#	 -+xmain -+xpoint -+xline -+xpal -+xrect -+xtext -+xprintf -+xbezier -+xfill -+xcbitm32
#	$(LIB) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib \
#	 -+xpbitmap -+xcbitmap -+xdetect -+xbmtools -+xfileio -+xrletool -+xpbmclip
#	$(LIB) $(XLIBOBJS)#xlib$(XLIB_VERSION)$(MODEL).lib \
#	 -+xvbitmap -+xmakevbm -+xmouse -+xcircle -+xclippbm -+xcomppbm -+xpolygon -+xvsync

# Object modules
xprintf.$(OBJ) : xprintf.c
	$(CC) -c $(CFLAGS) xprintf.c

xpoint.$(OBJ)  : xpoint.asm##			xpoint.inc	xlib.inc		model.inc
	$(CC) -c $(CFLAGS) xpoint.asm##	xpoint.inc	xlib.inc		model.inc
xline.$(OBJ) : xline.asm#				xline.inc		xlib.inc		model.inc
	$(CC) -c $(CFLAGS)  xline.asm#	xline.inc		xlib.inc		model.inc
xmain.$(OBJ): xmain.asm#			xmain.inc	xlib.inc		model.inc
	$(CC) -c $(CFLAGS) xmain.asm#	xmain.inc	xlib.inc		model.inc
xpal.$(OBJ)	 : xpal.asm#				xpal.inc		xlib.inc		model.inc
	$(CC) -c $(CFLAGS) xpal.asm#	xpal.inc		xlib.inc		model.inc
xpbitmap.$(OBJ): xpbitmap.asm#		xpbitmap.inc	xlib.inc		model.inc
	$(CC) -c $(CFLAGS) xpbitmap.asm#	xpbitmap.inc	xlib.inc		model.inc
xpbmclip.$(OBJ): xpbmclip.asm# 		xpbmclip.inc	xlib.inc		model.inc
	$(CC) -c $(CFLAGS) xpbmclip.asm# xpbmclip.inc	xlib.inc		model.inc
xclippbm.$(OBJ): xclippbm.asm#		xclippbm.inc	xlib.inc		model.inc
	$(CC) -c $(CFLAGS) xclippbm.asm# xclippbm.inc	xlib.inc		model.inc
xrect.$(OBJ): xpal.asm#	  			xpal.inc	  	xlib.inc		model.inc
	$(CC) -c $(CFLAGS) xpal.asm#	xpal.inc	  	xlib.inc		model.inc
xtext.$(OBJ): xtext.asm#	 			xtext.inc	xlib.inc		model.inc
	$(CC) -c $(CFLAGS) xtext.asm#	xtext.inc	xlib.inc		model.inc
xcbitmap.$(OBJ): xcbitmap.asm#		xcbitmap.inc	xlib.inc		model.inc
	$(CC) -c $(CFLAGS) xcbitmap.asm#	xcbitmap.inc	xlib.inc		model.inc
xcomppbm.$(OBJ): xcomppbm.asm# xcomppbm.inc xlib.inc			model.inc
	$(CC) -c $(CFLAGS) xcomppbm.asm# xcomppbm.inc xlib.inc	model.inc
xcbitm32.$(OBJ): xcbitm32.c
	$(CC) -c $(CFLAGS) xcbitm32.c
xdetect.$(OBJ) : xdetect.asm#			xdetect.inc			model.inc
	$(CC) -c $(CFLAGS) xdetect.asm#	xdetect.inc			model.inc
xbmtools.$(OBJ): xbmtools.asm#		xbmtools.inc			model.inc
	$(CC) -c $(CFLAGS) xbmtools.asm#	 xbmtools.inc			model.inc
xfileio.$(OBJ) : xfileio.asm#			xfileio.inc			model.inc
	$(CC) -c $(CFLAGS) xfileio.asm#	xfileio.inc			model.inc
xrletool.$(OBJ): xrletool.asm#			xrletool.inc			model.inc
	$(CC) -c $(CFLAGS) xrletool.asm#	xrletool.inc			model.inc
xvbitmap.$(OBJ): xvbitmap.asm#		xvbitmap.inc xlib.inc	model.inc
	$(CC) -c $(CFLAGS) xvbitmap.asm#	xvbitmap.inc xlib.inc	model.inc
xmakevbm.$(OBJ): xmakevbm.c	xvbitmap.h
	$(CC) -c $(CFLAGS) xmakevbm.c	xvbitmap.h
xmouse.$(OBJ)  : xmouse.asm#			xlib.inc				model.inc
	$(CC) -c $(CFLAGS) xmouse.asm#	xlib.inc				model.inc
xcircle.$(OBJ) : xcircle.asm#			xcircle.inc xlib.inc	model.inc
	$(CC) -c $(CFLAGS) xcircle.asm#	xcircle.inc xlib.inc	model.inc
xpolygon.$(OBJ): xpolygon.asm#		xpolygon.inc xlib.inc	model.inc
	$(CC) -c $(CFLAGS) xpolygon.asm# xpolygon.inc xlib.inc	model.inc
xvsync.$(OBJ)  : xvsync.asm#			xvsync.inc xlib.inc	model.inc
	$(CC) -c $(CFLAGS) xvsync.asm#	xvsync.inc xlib.inc	model.inc
xfill.$(OBJ)	: xfill.asm#				xfill.inc	xlib.inc		model.inc
	$(CC) -c $(CFLAGS) xfill.asm#	xfill.inc	xlib.inc		model.inc

xbezier.$(OBJ): xbezier.asm
	$(CC) -c $(CFLAGS) xbezier.asm

#
#other~
#
clean: .symbolic
	@$(REMOVECOMMAND) $(EXEC)
	@$(REMOVECOMMAND) *.$(OBJ)
