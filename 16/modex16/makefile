FLAGS=-0 -d3
all: test.exe pcxtest.exe test2.exe

test.exe: test.obj modex16.obj
	wcl $(FLAGS) test.obj modex16.obj
	
test2.exe: test2.obj modex16.obj
	wcl $(FLAGS) test2.obj modex16.obj
	
pcxtest.exe: pcxtest.obj modex16.obj
	wcl $(FLAGS) pcxtest.obj modex16.obj

test.obj: test.c modex16.h
	wcl $(FLAGS) -c test.c
	
test2.obj: test2.c modex16.h
	wcl $(FLAGS) -c test2.c
	
pcxtest.obj: pcxtest.c modex16.h
	wcl $(FLAGS) -c pcxtest.c

modex16.obj: modex16.h modex16.c
	wcl $(FLAGS) -c modex16.c
	
clean: 
	del *.obj
	del *.exe
