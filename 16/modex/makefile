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

all: x_demo.exe

x_demo.exe: x_demo.$(OBJ) modex.$(OBJ) c_utils.$(OBJ)
	wcl $(CFLAGS) x_demo.$(OBJ) modex.$(OBJ) c_utils.$(OBJ)
x_demo.$(OBJ): x_demo.c
	wcl -c $(CFLAGS) x_demo.c

c_utils.$(OBJ): c_utils.asm
	wcl -c $(CFLAGS) c_utils.asm
modex.$(OBJ): modex.asm
	wcl -c $(CFLAGS) modex.asm

clean: .symbolic
	@$(REMOVECOMMAND) x-demo.exe
	@$(REMOVECOMMAND) *.$(OBJ)
	@$(REMOVECOMMAND) *.err
