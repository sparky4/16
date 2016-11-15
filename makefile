#
#	Higanbana Project makefile
#
#
# Possible optimizations for 8088 class processors
#
# -oa	Relax alias checking
# -ob	Try to generate straight line code
# -oe - expand user functions inline (-oe=20 is default, adds lots of code)
# -oh	Enable repeated optimizations
# -oi	generate certain lib funcs inline
# -oi	+Set max inline depth (C++ only, use -oi for C)
# -ok	Flowing of register save into function flow graph
# -ol	loop optimizations
# -ol+	loop optimizations plus unrolling
# -or	Reorder for pipelined (486+ procs); not sure if good to use
# -os	Favor space over time
# -ot	Favor time over space
# -ei	Allocate an "int" for all enum types
# -zp2	Allow compiler to add padding to structs
# -zpw	Use with above; make sure you are warning free!
# -0	8088/8086 class code generation
# -s	disable stack overflow checking

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
DOSLIBMAKE=./make.sh
DOSLIBMAKEALL=./buildall.sh
!else		#DOS ^^
to_os_path=/=\
REMOVECOMMAND=del
COPYCOMMAND=copy /y
DIRSEP=\
OBJ=obj
DUMP=type
DOSLIBMAKE=.\make.bat
DOSLIBMAKEALL=.\build.bat
!endif

TARGET_OS = dos

BUILD_ROOT=$+$(%__CWD__)$-
#EXMMTESTDIR=16$(DIRSEP)exmmtest$(DIRSEP)
DATADIR=data$(DIRSEP)
SPRI=$(DATADIR)/spri
SRC=src
SRCLIB=src/lib
JSMNLIB=src/lib/jsmn
NYANLIB=src/lib/nyan
EXMMLIB=src/lib/exmm
MODEXLIB16=src/lib/16_vl
MODEXLIB=src/lib/modex
VGMSNDLIB=src/lib/vgmsnd
DOSLIB=src/lib/doslib
WCPULIB=src/lib/wcpu

DOSLIB_CPU=src/lib/doslib/hw/cpu
DOSLIB_DOS=src/lib/doslib/hw/dos
DOSLIB_VGA=src/lib/doslib/hw/vga
DOSLIB_8250=src/lib/doslib/hw/8250
DOSLIB_JOYSTICK=src/lib/doslib/hw/joystick

#
# quiet flags
#
WLIBQ=-q
WCLQ=-q
UPXQ=-qqq

#
# compile flags
#
S_FLAGS=-sg -st -of+ -zu -zdf -zff -zgf -zq -k32768#54096#60000
Z_FLAGS=-zk0 -zc -zp8 -zm
O_FLAGS=-obmilr -oe=24 -out -oh -ei -onac -ol+ -ok##x
T_FLAGS=-bt=dos -wx -mh -0 -fpi87 -fo=.$(OBJ) -d1###### -e=65536

CPPFLAGS=-DTARGET_MSDOS=16 -DMSDOS=1
!ifeq DEBUGSERIAL 1
CPPFLAGS += -DDEBUGSERIAL
!endif
AFLAGS=$(WCLQ) $(T_FLAGS)
CFLAGS=$(WCLQ) $(T_FLAGS) -wo -i"$(DOSLIB)" $(O_FLAGS) $(S_FLAGS) $(Z_FLAGS)
LFLAGS=$(WCLQ) -l=dos -fm=$^&.mah $(S_FLAGS)
LIBFLAGS=$(WLIBQ) -b -n

#
# objects
#
VGMSNDOBJ = vgmSnd.$(OBJ) 16_snd.$(OBJ)
OLDLIBOBJS=bitmap.$(OBJ) mapread.$(OBJ) 16render.$(OBJ)
GFXLIBOBJS = 16_vl.$(OBJ) 16text.$(OBJ) bakapee.$(OBJ) scroll16.$(OBJ) 16_vrs.$(OBJ) 16_sprit.$(OBJ) $(OLDLIBOBJS)
16LIBOBJS = 16_mm.$(OBJ) 16_pm.$(OBJ) 16_ca.$(OBJ) 16_tail.$(OBJ) 16_in.$(OBJ) 16_head.$(OBJ) 16_dbg.$(OBJ) kitten.$(OBJ) 16_hc.$(OBJ) wcpu.$(OBJ) 16_timer.$(OBJ) jsmn.$(OBJ)
####++++ 16_map.$(OBJ)
#16planar.$(OBJ) planar.$(OBJ)
DOSLIBOBJ = adlib.$(OBJ) 8254.$(OBJ) 8259.$(OBJ) dos.$(OBJ) cpu.$(OBJ)
!ifeq DEBUGSERIAL 1
DOSLIBOBJ += 8250.$(OBJ)
!endif

