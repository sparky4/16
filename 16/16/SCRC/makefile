all: test.exe fade.exe

fade.exe: fade.o palette.o
	wcl -l=dos -0 fade.o palette.o

test.exe: test.o palette.o
	wcl -l=dos -0 test.o palette.o

test.o: test.c
	wcl -0 -c test.c

fade.o: fade.c
	wcl -0 -c fade.c

palette.o: palette.h palette.c constant.h
	wcl -0 -c palette.c
