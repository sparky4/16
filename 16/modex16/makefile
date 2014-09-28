FLAGS=-0 
all: test.exe pcxtest.exe test2.exe scroll.exe

scroll.exe: scroll.obj modex16.obj dos_kb.obj bitmap.obj
	wcl $(FLAGS) scroll.obj modex16.obj dos_kb.obj bitmap.obj
scroll.obj: scroll.c
	wcl $(FLAGS) -c scroll.c
test.exe: test.obj modex16.obj bitmap.obj
	wcl $(FLAGS) test.obj modex16.obj bitmap.obj
	
test2.exe: test2.obj modex16.obj bitmap.obj
	wcl $(FLAGS) test2.obj modex16.obj bitmap.obj
	
pcxtest.exe: pcxtest.obj modex16.obj bitmap.obj
	wcl $(FLAGS) pcxtest.obj modex16.obj bitmap.obj

test.obj: test.c modex16.h
	wcl $(FLAGS) -c test.c
	
test2.obj: test2.c modex16.h
	wcl $(FLAGS) -c test2.c
	
pcxtest.obj: pcxtest.c modex16.h
	wcl $(FLAGS) -c pcxtest.c

modex16.obj: modex16.h modex16.c
	wcl $(FLAGS) -c modex16.c

dos_kb.obj: dos_kb.h dos_kb.c
	wcl $(FLAGS) -c dos_kb.c

bitmap.obj: bitmap.h bitmap.c
	wcl $(FLAGS) -c bitmap.c
	
clean: 
	del *.obj
	del *.exe
