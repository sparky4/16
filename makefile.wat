#
#	Higanbana Project makefile
#
#
# Possible optimizations for 8088 class processors
#
# -oa   Relax alias checking
# -ob   Try to generate straight line code
# -oe - expand user functions inline (-oe=20 is default, adds lots of code)
# -oh   Enable repeated optimizations
# -oi   generate certain lib funcs inline
# -oi+  Set max inline depth (C++ only, use -oi for C)
# -ok   Flowing of register save into function flow graph
# -ol   loop optimizations
# -ol+  loop optimizations plus unrolling
# -or   Reorder for pipelined (486+ procs); not sure if good to use
# -os   Favor space over time
# -ot   Favor time over space
# -ei   Allocate an "int" for all enum types
# -zp2  Allow compiler to add padding to structs
# -zpw  Use with above; make sure you are warning free!
# -0    8088/8086 class code generation
# -s    disable stack overflow checking

# -zk0u	translate kanji to unicode... wwww
# -zk0	kanji support~
# -zkl		current codepage

# this enables debug output to the serial port.
# comment this out on game release.
# serial output goes to COM1 at 9600 baud 1 stop bit odd parity.
# serial output is plain text ASCII.
DEBUGSERIAL=1

#192x144
#wwww will add these
!ifdef __LINUX__
#!ifdef UNIX
to_os_path=\=/
REMOVECOMMAND=rm -f
COPYCOMMAND=cp -f
DIRSEP=/
OBJ=obj
DUMP=cat
!else		#DOS ^^
to_os_path=/=\
REMOVECOMMAND=*del
COPYCOMMAND=*copy /y
DIRSEP=\
OBJ=obj
DUMP=*type
!endif

TARGET_OS = dos

BUILD_ROOT=$+$(%__CWD__)
#EXMMTESTDIR=16$(DIRSEP)exmmtest$(DIRSEP)
PDIR=..$(DIRSEP)
SRC=src$(DIRSEP)
DATADIR=data$(DIRSEP)
SPRI=$(DATADIR)$(DIRSEP)spri$(DIRSEP)
SRCLIB=$(SRC)lib$(DIRSEP)
JSMNLIB=$(SRCLIB)jsmn$(DIRSEP)
NYANLIB=$(SRCLIB)nyan$(DIRSEP)
#EXMMLIB=$(SRCLIB)exmm$(DIRSEP)
MODEXLIB16=$(SRCLIB)modex16$(DIRSEP)
MODEXLIB=$(SRCLIB)modex$(DIRSEP)
VGMSNDLIB=$(SRCLIB)vgmsnd$(DIRSEP)
DOSLIB=$(SRCLIB)doslib$(DIRSEP)
DOSLIBDIR=$(SRCLIB)doslib
WCPULIB=$(SRCLIB)wcpu$(DIRSEP)

#
# quiet flags
#
WLIBQ=-q
WCLQ=-zq $(WLIBQ)
UPXQ=-qqq

#
# compile flags
#
A_FLAGS=-mh -0 -d1
S_FLAGS=-sg -st -of+ -zu -zdf -zff -zgf -k32768#54096#60000
D_FLAGS=-bt=dos -DTARGET_MSDOS=16 -DMSDOS=1 $(S_FLAGS)
Z_FLAGS=-zk0 -zc -zp8 -zm
L_FLAGS=-lr -l=dos -v
C_FLAGS=$(A_FLAGS) $(I_FLAGS) -wo -i$(DOSLIB) $(L_FLAGS) -fo=.$(OBJ)
O_FLAGS=-obmilr -oe=24 -out -oh -ei -zp8 -fpi87  -onac -ol+ -ok##x
FLAGS=$(C_FLAGS) $(O_FLAGS) $(D_FLAGS) $(Z_FLAGS)

CPPFLAGS=-DTARGET_MSDOS=16
AFLAGS=$(WCLQ) -bt=dos -mh -0 -d1 -fo=.$(OBJ)
CFLAGS=$(WCLQ) -bt=dos -mh -0 -d1 -fo=.$(OBJ) -wo -i"$(DOSLIB)" $(O_FLAGS) $(S_FLAGS) $(Z_FLAGS)
LFLAGS=$(WCLQ) -l=dos
LIBFLAGS=-b $(WLIBQ)

