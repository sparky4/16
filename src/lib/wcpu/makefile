CFLAGS=-0

all: wcpu.exe

wcpu.exe: wcpu.obj
	wcl $(CFLAGS) $<

wcpu.obj: wcpu.c
	wcl $(CFLAGS) -c $<

.NOCHECK
clean:
	del *.exe
	del *.obj
