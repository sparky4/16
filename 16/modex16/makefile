all: test.exe

test.exe: test.obj modex16.obj
	wcl -0 -d2 test.obj modex16.obj

test.obj: test.c modex16.h
	wcl -0 -d2 -c test.c

modex16.obj: modex16.h modex16.c
	wcl -0 -d2 -c modex16.c