#
# objects
#
VGMSNDOBJ = vgmSnd.$(OBJ) 16_snd.$(OBJ)
DOSLIBOBJ = adlib.$(OBJ) 8254.$(OBJ) 8259.$(OBJ) dos.$(OBJ) cpu.$(OBJ)
16LIBOBJS = 16_in.$(OBJ) 16_mm.$(OBJ) wcpu.$(OBJ) 16_head.$(OBJ) 16_ca.$(OBJ) 16_dbg.$(OBJ) kitten.$(OBJ) 16_hc.$(OBJ) 16_timer.$(OBJ)
GFXLIBOBJS = modex16.$(OBJ) bitmap.$(OBJ) 16text.$(OBJ) bakapee.$(OBJ) scroll16.$(OBJ) 16render.$(OBJ) $(DOSLIBLIBS) 16_vrs.$(OBJ) 16_sprit.$(OBJ)
#planar.$(OBJ) 16planar.$(OBJ)
DOSLIBLIBS=$(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)cpu$(DIRSEP)dos86h$(DIRSEP)cpu.lib $(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)dos$(DIRSEP)dos86h$(DIRSEP)dos.lib $(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)vga$(DIRSEP)dos86h$(DIRSEP)vga.lib

!ifeq DEBUGSERIAL 1
FLAGS += -DDEBUGSERIAL
DOSLIBOBJ += 8250.$(OBJ)
DOSLIBLIBS += $(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)8250$(DIRSEP)dos86h$(DIRSEP)8250.lib
!endif

TESTEXEC = test.exe test0.exe pcxtest.exe pcxtest2.exe palettec.exe maptest.exe fmemtest.exe fonttest.exe fontgfx.exe scroll.exe vgmtest.exe inputest.exe palettel.exe exmmtest.exe vrstest.exe
EXEC = 16.exe bakapi.exe tesuto.exe 0.exe $(TESTEXEC)

all: $(EXEC) joytest.exe
#16.lib => $(16LIBOBJS) bug....
16LIB=$(16LIBOBJS)
#16LIB=16.lib

!ifeq DEBUGSERIAL 1
16LIBOBJS += $(DOSLIBLIBS)
!endif

#
#	Rules for compiling
#
.c.obj:
	*wcl $(CPPFLAGS) $(CFLAGS) $(extra_$^&_obj_opts) -c $[@

.asm.obj:
	*wcl $(AFLAGS) $(extra_$^&_obj_opts) -c $[@

.obj.exe :
	*wcl $(LFLAGS) $(extra_$^&_exe_opts) -fe=$@ $<

.obj.lib :
	*wlib $(LIBFLAGS) $(extra_$^&_lib_opts) $@ $<

.c : $(SRC);$(SRCLIB);$(JSMNLIB);$(NYANLIB);$(VGMSNDLIB);$(WCPULIB)

.asm : $(MODEXLIB)

.lib : .;$(DOSLIBDIR)/hw/cpu/dos86h;$(DOSLIBDIR)/hw/dos/dos86h;$(DOSLIBDIR)/hw/vga/dos86h;$(DOSLIBDIR)/hw/8250/dos86h

#
#game and bakapi executables
#
16.exe: 16.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) $(16LIB) gfx.lib
bakapi.exe: bakapi.$(OBJ) gfx.lib $(DOSLIBLIBS)

#
#Test Executables!
#
scroll.exe: scroll.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) $(16LIB) gfx.lib
scroll.$(OBJ): $(SRC)scroll.c
tesuto.exe: tesuto.$(OBJ) $(DOSLIBLIBS) 16_head.$(OBJ) gfx.lib
tesuto.$(OBJ): $(SRC)tesuto.c
0.exe: 0.$(OBJ) $(DOSLIBLIBS) gfx.lib $(16LIB)
0.$(OBJ): $(SRC)0.c
test.exe: test.$(OBJ) gfx.lib $(DOSLIBLIBS) $(16LIB)
#test2.exe: test2.$(OBJ) $(DOSLIBLIBS) gfx.lib
test0.exe: test0.$(OBJ)
fonttest.exe: fonttest.$(OBJ) $(16LIB) gfx.lib
#fonttes0.exe: fonttes0.$(OBJ) $(16LIB)
fontgfx.exe: fontgfx.$(OBJ) $(16LIB) gfx.lib $(DOSLIBLIBS)
inputest.exe: inputest.$(OBJ) $(16LIB)
#sountest.exe: sountest.$(OBJ) $(16LIB)
pcxtest.exe: pcxtest.$(OBJ) gfx.lib
vrstest.exe: vrstest.$(OBJ) gfx.lib $(16LIB)
palettec.exe: palettec.$(OBJ) gfx.lib #$(16LIB)
palettel.exe: palettel.$(OBJ) gfx.lib #$(16LIB)
pcxtest2.exe: pcxtest2.$(OBJ) gfx.lib
##planrpcx.exe: planrpcx.$(OBJ) gfx.lib
maptest.exe: maptest.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) $(16LIB) gfx.lib
fmemtest.exe: fmemtest.$(OBJ) $(16LIB)
exmmtest.exe: exmmtest.$(OBJ) 16_in.$(OBJ) 16_mm.$(OBJ) wcpu.$(OBJ) 16_head.$(OBJ) 16_ca.$(OBJ) 16_hc.$(OBJ) kitten.$(OBJ)
#	*wcl $(FLAGS) exmmtest.$(OBJ) -fm=exmmtest.mah $(16LIB)
gmtest.exe: vgmtest.$(OBJ) vgmsnd.lib $(16LIB)

