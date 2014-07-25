FLAGS=-0 -d3
all: test.exe

test.exe: test.obj modex16.obj
	wcl $(FLAGS) test.obj modex16.obj
	
test.obj: test.c lib\modex16.h
	wcl $(FLAGS) -c test.c
	
modex16.obj: lib\modex16.h lib\modex16.c
	wcl $(FLAGS) -c lib\modex16.c
	
clean: 
	del *.obj
	del *.exe
