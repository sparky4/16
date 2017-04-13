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
# -zd{f,p} DS floats vs DS pegged to DGROUP
# -zu SS != DGROUP
# -zp{1,2,4,8,16} struct packing align.
# -ei force enums to be type int
# -wo diagnose problems in overlaid code

# this enables debug output to the serial port.
# comment this out on game release.
# serial output goes to COM1 at 9600 baud 1 stop bit odd parity.
# serial output is plain text ASCII.
DEBUGSERIAL=1

DELLOGFILE=1

#192x144
#wwww will add these
!ifdef __LINUX__
#!ifdef UNIX
to_os_path=\=/
REMOVECOMMAND=rm -f
COPYCOMMAND=cp -f
MOVECOMMAND=mv
DIRSEP=/
OBJ=obj
DUMP=cat
DOSLIBMAKE=./make.sh build all
DOSLIBMAKEALL=./buildall.sh build all
!else		#DOS ^^
to_os_path=/=\
REMOVECOMMAND=del
COPYCOMMAND=copy /y
MOVECOMMAND=move
DIRSEP=\
OBJ=obj
DUMP=type
DOSLIBMAKE=.\make.bat
DOSLIBMAKEALL=.\build.bat
!endif

TARGET_OS = dos
MEMORYMODE = l

BUILD_ROOT=$+$(%__CWD__)$-
#EXMMTESTDIR=16$(DIRSEP)exmmtest$(DIRSEP)
DATADIR=data$(DIRSEP)
SPRI=$(DATADIR)/spri
SRC=src
UTIL=$(SRC)/util
GITCONFIGDIR=$(UTIL)/git
SCRIPTBATDIR=$(UTIL)/shbat
SRCLIB=$(SRC)/lib
JSMNLIB=$(SRCLIB)/jsmn
NYANLIB=$(SRCLIB)/nyan
EXMMLIB=$(SRCLIB)/exmm
MODEXLIB16=$(SRCLIB)/16_vl
MODEXLIB=$(SRCLIB)/modex
VGMSNDLIB=$(SRCLIB)/vgmsnd
DOSLIBDIR=$(SRCLIB)/doslib
WCPULIB=$(SRCLIB)/wcpu

DOSLIB_CPU=$(DOSLIBDIR)/hw/cpu
DOSLIB_DOS=$(DOSLIBDIR)/hw/dos
DOSLIB_VGA=$(DOSLIBDIR)/hw/vga
DOSLIB_8250=$(DOSLIBDIR)/hw/8250
DOSLIB_8254=$(DOSLIBDIR)/hw/8254
DOSLIB_8259=$(DOSLIBDIR)/hw/8259
DOSLIB_ADLIB=$(DOSLIBDIR)/hw/adlib
DOSLIB_JOYSTICK=$(DOSLIBDIR)/hw/joystick
DOSLIB_MEMMODE=dos86$(MEMORYMODE)

# remote host (for sparky4)
HOSTUSER=sparky4
HOSTADDR=4ch.mooo.com
HOSTDIR=/var/www/
#ssh port
HOSTPORT=26
# 2nd remote host (for sparky4)
HOST2USER=wwc001
HOST2ADDR=beta.latech.edu
HOST2DIR=~/public_html/
#ssh port
HOST2PORT=22

#
# quiet flags
#
WLIBQ=-q
WCLQ=-zq
UPXQ=-qqq

#
# compile flags
#
S_FLAGS=-sg -st -of+ -zu -zdf -zff -zgf -k32768#24576
Z_FLAGS=-zk0 -zc -zm#### -zp4 -ei
O_FLAGS=-opnr -oe=24 -oil+ -outback -ohm
T_FLAGS=-bt=dos -wx -m$(MEMORYMODE) -0 -fpi87 -d1 -fo=.$(OBJ)## -e=65536

DBUGFLAGS=-fm=$^&.meh -fd=$^&
CPPFLAGS=-DTARGET_MSDOS=16 -DMSDOS=1
!ifeq DEBUGSERIAL 1
CPPFLAGS += -DDEBUGSERIAL
!endif
AFLAGS=$(WCLQ) $(T_FLAGS)
CFLAGS=$(WCLQ) $(T_FLAGS) -wo -i"$(DOSLIBDIR)" $(O_FLAGS) $(S_FLAGS) $(Z_FLAGS)
LFLAGS=$(WCLQ) -l=dos $(S_FLAGS)		$(DBUGFLAGS)
LIBFLAGS=$(WLIBQ) -b -n

