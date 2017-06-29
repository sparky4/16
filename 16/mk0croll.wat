OBJ=obj
FLAGS=-0 -i"src/lib/doslib" -fo=.$(OBJ)

all: 0croll.exe

0croll.exe: 0croll.$(OBJ) omodex16.$(OBJ) bitmap.$(OBJ) 16_in.$(OBJ)
	wcl $(FLAGS) 0croll.$(OBJ) omodex16.$(OBJ) bitmap.$(OBJ) 16_in.$(OBJ)
0croll.$(OBJ): src/0croll.c
	wcl $(FLAGS) -c src/0croll.c

16_in.$(OBJ): src/lib/16_in.h src/lib/16_in.c
	wcl $(FLAGS) -c src/lib/16_in.c

omodex16.$(OBJ): 16/src/lib/omodex16.h 16/src/lib/omodex16.c
	wcl $(FLAGS) -c 16/src/lib/omodex16.c

bitmap.$(OBJ): 16/src/lib/bitmap.h 16/src/lib/bitmap.c
	wcl $(FLAGS) -c 16/src/lib/bitmap.c

clean:
	rm *.$(OBJ)
	rm *.exe
