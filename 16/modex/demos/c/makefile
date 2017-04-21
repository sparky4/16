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

CFLAGS=-d2 -0 -mc

all: x-demo.exe

x-demo.exe: x-demo.$(OBJ) modex.$(OBJ) c_utils.$(OBJ)
	wcl $(CFLAGS) x-demo.$(OBJ) modex.$(OBJ) c_utils.$(OBJ)
x-demo.$(OBJ): x-demo.c
	wcl -c $(CFLAGS) x-demo.c

c_utils.$(OBJ): c_utils.asm
	wcl -c $(CFLAGS) c_utils.asm
modex.$(OBJ): modex.asm
	wcl -c $(CFLAGS) modex.asm

clean: .symbolic
	@$(REMOVECOMMAND) x-demo.exe
	@$(REMOVECOMMAND) *.$(OBJ)
	@$(REMOVECOMMAND) *.err