#
# objects
#
VGMSNDOBJ = vgmSnd.$(OBJ)
#OLDLIBOBJS=bitmap.$(OBJ) 16render.$(OBJ)
GFXLIBOBJS = 16_vl.$(OBJ) 16_vl_1.$(OBJ) 16_vl_2.$(OBJ) 16_vlpal.$(OBJ) 16text.$(OBJ) bakapee.$(OBJ) scroll16.$(OBJ) 16_vrs.$(OBJ) 16_spri.$(OBJ) $(OLDLIBOBJS)
16LIBOBJS = 16_mm.$(OBJ) 16_pm.$(OBJ) 16_ca.$(OBJ) 16_tail.$(OBJ) 16_head.$(OBJ) 16_enti.$(OBJ) 16_dbg.$(OBJ) 16_in.$(OBJ) kitten.$(OBJ) 16_hc.$(OBJ) 16_wcpu.$(OBJ) 16_timer.$(OBJ) jsmn.$(OBJ) 16_map.$(OBJ) 16text.$(OBJ) 16_sd.$(OBJ) 16_tail_.$(OBJ) 16_dbg_1.$(OBJ)
DOSLIBOBJ = adlib.$(OBJ) 8254.$(OBJ) 8259.$(OBJ) dos.$(OBJ) cpu.$(OBJ)
!ifeq DEBUGSERIAL 1
DOSLIBOBJ += 8250.$(OBJ)
!endif

#
# libraries
#
DOSLIBLIBS = $(DOSLIB_CPU)/$(DOSLIB_MEMMODE)/cpu.lib $(DOSLIB_DOS)/$(DOSLIB_MEMMODE)/dos.lib $(DOSLIB_VGA)/$(DOSLIB_MEMMODE)/vga.lib $(DOSLIB_8254)/$(DOSLIB_MEMMODE)/8254.lib $(DOSLIB_8259)/$(DOSLIB_MEMMODE)/8259.lib $(DOSLIB_ADLIB)/$(DOSLIB_MEMMODE)/adlib.lib
!ifeq DEBUGSERIAL 1
DOSLIBLIBS += $(DOSLIB_8250)/$(DOSLIB_MEMMODE)/8250.lib
!endif
DOSLIB=doslib.lib
16LIB=16.lib

#
#	Files locations
#
.c : $(SRC);$(SRCLIB);$(MODEXLIB16);$(JSMNLIB);$(NYANLIB);$(VGMSNDLIB);$(WCPULIB);$(UTIL)

.asm : $(MODEXLIB);$(UTIL)

.lib : .;$(DOSLIB_CPU)/$(DOSLIB_MEMMODE);$(DOSLIB_DOS)/$(DOSLIB_MEMMODE);$(DOSLIB_VGA)/$(DOSLIB_MEMMODE);$(DOSLIB_8250)/$(DOSLIB_MEMMODE);$(DOSLIB_8254)/$(DOSLIB_MEMMODE);$(DOSLIB_ADLIB)/$(DOSLIB_MEMMODE)

.$(OBJ) : .

