FLAGS=-0 -d2 
SRC=src\
SRCLIB=$(SRC)lib\
all: test.exe pcxtest.exe test2.exe scroll.exe

scroll.exe: scroll.obj modex16.obj dos_kb.obj bitmap.obj mapread.obj jsmn.obj lib_head.obj
	wcl $(FLAGS) scroll.obj modex16.obj dos_kb.obj bitmap.obj mapread.obj jsmn.obj lib_head.obj# 16/lib/x/modex.lib
scroll.obj: $(SRC)scroll.c
	wcl $(FLAGS) -c $(SRC)scroll.c
test.exe: test.obj modex16.obj bitmap.obj
	wcl $(FLAGS) test.obj modex16.obj bitmap.obj

test2.exe: test2.obj modex16.obj bitmap.obj planar.obj
	wcl $(FLAGS) test2.obj modex16.obj bitmap.obj planar.obj

pcxtest.exe: pcxtest.obj modex16.obj bitmap.obj
	wcl $(FLAGS) pcxtest.obj modex16.obj bitmap.obj

maptest.exe: maptest.obj mapread.obj jsmn.obj modex16.obj bitmap.obj
	wcl $(FLAGS) maptest.obj jsmn.obj modex16.obj bitmap.obj


test.obj: $(SRC)test.c $(SRCLIB)modex16.h
	wcl $(FLAGS) -c $(SRC)test.c

test2.obj: $(SRC)test2.c $(SRCLIB)modex16.h
	wcl $(FLAGS) -c $(SRC)test2.c

pcxtest.obj: $(SRC)pcxtest.c $(SRCLIB)modex16.h
	wcl $(FLAGS) -c $(SRC)pcxtest.c

maptest.obj: $(SRC)maptest.c $(SRCLIB)mapread.h
	wcl $(FLAGS) -c $(SRC)maptest.c

modex16.obj: $(SRCLIB)modex16.h $(SRCLIB)modex16.c
	wcl $(FLAGS) -c $(SRCLIB)modex16.c

dos_kb.obj: $(SRCLIB)dos_kb.h $(SRCLIB)dos_kb.c
	wcl $(FLAGS) -c $(SRCLIB)dos_kb.c

bitmap.obj: $(SRCLIB)bitmap.h $(SRCLIB)bitmap.c
	wcl $(FLAGS) -c $(SRCLIB)bitmap.c

planar.obj: $(SRCLIB)planar.h $(SRCLIB)planar.c
	wcl $(FLAGS) -c $(SRCLIB)planar.c

mapread.obj: $(SRCLIB)mapread.h $(SRCLIB)mapread.c jsmn.obj lib_head.obj
	wcl $(FLAGS) -c $(SRCLIB)mapread.c

lib_head.obj: $(SRCLIB)lib_head.h $(SRCLIB)lib_head.c
	wcl $(FLAGS) -c $(SRCLIB)lib_head.c

jsmn.obj: $(SRCLIB)jsmn\jsmn.h $(SRCLIB)jsmn\jsmn.c
	wcl $(FLAGS) -c $(SRCLIB)jsmn\jsmn.c

clean:
	del *.obj
#	del *.exe
