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

all: x-demo.exe

x-demo.exe: x-demo.$(OBJ) modex.$(OBJ) c_utils.$(OBJ)
	wcl -0 -mh x-demo.$(OBJ) modex.$(OBJ) c_utils.$(OBJ)
x-demo.$(OBJ): x-demo.c
	wcl -c -0 -mh x-demo.c

c_utils.$(OBJ): c_utils.asm
	wcl -0 -c -mh c_utils.asm
modex.$(OBJ): modex.asm
	wcl -c -0 -mh modex.asm

clean: .symbolic
	@$(REMOVECOMMAND) x-demo.exe
	@$(REMOVECOMMAND) *.$(OBJ)
	@$(REMOVECOMMAND) *.err