#
#executable's objects
#
16.$(OBJ): $(SRC)16.c $(SRC)16.h
bakapi.$(OBJ): $(SRC)bakapi.c $(SRC)bakapi.h
test.$(OBJ): $(SRC)test.c $(SRCLIB)modex16.h
#test2.$(OBJ): $(SRC)test2.c $(SRCLIB)modex16.h
test0.$(OBJ): $(SRC)test0.c
pcxtest.$(OBJ): $(SRC)pcxtest.c $(SRCLIB)modex16.h
vrstest.$(OBJ): $(SRC)vrstest.c $(SRCLIB)modex16.h
##planrpcx.$(OBJ): $(SRC)planrpcx.c $(SRCLIB)modex16.h
pcxtest2.$(OBJ): $(SRC)pcxtest2.c $(SRCLIB)modex16.h
palettec.$(OBJ): $(SRC)palettec.c
palettel.$(OBJ): $(SRC)palettel.c
maptest.$(OBJ): $(SRC)maptest.c $(SRCLIB)modex16.h
#emmtest.$(OBJ): $(SRC)emmtest.c
#emsdump.$(OBJ): $(SRC)emsdump.c
fmemtest.$(OBJ): $(SRC)fmemtest.c
fonttest.$(OBJ): $(SRC)fonttest.c
#fonttes0.$(OBJ): $(SRC)fonttes0.c
fontgfx.$(OBJ): $(SRC)fontgfx.c
inputest.$(OBJ): $(SRC)inputest.c
#sountest.$(OBJ): $(SRC)sountest.c
#miditest.$(OBJ): $(SRC)miditest.c
#testemm.$(OBJ): $(SRC)testemm.c
#testemm0.$(OBJ): $(SRC)testemm0.c
#tsthimem.$(OBJ): $(SRC)tsthimem.c
exmmtest.$(OBJ): $(SRC)exmmtest.c
vgmtest.$(OBJ): $(SRC)vgmtest.c

#
#non executable objects libraries
#
#.C.LIB:
#	*wlib -b $(WLIBQ) $[@

16.lib: $(16LIBOBJS)# doslib.lib vgmsnd.lib
	*wlib -b $(WLIBQ) 16.lib $(16LIBOBJS)# doslib.lib vgmsnd.lib

gfx.lib: $(GFXLIBOBJS)
	*wlib -b $(WLIBQ) gfx.lib $(GFXLIBOBJS)

vgmsnd.lib: $(VGMSNDOBJ)
	*wlib -b $(WLIBQ) vgmsnd.lib $(VGMSNDOBJ)

# extdep:
# !include $(DOSLIBDIR)$(DIRSEP)extdep.mak

# library deps 16-bit huge
$(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)cpu$(DIRSEP)dos86h$(DIRSEP)cpu.lib:
	cd $(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)cpu && .$(DIRSEP)make.sh
	cd $(BUILD_ROOT)
$(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)dos$(DIRSEP)dos86h$(DIRSEP)dos.lib:
	cd $(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)dos && .$(DIRSEP)make.sh
	cd $(BUILD_ROOT)
$(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)vga$(DIRSEP)dos86h$(DIRSEP)vgatty.lib:
	cd $(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)vga && .$(DIRSEP)make.sh
	cd $(BUILD_ROOT)
$(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)vga$(DIRSEP)dos86h$(DIRSEP)vga.lib:
	cd $(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)vga && .$(DIRSEP)make.sh
	cd $(BUILD_ROOT)