#
#	Default make rules
#
.c.$(OBJ):
	*wcl $(CFLAGS)   $(extra_$^&_obj_opts)		$(CPPFLAGS) -c $[@

.asm.$(OBJ):
	*wcl $(AFLAGS)   $(extra_$^&_obj_opts)		-c $[@

#CFLAGS is neccessary here
.$(OBJ).exe :
	*wcl $(LFLAGS) $(extra_$^&_exe_opts)$<		-fe=$@

LIBMAKERULE=*wlib $(LIBFLAGS) $(extra_$^&_lib_opts)$@ $<
.$(OBJ).lib :
	$(LIBMAKERULE)

#
# List of executables to build
#
EXTERNTESTEXEC = &
	joytest.exe &
	wcpu.exe &
	db.exe
TESTEXEC = &
	vidtest.exe &
	exmmtest.exe &
	vgmtest.exe &
	sountest.exe &
	xcroll.exe &
	inputest.exe &
	vrstest.exe &
	tesuto.exe &
	0.exe &
	maptest.exe &
	imfplay.exe
#zcroll.exe &
TESTEXEC2 = &
	pcxtest.exe &
	scroll.exe &
	fmemtest.exe &
	fonttest.exe &
	fontgfx.exe &
	test0.exe &
	$(EXTERNTESTEXEC)
TESTEXEC3= &
	pcxtest2.exe
UTILEXEC = &
	palettel.exe &
	palllist.exe &
	palbomb.exe &
	palettec.exe
!ifdef __LINUX__
SPRIUTILEXEC = &
	pcx2vrl &
	pcxsscut &
	vrl2vrs &
	vrsdump
#UTILEXEC += $(SPRIUTILEXEC)
!endif

EXEC = &
	16.exe &
	bakapi.exe &
	$(TESTEXEC) &
	$(UTILEXEC) &
	opltest.exe

#!ifdef __LINUX__
#EXEC += $(SPRIUTILEXEC)
#!endif

ALLEXEC = &
	$(EXEC) &
	$(UTILEXEC) &
	$(TESTEXEC2) &
	$(TESTEXEC3)

all: $(EXEC)
testexec: $(EXEC) $(TESTEXEC2)

#
# game and bakapi executables
#
16.exe:		16.$(OBJ) $(16LIB) gfx.lib $(DOSLIB)
bakapi.exe:		bakapi.$(OBJ) 16_vl.$(OBJ) 16_vl_1.$(OBJ) 16text.$(OBJ) bakapee.$(OBJ) $(DOSLIB) 16_wcpu.$(OBJ)# gfx.lib

#
# Test Executables!
#
scroll.exe:	scroll.$(OBJ) $(16LIB) gfx.lib $(DOSLIB)
scroll.$(OBJ):	$(SRC)/scroll.c
xcroll.exe:	xcroll.$(OBJ) $(16LIB) gfx.lib $(DOSLIB)
xcroll.$(OBJ):	$(SRC)/xcroll.c
zcroll.exe:	zcroll.$(OBJ) $(16LIB) gfx.lib $(DOSLIB)
zcroll.$(OBJ):	$(SRC)/zcroll.c
tesuto.exe:	tesuto.$(OBJ) 16_head.$(OBJ) gfx.lib $(DOSLIB)
tesuto.$(OBJ):	$(SRC)/tesuto.c
0.exe:			0.$(OBJ) $(16LIB) gfx.lib $(DOSLIB)
0.$(OBJ):		 $(SRC)/0.c
vidtest.exe:		 vidtest.$(OBJ) $(16LIB) gfx.lib $(DOSLIB)
#test2.exe:	test2.$(OBJ) gfx.lib $(DOSLIB)
test0.exe:		test0.$(OBJ)
fonttest.exe:	 fonttest.$(OBJ) gfx.lib
#fonttes0.exe:	fonttes0.$(OBJ) $(16LIB)
fontgfx.exe:	fontgfx.$(OBJ) gfx.lib $(DOSLIB)
inputest.exe:	 inputest.$(OBJ) $(16LIB) $(DOSLIB) gfx.lib
#inntest.exe:	 	inntest.$(OBJ)	$(16LIBNOINOBJS) 16_in_1.$(OBJ) $(DOSLIB) gfx.lib
sountest.exe:	sountest.$(OBJ) $(16LIB) $(DOSLIB) gfx.lib
imfplay.exe:		imfplay.$(OBJ) $(16LIB) $(DOSLIB) gfx.lib
pcxtest.exe:	pcxtest.$(OBJ) gfx.lib $(DOSLIB) $(16LIB)
vrstest.exe:	vrstest.$(OBJ) $(16LIB) gfx.lib $(DOSLIB)
#vgacamm.exe:	vgacamm.$(OBJ) $(16LIB) gfx.lib $(DOSLIB)
palettec.exe:	palettec.$(OBJ) $(16LIB) gfx.lib $(DOSLIB)
palettel.exe:	 	palettel.$(OBJ) $(16LIB) gfx.lib $(DOSLIB)
palbomb.exe: 	palbomb.$(OBJ) $(16LIB) gfx.lib $(DOSLIB)
palllist.exe:	 	palllist.$(OBJ) $(16LIB) gfx.lib $(DOSLIB) ll.$(OBJ)
pcxtest2.exe:	 pcxtest2.$(OBJ) gfx.lib $(DOSLIB)
#planrpcx.exe:	planrpcx.$(OBJ) gfx.lib
maptest.exe:	maptest.$(OBJ) 16_map.$(OBJ) 16_head.$(OBJ) gfx.lib $(DOSLIB) $(16LIB)
fmemtest.exe:	 fmemtest.$(OBJ)
exmmtest.exe:	 exmmtest.$(OBJ) 16_pm.$(OBJ) 16_head.$(OBJ) 16_ca.$(OBJ) 16_mm.$(OBJ) 16_hc.$(OBJ) kitten.$(OBJ) 16_vl_1.$(OBJ) 16_tail.$(OBJ) 16_in.$(OBJ) 16_dbg.$(OBJ)
#$(16LIB) gfx.lib $(DOSLIB)
vgmtest.exe:	vgmtest.$(OBJ) vgmsnd.lib $(16LIB) $(DOSLIB) gfx.lib
db.exe:		db.$(OBJ)
ps.exe:		ps.$(OBJ) 16_head.$(OBJ)
wcpu.exe:		wcpu.$(OBJ) $(16LIB) $(DOSLIB)

#
# executable's objects
#
16.$(OBJ):		$(SRC)/16.c $(SRC)/16.h
bakapi.$(OBJ):	$(SRC)/bakapi.c $(SRC)/bakapi.h
vidtest.$(OBJ):	$(SRC)/vidtest.c $(SRCLIB)/16_vl.h
#test2.$(OBJ):	$(SRC)/test2.c $(SRCLIB)/16_vl.h
test0.$(OBJ):	 $(SRC)/test0.c
pcxtest.$(OBJ):$(SRC)/pcxtest.c $(SRCLIB)/16_vl.h
vrstest.$(OBJ):$(SRC)/vrstest.c $(SRCLIB)/16_vl.h
#vgacamm.$(OBJ):$(SRC)/vgacamm.c $(SRCLIB)/16_vl.h
#planrpcx.$(OBJ): $(SRC)/planrpcx.c $(SRCLIB)/16_vl.h
pcxtest2.$(OBJ):$(SRC)/pcxtest2.c $(SRCLIB)/16_vl.h
palettec.$(OBJ):	$(SRC)/palettec.c
palettel.$(OBJ):	$(SRC)/palettel.c
palbomb.$(OBJ):	$(SRC)/palbomb.c
palllist.$(OBJ):	$(SRC)/palllist.c
maptest.$(OBJ):$(SRC)/maptest.c $(SRCLIB)/16_vl.h
#emmtest.$(OBJ):$(SRC)/emmtest.c
#emsdump.$(OBJ):$(SRC)/emsdump.c
fmemtest.$(OBJ):$(SRC)/fmemtest.c
fonttest.$(OBJ):$(SRC)/fonttest.c
#fonttes0.$(OBJ): $(SRC)/fonttes0.c
fontgfx.$(OBJ):$(SRC)/fontgfx.c
inputest.$(OBJ):$(SRC)/inputest.c
#inntest.$(OBJ):$(SRC)/inntest.c
sountest.$(OBJ): $(SRC)/sountest.c
imfplay.$(OBJ): $(SRC)/imfplay.c
#miditest.$(OBJ): $(SRC)/miditest.c
#testemm.$(OBJ):$(SRC)/testemm.c
#testemm0.$(OBJ): $(SRC)/testemm0.c
#tsthimem.$(OBJ): $(SRC)/tsthimem.c
exmmtest.$(OBJ):$(SRC)/exmmtest.c
vgmtest.$(OBJ):$(SRC)/vgmtest.c
db.$(OBJ):		$(UTIL)/db.c
ps.$(OBJ):		$(SRC)/ps.c

#
# non executable objects libraries
#
16.lib: $(16LIBOBJS)
vgmsnd.lib: $(VGMSNDOBJ)
gfx.lib: $(GFXLIBOBJS)
	$(LIBMAKERULE)
doslib.lib: $(DOSLIBLIBS)
	$(LIBMAKERULE)

#
#	doslib
#
# library deps 16-bit large
$(DOSLIB_CPU)/$(DOSLIB_MEMMODE)/cpu.lib:
	cd $(DOSLIB_CPU:$(to_os_path)) && $(DOSLIBMAKE) $(DOSLIB_MEMMODE) && cd $(BUILD_ROOT)
$(DOSLIB_DOS)/$(DOSLIB_MEMMODE)/dos.lib:
	cd $(DOSLIB_DOS:$(to_os_path)) && $(DOSLIBMAKE) $(DOSLIB_MEMMODE) && cd $(BUILD_ROOT)
$(DOSLIB_VGA)/$(DOSLIB_MEMMODE)/vgatty.lib:
	cd $(DOSLIB_VGA:$(to_os_path)) && $(DOSLIBMAKE) $(DOSLIB_MEMMODE) && cd $(BUILD_ROOT)
$(DOSLIB_VGA)/$(DOSLIB_MEMMODE)/vga.lib:
	cd $(DOSLIB_VGA:$(to_os_path)) && $(DOSLIBMAKE) $(DOSLIB_MEMMODE) && cd $(BUILD_ROOT)
$(DOSLIB_8250)/$(DOSLIB_MEMMODE)/8250.lib:
	cd $(DOSLIB_8250:$(to_os_path)) && $(DOSLIBMAKE) $(DOSLIB_MEMMODE) && cd $(BUILD_ROOT)
$(DOSLIB_8254)/$(DOSLIB_MEMMODE)/8254.lib:
	cd $(DOSLIB_8254:$(to_os_path)) && $(DOSLIBMAKE) $(DOSLIB_MEMMODE) && cd $(BUILD_ROOT)
$(DOSLIB_8259)/$(DOSLIB_MEMMODE)/8259.lib:
	cd $(DOSLIB_8259:$(to_os_path)) && $(DOSLIBMAKE) $(DOSLIB_MEMMODE) && cd $(BUILD_ROOT)
$(DOSLIB_ADLIB)/$(DOSLIB_MEMMODE)/adlib.lib:
	cd $(DOSLIB_ADLIB:$(to_os_path)) && $(DOSLIBMAKE) $(DOSLIB_MEMMODE) && cd $(BUILD_ROOT)

joytest.exe:
	cd $(DOSLIB_JOYSTICK:$(to_os_path)) && $(DOSLIBMAKE) $(DOSLIB_MEMMODE) && cd $(BUILD_ROOT)
	$(COPYCOMMAND) $(DOSLIB_JOYSTICK:$(to_os_path))$(DIRSEP)$(DOSLIB_MEMMODE)$(DIRSEP)test.exe joytest.exe

#imfplay.exe:
#	cd $(DOSLIB_ADLIB:$(to_os_path)) && $(DOSLIBMAKE) $(DOSLIB_MEMMODE) && cd $(BUILD_ROOT)
#	$(COPYCOMMAND) $(DOSLIB_ADLIB:$(to_os_path))$(DIRSEP)$(DOSLIB_MEMMODE)$(DIRSEP)imfplay.exe imfplay.exe

opltest.exe:
	cd $(DOSLIB_ADLIB:$(to_os_path)) && $(DOSLIBMAKE) $(DOSLIB_MEMMODE) && cd $(BUILD_ROOT)
	$(COPYCOMMAND) $(DOSLIB_ADLIB:$(to_os_path))$(DIRSEP)$(DOSLIB_MEMMODE)$(DIRSEP)test.exe opltest.exe

16_vl.$(OBJ):	$(SRCLIB)/16_vl.c $(SRCLIB)/16_vl.h
16_vl_1.$(OBJ):	$(SRCLIB)/16_vl_1.c $(SRCLIB)/16_vl.h
16_vlpal.$(OBJ):	$(SRCLIB)/16_vlpa_.c	$(SRCLIB)/16_vlpal.c $(SRCLIB)/16_vlpal.h
bakapee.$(OBJ):	$(SRCLIB)/bakapee.c $(SRCLIB)/bakapee.h
16render.$(OBJ):$(SRCLIB)/16render.c $(SRCLIB)/16render.h
16planar.$(OBJ):$(MODEXLIB16)/16planar.c $(MODEXLIB16)/16planar.h
16_vrs.$(OBJ):	$(SRCLIB)/16_vrs.c $(SRCLIB)/16_vrs.h $(DOSLIB)
16_spri.$(OBJ):$(SRCLIB)/16_spri.c $(SRCLIB)/16_spri.h
bitmap.$(OBJ):	$(SRCLIB)/bitmap.c $(SRCLIB)/bitmap.h
planar.$(OBJ):	$(SRCLIB)/planar.c $(SRCLIB)/planar.h
scroll16.$(OBJ):$(SRCLIB)/scroll16.c $(SRCLIB)/scroll16.h
16text.$(OBJ):	$(SRCLIB)/16text.c $(SRCLIB)/16text.h
16_enti.$(OBJ):	$(SRCLIB)/16_enti.c $(SRCLIB)/16_enti.h
mapread.$(OBJ):$(SRCLIB)/mapread.c $(SRCLIB)/mapread.h
16_map.$(OBJ):$(SRCLIB)/16_map.c $(SRCLIB)/16_map.h
16_timer.$(OBJ):$(SRCLIB)/16_timer.c $(SRCLIB)/16_timer.h
16_in.$(OBJ):	 $(SRCLIB)/16_in.c $(SRCLIB)/16_in.h
#16_in_1.$(OBJ):	 $(SRCLIB)/16_in_1.c $(SRCLIB)/16_in_1.h
16_rf.$(OBJ):	 $(SRCLIB)/16_rf.c	$(SRCLIB)/16_rf.h
16_mm.$(OBJ):	 $(SRCLIB)/16_mm.c	$(SRCLIB)/16_mm.h
16_pm.$(OBJ):	 $(SRCLIB)/16_pm.c	$(SRCLIB)/16_pm.h
16_ca.$(OBJ):	 $(SRCLIB)/16_ca.c	$(SRCLIB)/16_ca.h
16_us.$(OBJ):	 $(SRCLIB)/16_us.c
16_dbg.$(OBJ):	$(SRCLIB)/16_dbg.c $(SRCLIB)/16_dbg.h
16_dbg_1.$(OBJ):	$(SRCLIB)/16_dbg_1.c $(SRCLIB)/16_dbg.h
midi.$(OBJ):	$(SRCLIB)/midi.c $(SRCLIB)/midi.h
16_head.$(OBJ):$(SRCLIB)/16_head.c $(SRCLIB)/16_head.h
16_tail.$(OBJ):$(SRCLIB)/16_tail.c $(SRCLIB)/16_tail.h
16_tail_.$(OBJ):$(SRCLIB)/16_tail_.c $(SRCLIB)/16_tail.h
16_hc.$(OBJ):	 $(SRCLIB)/16_hc.c $(SRCLIB)/16_hc.h
16_sd.$(OBJ):	$(SRCLIB)/16_sd.c $(SRCLIB)/16_sd.h
jsmn.$(OBJ):	$(JSMNLIB)/jsmn.c $(JSMNLIB)/jsmn.h
kitten.$(OBJ):	$(NYANLIB)/kitten.c $(NYANLIB)/kitten.h
vgmSnd.$(OBJ):	$(VGMSNDLIB)/vgmSnd.c $(VGMSNDLIB)/vgmSnd.h
16_wcpu.$(OBJ):	$(WCPULIB)/16_wcpu.c $(WCPULIB)/16_wcpu.h
#memory.$(OBJ):$(EXMMLIB)/memory.c $(EXMMLIB)/memory.h
c_utils.$(OBJ):$(MODEXLIB)/c_utils.asm
modex.$(OBJ):	 $(MODEXLIB)/modex.asm
ll.$(OBJ):		$(SRCLIB)/ll.c	$(SRCLIB)/ll.h

#
#other~
#
clean: .symbolic
	@if not exist $(DOSLIBDIR)/buildall.sh wmake -s -h initlibs
	@wmake -s -h initscript
	@for %f in ($(ALLEXEC)) do @if exist %f $(REMOVECOMMAND) %f
!ifdef __LINUX__
	@if exist *.LIB $(REMOVECOMMAND) *.LIB
	@. src/util/bcexmm.sh
	@if exist *.EXE $(REMOVECOMMAND) *.EXE
	@if exist *.OBJ $(REMOVECOMMAND) *.OBJ
	#@for %f in ($(SPRIUTILEXEC)) do @if exist %f $(REMOVECOMMAND) %f
	@if not exist vrl2vrs wmake -s -h pcx2vrl
!else
	@if exist *.o $(REMOVECOMMAND) *.o
!endif
	@if exist *.$(OBJ) $(REMOVECOMMAND) *.$(OBJ)
	@if exist *.bco $(REMOVECOMMAND) *.bco
	@if exist *.BCO $(REMOVECOMMAND) *.BCO
	@if exist *.LIB $(REMOVECOMMAND) *.LIB
	@if exist *.lnk $(REMOVECOMMAND) *.lnk
	@if exist *.LNK $(REMOVECOMMAND) *.LNK
	@if exist *.smp $(REMOVECOMMAND) *.smp
	@if exist *.SMP $(REMOVECOMMAND) *.SMP
	@if exist *.hed $(REMOVECOMMAND) *.hed
	@if exist *.meh $(REMOVECOMMAND) *.meh
	@if exist *.MEH $(REMOVECOMMAND) *.MEH
	@if exist *.err $(REMOVECOMMAND) *.err
	@if exist TC*.SWP $(REMOVECOMMAND) TC*.SWP
!ifeq DELLOGFILE 1
	@if exist *.16W $(REMOVECOMMAND) *.16W
	@if exist *.16B $(REMOVECOMMAND) *.16B
	@if exist *.L16 $(REMOVECOMMAND) *.L16
!endif
#	@$(COPYCOMMAND) $(SRC)exmmtest.c $(EXMMTESTDIR)$(SRC)
#	@$(COPYCOMMAND) $(SRCLIB)16_mm.* $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(SRCLIB)16_head.* $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(SRCLIB)16_ca.* $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(SRCLIB)16_hc.* $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(SRCLIB)types.h $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(NYANLIB)* $(EXMMTESTDIR)$(NYANLIB)

nuke: .symbolic
	@wmake clean
	@wmake backupscript
	@wmake cldl
	@wmake all
	@wmake comp

backupconfig: .symbolic
	@$(COPYCOMMAND) .git$(DIRSEP)config $(GITCONFIGDIR)/git_con.fig
	@$(COPYCOMMAND) .gitmodules $(GITCONFIGDIR)/git_modu.les
	@$(COPYCOMMAND) .gitignore $(GITCONFIGDIR)/git_igno.re

initconfig: .symbolic
	@$(COPYCOMMAND) $(GITCONFIGDIR)/git_con.fig .git/config
	@$(COPYCOMMAND) $(GITCONFIGDIR)/git_modu.les .gitmodules
	@$(COPYCOMMAND) $(GITCONFIGDIR)/git_igno.re .gitignore


backupscript: .symbolic
	@if exist *.bat $(MOVECOMMAND) *.bat $(SCRIPTBATDIR)/
	@if exist *.sh $(MOVECOMMAND) *.sh $(SCRIPTBATDIR)/
!ifdef __LINUX__
	@if exist *.BAT $(MOVECOMMAND) *.BAT $(SCRIPTBATDIR)/
	@if not exist ud.sh $(COPYCOMMAND) $(SCRIPTBATDIR)/ud.sh ./
!endif
	@if not exist wbuild.sh $(COPYCOMMAND) $(SCRIPTBATDIR)/wbuild.sh ./
	@if not exist WBUILD.BAT $(COPYCOMMAND) $(SCRIPTBATDIR)/WBUILD.BAT ./

initscript: .symbolic
	@$(COPYCOMMAND) $(SCRIPTBATDIR)/*.bat ./
	@$(COPYCOMMAND) $(SCRIPTBATDIR)/*.sh ./
!ifdef __LINUX__
	@$(COPYCOMMAND) $(SCRIPTBATDIR)/*.BAT ./
!endif


comp: .symbolic
	@*upx -9 $(EXEC)

comq: .symbolic
	@*upx -9 $(UPXQ) $(EXEC)

www: .symbolic
	@for %f in (/var/www/$(EXEC)) do @if exist /var/www/%f $(REMOVECOMMAND) /var/www/%f
	@$(REMOVECOMMAND) /var/www/*.exe.zi*
	@$(REMOVECOMMAND) /var/www/*.zip.zi*
	@for %f in ($(EXEC)) do @if exist %f @$(COPYCOMMAND) %f /var/www/
	@./src/util/z.sh $(EXEC) $(EXEC)
	@./src/util/z2.sh data.zip data
	@wmake -s -h wwwext

wwwext: .symbolic
	@wmake -s -h wwwext1
	@wmake -s -h wwwext2

wwwext1: .symbolic
	####----@ssh -p $(HOSTPORT) $(HOSTUSER)@$(HOSTADDR) 'rm -f $(HOSTDIR)/16/*exe*'
	#4ch.mooo.com
	@scp -r -P $(HOSTPORT) *.exe $(HOSTUSER)@$(HOSTADDR):$(HOSTDIR)/16/
	@scp -r -P $(HOSTPORT) *get.bat $(HOSTUSER)@$(HOSTADDR):$(HOSTDIR)/16/
	@scp -r -P $(HOSTPORT) /var/www/*.exe.zip.* $(HOSTUSER)@$(HOSTADDR):$(HOSTDIR)/16/
	#@scp -r -P $(HOSTPORT) $(DATADIR)spri.zip $(HOSTUSER)@$(HOSTADDR):$(HOSTDIR)/16/
	@scp -r -P $(HOSTPORT) /var/www/data.zip.* $(HOSTUSER)@$(HOSTADDR):$(HOSTDIR)/16/


wwwext2: .symbolic
	#beta.latech.edu
	@scp -r -P $(HOST2PORT) *.exe $(HOST2USER)@$(HOST2ADDR):$(HOST2DIR)/16/
	@scp -r -P $(HOST2PORT) *get.bat $(HOST2USER)@$(HOST2ADDR):$(HOST2DIR)/16/
	@scp -r -P $(HOST2PORT) /var/www/*.exe.zip.* $(HOST2USER)@$(HOST2ADDR):$(HOST2DIR)/16/
	#@scp -r -P $(HOST2PORT) $(DATADIR)spri.zip $(HOST2USER)@$(HOST2ADDR):$(HOST2DIR)/16/
	@scp -r -P $(HOST2PORT) /var/www/data.zip.* $(HOST2USER)@$(HOST2ADDR):$(HOST2DIR)/16/

getwww: .symbolic
	*x4get.bat $(EXEC)

vomitchan: .symbolic
	@if exist *.err $(DUMP) *.err

##
##	External library management~ ^^
##
#git submodule add <repo>
mkdl: .symbolic
	@cd $(DOSLIBDIR:$(to_os_path))
	@$(DOSLIBMAKEALL) $(DOSLIB_MEMMODE)
	@cd $(BUILD_ROOT)

cldl: .symbolic
	@cd $(DOSLIBDIR:$(to_os_path))
	@$(DOSLIBMAKEALL) $(DOSLIB_MEMMODE) clean
	@cd $(BUILD_ROOT)

uplibs: .symbolic
	@cd $(JSMNLIB:$(to_os_path))
	@git pull
	@cd $(BUILD_ROOT)
	@cd $(DOSLIBDIR:$(to_os_path))
	@git pull
	@cd $(BUILD_ROOT)

reinitlibs: .symbolic
	@$(REMOVECOMMAND) -rf $(DOSLIBDIR)
	@$(REMOVECOMMAND) -rf $(JSMNLIB)
	@$(REMOVECOMMAND) -rf 16/CatacombApocalypse
	@$(REMOVECOMMAND) -rf 16/wolf3d
	@$(REMOVECOMMAND) -rf 16/keen
	@$(REMOVECOMMAND) -rf 16/Catacomb3D
	@wmake -s -h initlibs

initlibs: .symbolic
	@wmake -s -h initconfig
	@wmake -s -h getlib
	@cd 16
	@git clone https://github.com/FlatRockSoft/CatacombApocalypse.git
	@git clone https://github.com/id-Software/wolf3d.git
	@git clone https://github.com/keendreams/keen.git
	@git clone https://github.com/FlatRockSoft/Catacomb3D.git
	@cd $(BUILD_ROOT)
	@$(COPYCOMMAND) $(DOSLIBDIR)/make-lowercase .

getlib: .symbolic
	@cd $(SRCLIB:$(to_os_path))
	@git clone https://github.com/joncampbell123/doslib.git
	@git clone https://github.com/zserge/jsmn.git
	@cd $(BUILD_ROOT)

##
##	experimental libs
##
xlib: .symbolic
	@cd 16$(DIRSEP)xlib
	@wmake -s -h clean
	@wmake -s -h all
	@cd $(BUILD_ROOT)

mx: .symbolic
	@cd 16$(DIRSEP)xw
#	@wmake clean
	@wmake -s -h all
	@cd $(BUILD_ROOT)

mx_: .symbolic
	@cd 16$(DIRSEP)xw_
	@wmake -s -h -f makefile all
	@cd $(BUILD_ROOT)

!ifdef __LINUX__
vrs: .symbolic
$(SPRIUTILEXEC):
	@cd $(DOSLIB_VGA:$(to_os_path))
	#@make clean
	@make all
	@for %f in ($(SPRIUTILEXEC)) do @if exist %f $(COPYCOMMAND) %f $(BUILD_ROOT)#$(DIRSEP)
	@cd $(BUILD_ROOT)
!endif