#
# libraries
#
DOSLIBLIBS = $(DOSLIB_CPU)/dos86h/cpu.lib $(DOSLIB_DOS)/dos86h/dos.lib $(DOSLIB_VGA)/dos86h/vga.lib
!ifeq DEBUGSERIAL 1
DOSLIBLIBS += $(DOSLIB_8250)/dos86h/8250.lib
!endif
16LIB=$(16LIBOBJS)#16.lib bad program lock up

#
#	Files locations
#
.c : $(SRC);$(SRCLIB);$(MODEXLIB16);$(JSMNLIB);$(NYANLIB);$(VGMSNDLIB);$(WCPULIB)

.asm : $(MODEXLIB)

.lib : .;$(DOSLIB_CPU)/dos86h;$(DOSLIB_DOS)/dos86h;$(DOSLIB_VGA)/dos86h;$(DOSLIB_8250)/dos86h

.obj : .

#
#	Default make rules
#
.c.obj:
	*wcl $(CPPFLAGS) $(CFLAGS) $(extra_$^&_obj_opts) -c $[@

.asm.obj:
	*wcl $(AFLAGS) $(extra_$^&_obj_opts) -c $[@

#CFLAGS is neccessary here
.obj.exe :
	*wcl $(LFLAGS) $(extra_$^&_exe_opts) -fe=$@ $<

.obj.lib :
	*wlib $(LIBFLAGS) $(extra_$^&_lib_opts) $@ $<

#
# List of executables to build
#
TESTEXEC = &
	tesuto.exe &
	0.exe &
	scroll.exe &
	zcroll.exe &
	exmmtest.exe &
	vrstest.exe
TESTEXEC2 = &
	vgmtest.exe &
	inputest.exe &
	maptest.exe &
	fmemtest.exe &
	fonttest.exe &
	fontgfx.exe &
	test.exe &
	test0.exe &
	pcxtest.exe &
	pcxtest2.exe
UTILEXEC = &
	palettel.exe &
	palettec.exe
EXEC = &
	16.exe &
	bakapi.exe &
	$(UTILEXEC) &
	$(TESTEXEC)

all: $(EXEC) joytest.exe
testexec: $(EXEC) joytest.exe $(TESTEXEC2)

#
# game and bakapi executables
#
16.exe:		16.$(OBJ) $(16LIB) gfx.lib $(DOSLIBLIBS)
bakapi.exe:		bakapi.$(OBJ) gfx.lib $(DOSLIBLIBS)

#
# Test Executables!
#
scroll.exe:	scroll.$(OBJ) $(16LIB) gfx.lib $(DOSLIBLIBS)
scroll.$(OBJ):	$(SRC)/scroll.c
zcroll.exe:	zcroll.$(OBJ) $(16LIB) gfx.lib $(DOSLIBLIBS)
zcroll.$(OBJ):	$(SRC)/zcroll.c
tesuto.exe:	tesuto.$(OBJ) 16_head.$(OBJ) gfx.lib $(DOSLIBLIBS)
tesuto.$(OBJ):	$(SRC)/tesuto.c
0.exe:			0.$(OBJ) $(16LIB) gfx.lib $(DOSLIBLIBS)
0.$(OBJ):		 $(SRC)/0.c
test.exe:		 test.$(OBJ) $(16LIB) gfx.lib $(DOSLIBLIBS)
#test2.exe:	test2.$(OBJ) gfx.lib $(DOSLIBLIBS)
test0.exe:		test0.$(OBJ)
fonttest.exe:	 fonttest.$(OBJ) gfx.lib
#fonttes0.exe:	fonttes0.$(OBJ) $(16LIB)
fontgfx.exe:	fontgfx.$(OBJ) gfx.lib $(DOSLIBLIBS)
inputest.exe:	 inputest.$(OBJ) $(16LIB) $(DOSLIBLIBS)
#sountest.exe:	sountest.$(OBJ) $(16LIB)
pcxtest.exe:	pcxtest.$(OBJ) gfx.lib $(DOSLIBLIBS)
vrstest.exe:	vrstest.$(OBJ) $(16LIB) gfx.lib $(DOSLIBLIBS)
#vgacamm.exe:	vgacamm.$(OBJ) $(16LIB) gfx.lib $(DOSLIBLIBS)
palettec.exe:	 palettec.$(OBJ) gfx.lib $(DOSLIBLIBS)
palettel.exe:	 palettel.$(OBJ) gfx.lib $(DOSLIBLIBS)
pcxtest2.exe:	 pcxtest2.$(OBJ) gfx.lib $(DOSLIBLIBS)
#planrpcx.exe:	planrpcx.$(OBJ) gfx.lib
maptest.exe:	maptest.$(OBJ) 16_map.$(OBJ) 16_head.$(OBJ) gfx.lib $(DOSLIBLIBS)
fmemtest.exe:	 fmemtest.$(OBJ)
exmmtest.exe:	 exmmtest.$(OBJ) $(16LIB) $(DOSLIBLIBS)
vgmtest.exe:	vgmtest.$(OBJ) vgmsnd.lib $(16LIB) $(DOSLIBLIBS)

#
# executable's objects
#
16.$(OBJ):		$(SRC)/16.c $(SRC)/16.h
bakapi.$(OBJ):	$(SRC)/bakapi.c $(SRC)/bakapi.h
test.$(OBJ):	$(SRC)/test.c $(SRCLIB)/16_vl.h
#test2.$(OBJ):	$(SRC)/test2.c $(SRCLIB)/16_vl.h
test0.$(OBJ):	 $(SRC)/test0.c
pcxtest.$(OBJ):$(SRC)/pcxtest.c $(SRCLIB)/16_vl.h
vrstest.$(OBJ):$(SRC)/vrstest.c $(SRCLIB)/16_vl.h
#vgacamm.$(OBJ):$(SRC)/vgacamm.c $(SRCLIB)/16_vl.h
#planrpcx.$(OBJ): $(SRC)/planrpcx.c $(SRCLIB)/16_vl.h
pcxtest2.$(OBJ):$(SRC)/pcxtest2.c $(SRCLIB)/16_vl.h
palettec.$(OBJ):$(SRC)/palettec.c
palettel.$(OBJ):$(SRC)/palettel.c
maptest.$(OBJ):$(SRC)/maptest.c $(SRCLIB)/16_vl.h
#emmtest.$(OBJ):$(SRC)/emmtest.c
#emsdump.$(OBJ):$(SRC)/emsdump.c
fmemtest.$(OBJ):$(SRC)/fmemtest.c
fonttest.$(OBJ):$(SRC)/fonttest.c
#fonttes0.$(OBJ): $(SRC)/fonttes0.c
fontgfx.$(OBJ):$(SRC)/fontgfx.c
inputest.$(OBJ):$(SRC)/inputest.c
#sountest.$(OBJ): $(SRC)/sountest.c
#miditest.$(OBJ): $(SRC)/miditest.c
#testemm.$(OBJ):$(SRC)/testemm.c
#testemm0.$(OBJ): $(SRC)/testemm0.c
#tsthimem.$(OBJ): $(SRC)/tsthimem.c
exmmtest.$(OBJ):$(SRC)/exmmtest.c
vgmtest.$(OBJ):$(SRC)/vgmtest.c

#
# non executable objects libraries
#
16.lib: $(16LIBOBJS)
vgmsnd.lib: $(VGMSNDOBJ)

gfx.lib: $(GFXLIBOBJS)
	*wlib $(LIBFLAGS) $(extra_$^&_lib_opts) $@ $<

#
#	doslib
#
# library deps 16-bit huge
$(DOSLIB_CPU)/dos86h/cpu.lib:
	cd $(DOSLIB_CPU:$(to_os_path)) && $(DOSLIBMAKE) && cd $(BUILD_ROOT)
$(DOSLIB_DOS)/dos86h/dos.lib:
	cd $(DOSLIB_DOS:$(to_os_path)) && $(DOSLIBMAKE) && cd $(BUILD_ROOT)
$(DOSLIB_VGA)/dos86h/vgatty.lib:
	cd $(DOSLIB_VGA:$(to_os_path)) && $(DOSLIBMAKE) && cd $(BUILD_ROOT)
$(DOSLIB_VGA)/dos86h/vga.lib:
	cd $(DOSLIB_VGA:$(to_os_path)) && $(DOSLIBMAKE) && cd $(BUILD_ROOT)
$(DOSLIB_8250)/dos86h/8250.lib:
	cd $(DOSLIB_8250:$(to_os_path)) && $(DOSLIBMAKE) && cd $(BUILD_ROOT)

joytest.exe:
	cd $(DOSLIB_JOYSTICK:$(to_os_path)) && $(DOSLIBMAKE) && cd $(BUILD_ROOT)
	$(COPYCOMMAND) $(DOSLIB_JOYSTICK:$(to_os_path))$(DIRSEP)dos86h$(DIRSEP)test.exe joytest.exe

16_vl.$(OBJ):$(SRCLIB)/16_vl.c $(SRCLIB)/16_vl.h
bakapee.$(OBJ):$(SRCLIB)/bakapee.c $(SRCLIB)/bakapee.h
16render.$(OBJ):$(SRCLIB)/16render.c $(SRCLIB)/16render.h
16planar.$(OBJ):$(MODEXLIB16)/16planar.c $(MODEXLIB16)/16planar.h
16_vrs.$(OBJ):	$(SRCLIB)/16_vrs.c $(SRCLIB)/16_vrs.h $(DOSLIBLIBS)
16_sprit.$(OBJ):$(SRCLIB)/16_sprit.c $(SRCLIB)/16_sprit.h
bitmap.$(OBJ):	$(SRCLIB)/bitmap.c $(SRCLIB)/bitmap.h
planar.$(OBJ):	$(SRCLIB)/planar.c $(SRCLIB)/planar.h
scroll16.$(OBJ):$(SRCLIB)/scroll16.c $(SRCLIB)/scroll16.h
16text.$(OBJ):	$(SRCLIB)/16text.c
mapread.$(OBJ):$(SRCLIB)/mapread.c $(SRCLIB)/mapread.h
16_map.$(OBJ):$(SRCLIB)/16_map.c $(SRCLIB)/16_map.h
16_timer.$(OBJ):$(SRCLIB)/16_timer.c $(SRCLIB)/16_timer.h
16_in.$(OBJ):	 $(SRCLIB)/16_in.c $(SRCLIB)/16_in.h
16_mm.$(OBJ):	 $(SRCLIB)/16_mm.c $(SRCLIB)/16_mm.h
16_pm.$(OBJ):	 $(SRCLIB)/16_pm.c $(SRCLIB)/16_pm.h
16_ca.$(OBJ):	 $(SRCLIB)/16_ca.c $(SRCLIB)/16_ca.h
16_dbg.$(OBJ):	$(SRCLIB)/16_dbg.c $(SRCLIB)/16_dbg.h
midi.$(OBJ):	$(SRCLIB)/midi.c $(SRCLIB)/midi.h
16_head.$(OBJ):$(SRCLIB)/16_head.c $(SRCLIB)/16_head.h
16_tail.$(OBJ):$(SRCLIB)/16_tail.c $(SRCLIB)/16_tail.h
16_hc.$(OBJ):	 $(SRCLIB)/16_hc.c $(SRCLIB)/16_hc.h
16_snd.$(OBJ):	$(SRCLIB)/16_snd.c $(SRCLIB)/16_snd.h
jsmn.$(OBJ):	$(JSMNLIB)/jsmn.c $(JSMNLIB)/jsmn.h
kitten.$(OBJ):	$(NYANLIB)/kitten.c $(NYANLIB)/kitten.h
vgmSnd.$(OBJ):	$(VGMSNDLIB)/vgmSnd.c $(VGMSNDLIB)/vgmSnd.h
wcpu.$(OBJ):	$(WCPULIB)/wcpu.c $(WCPULIB)/wcpu.h
#memory.$(OBJ):$(EXMMLIB)/memory.c $(EXMMLIB)/memory.h
c_utils.$(OBJ):$(MODEXLIB)/c_utils.asm
modex.$(OBJ):	 $(MODEXLIB)/modex.asm

#
#other~
#
clean: .symbolic
	@for %f in ($(EXEC)) do @if exist %f $(REMOVECOMMAND) %f
!ifdef __LINUX__
	@rm *.LIB
	@. src/util/bcexmm.sh
	@rm *.EXE
!endif
	@if exist *.obj $(REMOVECOMMAND) *.obj
	@if exist *.OBJ $(REMOVECOMMAND) *.OBJ
	@if exist *.bco $(REMOVECOMMAND) *.bco
	@if exist *.BCO $(REMOVECOMMAND) *.BCO
	@if exist *.LIB $(REMOVECOMMAND) *.LIB
	@if exist *.lnk $(REMOVECOMMAND) *.lnk
	@if exist *.LNK $(REMOVECOMMAND) *.LNK
	@if exist *.smp $(REMOVECOMMAND) *.smp
	@if exist *.SMP $(REMOVECOMMAND) *.SMP
	@if exist *.hed $(REMOVECOMMAND) *.hed
	@if exist *.mah $(REMOVECOMMAND) *.mah
	@if exist *.MAH $(REMOVECOMMAND) *.MAH
	@if exist *.err $(REMOVECOMMAND) *.err
#	@if exist *.16W $(REMOVECOMMAND) *.16W
#	@if exist *.16B $(REMOVECOMMAND) *.16B

#	@$(COPYCOMMAND) $(SRC)exmmtest.c $(EXMMTESTDIR)$(SRC)
#	@$(COPYCOMMAND) $(SRCLIB)16_mm.* $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(SRCLIB)16_head.* $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(SRCLIB)16_ca.* $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(SRCLIB)16_hc.* $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(SRCLIB)types.h $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(NYANLIB)* $(EXMMTESTDIR)$(NYANLIB)

nuke: .symbolic
	@wmake clean
	@wmake cldl
	@wmake all
	@wmake comp

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
	@if exist *.err $(DUMP) *.err

##
##	External library management~ ^^
##
#git submodule add <repo>
mkdl: .symbolic
	@cd $(DOSLIB:$(to_os_path))
	@$(DOSLIBMAKEALL)
	@cd $(BUILD_ROOT)

cldl: .symbolic
	@cd $(DOSLIB:$(to_os_path))
	@$(DOSLIBMAKEALL) clean
	@cd $(BUILD_ROOT)

uplibs: .symbolic
	@cd $(JSMNLIB:$(to_os_path))
	@git pull
	@cd $(BUILD_ROOT)
	@cd $(DOSLIB:$(to_os_path))
	@git pull
	@cd $(BUILD_ROOT)

reinitlibs: .symbolic
	@rm -rf $(DOSLIB)
	@rm -rf $(JSMNLIB)
	@rm -rf 16/CatacombApocalypse
	@rm -rf 16/wolf3d
	@rm -rf 16/keen
	@rm -rf 16/Catacomb3D
	@wmake -h initlibs

initlibs: .symbolic
	@cp git_con.fig .git/config
	@cp git_modu.les .gitmodules
	@cp git_igno.re .gitignore
	@cd $(SRCLIB:$(to_os_path))
	@git clone https://github.com/joncampbell123/doslib.git
	@git clone https://github.com/zserge/jsmn.git
	@cd $(BUILD_ROOT)
	@cd 16
	@git clone https://github.com/FlatRockSoft/CatacombApocalypse.git
	@git clone https://github.com/id-Software/wolf3d.git
	@git clone https://github.com/keendreams/keen.git
	@git clone https://github.com/FlatRockSoft/Catacomb3D.git
	@cd $(BUILD_ROOT)
	@cp $(DOSLIB)/make-lowercase .

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
	@cd $(DOSLIB_VGA:$(to_os_path))
	#@make clean
	@make all
	@mv pcx2vrl $(BUILD_ROOT)$(DIRSEP)
	@mv pcxsscut $(BUILD_ROOT)$(DIRSEP)
	@mv vrl2vrs $(BUILD_ROOT)$(DIRSEP)
	@mv vrsdump $(BUILD_ROOT)$(DIRSEP)
	@cd $(BUILD_ROOT)