$(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)8250$(DIRSEP)dos86h$(DIRSEP)8250.lib:
	cd $(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)8250 && .$(DIRSEP)make.sh
	cd $(BUILD_ROOT)

joytest.exe:
	cd $(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)joystick && .$(DIRSEP)make.sh && $(COPYCOMMAND) dos86h$(DIRSEP)test.exe $(BUILD_ROOT)$(DIRSEP)joytest.exe
	cd $(BUILD_ROOT)
#$(DOSLIBLIBS): .symbolic
#	@cd $(DOSLIB)
#	@.$(DIRSEP)buildall.sh
#	@cd $(BUILD_ROOT)

modex16.$(OBJ): $(SRCLIB)modex16.c $(SRCLIB)modex16.h
bakapee.$(OBJ): $(SRCLIB)bakapee.c $(SRCLIB)bakapee.h
16render.$(OBJ): $(SRCLIB)16render.c $(SRCLIB)16render.h
##16planar.$(OBJ): $(MODEXLIB16)16planar.c $(MODEXLIB16)16planar.h
16_vrs.$(OBJ): $(SRCLIB)16_vrs.c $(SRCLIB)16_vrs.h $(DOSLIBLIBS)
16_sprit.$(OBJ): $(SRCLIB)16_sprit.c $(SRCLIB)16_sprit.h
bitmap.$(OBJ): $(SRCLIB)bitmap.c $(SRCLIB)bitmap.h
##planar.$(OBJ): $(SRCLIB)planar.c $(SRCLIB)planar.h
scroll16.$(OBJ): $(SRCLIB)scroll16.c $(SRCLIB)scroll16.h
16text.$(OBJ): $(SRCLIB)16text.c
mapread.$(OBJ): $(SRCLIB)mapread.c $(SRCLIB)mapread.h
16_timer.$(OBJ): $(SRCLIB)16_timer.c $(SRCLIB)16_timer.h
16_in.$(OBJ): $(SRCLIB)16_in.c $(SRCLIB)16_in.h
16_mm.$(OBJ): $(SRCLIB)16_mm.c $(SRCLIB)16_mm.h
16_ca.$(OBJ): $(SRCLIB)16_ca.c $(SRCLIB)16_ca.h
16_dbg.$(OBJ): $(SRCLIB)16_dbg.c $(SRCLIB)16_dbg.h
midi.$(OBJ): $(SRCLIB)midi.c $(SRCLIB)midi.h
16_head.$(OBJ): $(SRCLIB)16_head.c $(SRCLIB)16_head.h
16_hc.$(OBJ): $(SRCLIB)16_hc.c $(SRCLIB)16_hc.h
16_snd.$(OBJ): $(SRCLIB)16_snd.c $(SRCLIB)16_snd.h
jsmn.$(OBJ): $(JSMNLIB)jsmn.c $(JSMNLIB)jsmn.h
kitten.$(OBJ): $(NYANLIB)kitten.c $(NYANLIB)kitten.h
vgmSnd.$(OBJ): $(VGMSNDLIB)vgmSnd.c $(VGMSNDLIB)vgmSnd.h
wcpu.$(OBJ): $(WCPULIB)wcpu.c $(WCPULIB)wcpu.h
#memory.$(OBJ): $(EXMMLIB)memory.c $(EXMMLIB)memory.h
#	*wcl $(FLAGS) $(MFLAGS) -c $(EXMMLIB)memory.c

c_utils.$(OBJ): $(MODEXLIB)c_utils.asm
modex.$(OBJ): $(MODEXLIB)modex.asm

#
#other~
#
clean: .symbolic
	@$(REMOVECOMMAND) $(EXEC)
	@$(REMOVECOMMAND) *.$(OBJ)
!ifdef __LINUX__
	@rm *.LIB
!endif
	@$(REMOVECOMMAND) 16.lib
	@$(REMOVECOMMAND) gfx.lib
	@$(REMOVECOMMAND) vgmsnd.lib
	@*wlib -n $(WLIBQ) 16.lib
	@*wlib -n $(WLIBQ) gfx.lib
	@*wlib -n $(WLIBQ) vgmsnd.lib
	@$(REMOVECOMMAND) *.16W
	@$(REMOVECOMMAND) *.16B
	@$(REMOVECOMMAND) *.OBJ
	@$(REMOVECOMMAND) *.o
	@$(REMOVECOMMAND) *.BCO
	#@$(REMOVECOMMAND) makefi~1
	#@$(REMOVECOMMAND) makefile~
	@$(REMOVECOMMAND) __wcl__.LNK
