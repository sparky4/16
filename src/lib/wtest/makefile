CFLAGS=-0

all: wtest.exe

wtest.exe: wtest.obj
	wcl $(CFLAGS) $<

wtest.obj: wtest.c
	wcl $(CFLAGS) -c $<

.NOCHECK
clean:
	del *.exe
	del *.obj