#	@$(REMOVECOMMAND) *.smp
	@$(REMOVECOMMAND) *.SMP
	@$(REMOVECOMMAND) *.hed
	@$(REMOVECOMMAND) *.MAH
	@$(REMOVECOMMAND) *.mah
	@$(REMOVECOMMAND) *.err
	@echo $(OBJ)
	#@cd $(DOSLIB)
	#@./buildall.sh clean
	#@cd $(BUILD_ROOT)
#	@$(COPYCOMMAND) $(SRC)exmmtest.c $(EXMMTESTDIR)$(SRC)
#	@$(COPYCOMMAND) $(SRCLIB)16_mm.* $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(SRCLIB)16_head.* $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(SRCLIB)16_ca.* $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(SRCLIB)16_hc.* $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(SRCLIB)types.h $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(NYANLIB)* $(EXMMTESTDIR)$(NYANLIB)
#	@echo $(watcom)
#	@echo $(INCLUDE)

backupconfig: .symbolic
	@$(COPYCOMMAND) .git$(DIRSEP)config git_con.fig
	@$(COPYCOMMAND) .gitmodules git_modu.les
	@$(COPYCOMMAND) .gitignore git_igno.re

comp: .symbolic
	@*upx -9 $(EXEC)

comq: .symbolic
	@*upx -9 $(UPXQ) $(EXEC)

www: .symbolic
	@ssh -p 26 sparky4@4ch.mooo.com 'rm -f /var/www/16/*exe*'
	#@rm -f /var/www/$(EXEC)*
	@rm -f /var/www/*.exe.zip*
	#@cp ./$(EXEC) $(DIRSEP)var$(DIRSEP)www$(DIRSEP)
	@./src/util/z.sh $(EXEC) $(EXEC)
	@scp -r -P 26 *.exe 4ch.mooo.com:/var/www/16/
	@scp -r -P 26 x4get.bat 4ch.mooo.com:/var/www/16/
	@scp -r -P 26 /var/www/*.exe.zip.* 4ch.mooo.com:/var/www/16/

getwww: .symbolic
	*x4get.bat $(EXEC)

vomitchan: .symbolic
	@$(DUMP) *.err

##
##	External library management~ ^^
##
#git submodule add <repo>
mkdl: .symbolic
	@cd $(DOSLIB)
	@./buildall.sh
	@cd $(BUILD_ROOT)

uplibs: .symbolic
	@cd $(JSMNLIB)
	@git pull
	@cd $(BUILD_ROOT)
	@cd $(DOSLIB)
	@git pull
	@cd $(BUILD_ROOT)

reinitlibs: .symbolic
	@rm -rf $(SRCLIB)doslib
	@rm -rf $(SRCLIB)jsmn
	@rm -rf 16/CatacombApocalypse
	@rm -rf 16/wolf3d
	@rm -rf 16/keen
	@wmake -h initlibs

initlibs: .symbolic
	@cp git_con.fig .git/config
	@cp git_modu.les .gitmodules
	@cp git_igno.re .gitignore
	@cd $(SRCLIB)
	@git clone https://github.com/joncampbell123/doslib.git
	@git clone https://github.com/zserge/jsmn.git
	@cd $(BUILD_ROOT)
	@cd 16
	@git clone https://github.com/FlatRockSoft/CatacombApocalypse.git
	@git clone https://github.com/id-Software/wolf3d.git
	@git clone https://github.com/keendreams/keen.git
	@cd $(BUILD_ROOT)
	@cp $(SRCLIB)doslib/make-lowercase .

##
##	experimental libs
##
xlib: .symbolic
	@cd 16$(DIRSEP)xlib
	@wmake -h clean
	@wmake -h all
	@cd $(BUILD_ROOT)

mx: .symbolic
	@cd 16$(DIRSEP)xw
#	@wmake clean
	@wmake -h all
	@cd $(BUILD_ROOT)

mx_: .symbolic
	@cd 16$(DIRSEP)xw_
	@wmake -h -f makefile all
	@cd $(BUILD_ROOT)

vrs: .symbolic
	@cd $(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)vga
	#@make clean
	@make all
	@mv pcx2vrl $(BUILD_ROOT)$(DIRSEP)
	@mv pcxsscut $(BUILD_ROOT)$(DIRSEP)
	@mv vrl2vrs $(BUILD_ROOT)$(DIRSEP)
	@mv vrsdump $(BUILD_ROOT)$(DIRSEP)
	@cd $(BUILD_ROOT)
