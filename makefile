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

#%.C
#192x144
#wwww will add these
!ifdef __LINUX__
REMOVECOMMAND=rm -f
COPYCOMMAND=cp -f
DIRSEP=/
OBJ=o
DUMP=cat
!else
REMOVECOMMAND=del
COPYCOMMAND=copy /y
DIRSEP=\
OBJ=obj
DUMP=type
!endif

TARGET_OS = dos

#EXMMTESTDIR=16$(DIRSEP)exmmtest$(DIRSEP)
PDIR=..$(DIRSEP)
SRC=src$(DIRSEP)
DATADIR=data$(DIRSEP)
SPRI=$(DATADIR)$(DIRSEP)spri$(DIRSEP)
SRCLIB=$(SRC)lib$(DIRSEP)
JSMNLIB=$(SRCLIB)jsmn$(DIRSEP)
NYANLIB=$(SRCLIB)nyan$(DIRSEP)
#EXMMLIB=$(SRCLIB)exmm$(DIRSEP)
MODEXLIB=$(SRCLIB)modex16$(DIRSEP)
MODEXLIB_=$(SRCLIB)modex$(DIRSEP)
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
AFLAGS=-mh -0 -d1
SFLAGS=-sg -st -of+ -zu -zdf -zff -zgf -k32768#54096#60000
DFLAGS=-DTARGET_MSDOS=16 -DMSDOS=1 $(SFLAGS)
ZFLAGS=-zk0 -zc -zp8 -zm $(WCLQ)
LFLAGS=-lr -l=dos
CFLAGS=$(AFLAGS) $(IFLAGS) -wo -i$(DOSLIB) $(LFLAGS)
OFLAGS=-obmilr -oe=24 -out -oh -ei -zp8 -fpi87  -onac -ol+ -ok####x
FLAGS=$(CFLAGS) $(OFLAGS) $(DFLAGS) $(ZFLAGS)

#
# objects
#
VGMSNDOBJ = vgmSnd.$(OBJ) 16_snd.$(OBJ)
DOSLIBOBJ = adlib.$(OBJ) 8254.$(OBJ) 8259.$(OBJ) dos.$(OBJ) cpu.$(OBJ)
16LIBOBJS = 16_in.$(OBJ) 16_mm.$(OBJ) wcpu.$(OBJ) 16_head.$(OBJ) 16_ca.$(OBJ) 16_dbg.$(OBJ) kitten.$(OBJ) 16_hc.$(OBJ) 16_timer.$(OBJ)

GFXLIBOBJS = modex16.$(OBJ) bitmap.$(OBJ) planar.$(OBJ) 16text.$(OBJ) bakapee.$(OBJ) scroll16.$(OBJ) 16render.$(OBJ) 16planar.$(OBJ) 16_vrs.$(OBJ) $(DOSLIBLIBS)

DOSLIBLIBS=$(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)cpu$(DIRSEP)dos86h$(DIRSEP)cpu.lib $(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)dos$(DIRSEP)dos86h$(DIRSEP)dos.lib $(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)vga$(DIRSEP)dos86h$(DIRSEP)vga.lib

!ifeq DEBUGSERIAL 1
FLAGS += -DDEBUGSERIAL
DOSLIBOBJ += 8250.$(OBJ)
DOSLIBLIBS += $(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)8250$(DIRSEP)dos86h$(DIRSEP)8250.lib
!endif

TESTEXEC = test.exe test0.exe pcxtest.exe pcxtest2.exe test2.exe palettec.exe maptest.exe fmemtest.exe fonttest.exe fontgfx.exe scroll.exe vgmtest.exe inputest.exe palettel.exe planrpcx.exe exmmtest.exe

EXEC = 16.exe bakapi.exe tesuto.exe 0.exe $(TESTEXEC)

all: $(EXEC) joytest.exe
#16.lib => $(16LIBOBJS) bug....
16LIB=$(16LIBOBJS)
#16LIB=16.lib

!ifeq DEBUGSERIAL 1
16LIBOBJS += $(DOSLIBLIBS)
!endif
#
#game and bakapi executables
#
16.exe: 16.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) $(16LIB) gfx.lib
	wcl $(FLAGS) $(16FLAGS) 16.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) $(16LIB) gfx.lib -fm=16.mah

bakapi.exe: bakapi.$(OBJ) gfx.lib $(DOSLIBLIBS)
	wcl $(FLAGS) $(BAKAPIFLAGS) bakapi.$(OBJ) gfx.lib $(DOSLIBLIBS) -fm=bakapi.mah
#modex.lib
#
#Test Executables!
#
scroll.exe: scroll.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) $(16LIB) gfx.lib
	wcl $(FLAGS) scroll.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) $(16LIB) gfx.lib -fm=scroll.mah
scroll.$(OBJ): $(SRC)scroll.c
	wcl $(FLAGS) -c $(SRC)scroll.c


# NOTE: dos86h = 16-bit huge memory model. memory model must match!
tesuto.exe: tesuto.$(OBJ) $(DOSLIBLIBS) 16_head.$(OBJ) gfx.lib
#	%write tmp.cmd option quiet option max=tesuto.mah $(DOSLIB_LDFLAGS_DOS16H) file tesuto.obj name tesuto.exe
#	%write tmp.cmd library $(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)cpu$(DIRSEP)dos86h$(DIRSEP)cpu.lib
#	%write tmp.cmd library $(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)dos$(DIRSEP)dos86h$(DIRSEP)dos.lib
#	@wlink @tmp.cmd

#
# add this ww to reduce junk www
#
# NTS we have to construct the command line into tmp.cmd because for MS-DOS
# systems all arguments would exceed the pitiful 128 char command line limit
#.C.OBJ:
#	%write temp.cmd $(CFLAGS_THIS) $(CFLAGS_CON) $[@
#	wcl @temp.cmd
#
	wcl $(FLAGS) $(WCLQ) tesuto.$(OBJ) $(DOSLIBLIBS) 16_head.$(OBJ) gfx.lib
tesuto.$(OBJ): $(SRC)tesuto.c
	wcl $(FLAGS) $(WCLQ) -c $(SRC)tesuto.c

0.exe: 0.$(OBJ) $(DOSLIBLIBS) gfx.lib $(16LIB)
	wcl $(FLAGS) $(WCLQ) 0.$(OBJ) $(DOSLIBLIBS) gfx.lib $(16LIB)
0.$(OBJ): $(SRC)0.c
	wcl $(FLAGS) $(WCLQ) -c $(SRC)0.c

test.exe: test.$(OBJ) gfx.lib $(DOSLIBLIBS) $(16LIB)
	wcl $(FLAGS) test.$(OBJ) gfx.lib $(DOSLIBLIBS) $(16LIB) -fm=test.mah

test2.exe: test2.$(OBJ) $(DOSLIBLIBS) gfx.lib
	wcl $(FLAGS) test2.$(OBJ) $(DOSLIBLIBS) gfx.lib -fm=test2.mah

test0.exe: test0.$(OBJ)
	wcl $(FLAGS) test0.$(OBJ) -fm=test0.mah

fonttest.exe: fonttest.$(OBJ) $(16LIB) gfx.lib
	wcl $(FLAGS) fonttest.$(OBJ) $(16LIB) gfx.lib -fm=fonttest.mah

#fonttes0.exe: fonttes0.$(OBJ) $(16LIB)
#	wcl $(FLAGS) fonttes0.$(OBJ) $(16LIB)

fontgfx.exe: fontgfx.$(OBJ) $(16LIB) gfx.lib $(DOSLIBLIBS)
	wcl $(FLAGS) fontgfx.$(OBJ) $(16LIB) gfx.lib $(DOSLIBLIBS) -fm=fontgfx.mah

inputest.exe: inputest.$(OBJ) $(16LIB)
	wcl $(FLAGS) inputest.$(OBJ) $(16LIB) -fm=inputest.mah

#sountest.exe: sountest.$(OBJ) $(16LIB)
#	wcl $(FLAGS) sountest.$(OBJ) $(16LIB)

pcxtest.exe: pcxtest.$(OBJ) gfx.lib
	wcl $(FLAGS) pcxtest.$(OBJ) gfx.lib -fm=pcxtest.mah

palettec.exe: palettec.$(OBJ) gfx.lib #$(16LIB)
	wcl $(FLAGS) palettec.$(OBJ) gfx.lib -fm=palettec.mah #$(16LIB)

palettel.exe: palettel.$(OBJ) gfx.lib #$(16LIB)
	wcl $(FLAGS) palettel.$(OBJ) gfx.lib -fm=palettel.mah #$(16LIB)

pcxtest2.exe: pcxtest2.$(OBJ) gfx.lib
	wcl $(FLAGS) pcxtest2.$(OBJ) gfx.lib -fm=pcxtest2.mah

planrpcx.exe: planrpcx.$(OBJ) gfx.lib
	wcl $(FLAGS) planrpcx.$(OBJ) gfx.lib -fm=planrpcx.mah

maptest.exe: maptest.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) $(16LIB) gfx.lib
	wcl $(FLAGS) maptest.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) $(16LIB) gfx.lib -fm=maptest.mah

fmemtest.exe: fmemtest.$(OBJ) $(16LIB)
	wcl $(FLAGS) fmemtest.$(OBJ) $(16LIB) -fm=fmemtest.mah

exmmtest.exe: exmmtest.$(OBJ) 16_in.$(OBJ) 16_mm.$(OBJ) wcpu.$(OBJ) 16_head.$(OBJ) 16_ca.$(OBJ) 16_hc.$(OBJ) kitten.$(OBJ)
	wcl $(FLAGS) exmmtest.$(OBJ) -fm=exmmtest.mah $(16LIB)
####0000	./wlink32 @__wcl__.lnk

vgmtest.exe: vgmtest.$(OBJ) vgmsnd.lib $(16LIB)
	wcl $(FLAGS) vgmtest.$(OBJ) vgmsnd.lib -fm=vgmtest.mah $(16LIB)
	#====wcl -mc vgmtest.$(OBJ) $(VGMSNDOBJ) -fm=vgmtest.mah


#
#executable's objects
#
16.$(OBJ): $(SRC)16.h $(SRC)16.c
	wcl $(FLAGS) -c $(SRC)16.c

bakapi.$(OBJ): $(SRC)bakapi.h $(SRC)bakapi.c
	wcl $(FLAGS) -c $(SRC)bakapi.c

test.$(OBJ): $(SRC)test.c $(SRCLIB)modex16.h
	wcl $(FLAGS) -c $(SRC)test.c

test2.$(OBJ): $(SRC)test2.c $(SRCLIB)modex16.h
	wcl $(FLAGS) -c $(SRC)test2.c

test0.$(OBJ): $(SRC)test0.c
	wcl $(FLAGS) -c $(SRC)test0.c

pcxtest.$(OBJ): $(SRC)pcxtest.c $(SRCLIB)modex16.h
	wcl $(FLAGS) -c $(SRC)pcxtest.c

planrpcx.$(OBJ): $(SRC)planrpcx.c $(SRCLIB)modex16.h
	wcl $(FLAGS) -c $(SRC)planrpcx.c

pcxtest2.$(OBJ): $(SRC)pcxtest2.c $(SRCLIB)modex16.h
	wcl $(FLAGS) -c $(SRC)pcxtest2.c

palettec.$(OBJ): $(SRC)palettec.c
	wcl $(FLAGS) -c $(SRC)palettec.c

palettel.$(OBJ): $(SRC)palettel.c
	wcl $(FLAGS) -c $(SRC)palettel.c

maptest.$(OBJ): $(SRC)maptest.c $(SRCLIB)modex16.h
	wcl $(FLAGS) -c $(SRC)maptest.c

#maptest0.$(OBJ): $(SRC)maptest0.c# $(SRCLIB)modex16.h
#	wcl $(FLAGS) $(MFLAGS) -c $(SRC)maptest0.c

#emmtest.$(OBJ): $(SRC)emmtest.c
#	wcl $(FLAGS) $(MFLAGS) -c $(SRC)emmtest.c

#emsdump.$(OBJ): $(SRC)emsdump.c
#	wcl $(FLAGS) $(MFLAGS) -c $(SRC)emsdump.c

fmemtest.$(OBJ): $(SRC)fmemtest.c
	wcl $(FLAGS) -c $(SRC)fmemtest.c

fonttest.$(OBJ): $(SRC)fonttest.c
	wcl $(FLAGS) -c $(SRC)fonttest.c

#fonttes0.$(OBJ): $(SRC)fonttes0.c
#	wcl $(FLAGS) -c $(SRC)fonttes0.c

fontgfx.$(OBJ): $(SRC)fontgfx.c
	wcl $(FLAGS) -c $(SRC)fontgfx.c

inputest.$(OBJ): $(SRC)inputest.c
	wcl $(FLAGS) -c $(SRC)inputest.c

#sountest.$(OBJ): $(SRC)sountest.c
#	wcl $(FLAGS) -c $(SRC)sountest.c

#miditest.$(OBJ): $(SRC)miditest.c
#	wcl $(FLAGS) -c $(SRC)miditest.c

#testemm.$(OBJ): $(SRC)testemm.c
#	wcl $(FLAGS) -c $(SRC)testemm.c

#testemm0.$(OBJ): $(SRC)testemm0.c
#	wcl $(FLAGS) -c $(SRC)testemm0.c

tsthimem.$(OBJ): $(SRC)tsthimem.c
	wcl $(FLAGS) -c $(SRC)tsthimem.c

exmmtest.$(OBJ): $(SRC)exmmtest.c
	wcl $(FLAGS) -c $(SRC)exmmtest.c 16_in.$(OBJ) 16_mm.$(OBJ) wcpu.$(OBJ) 16_head.$(OBJ) 16_ca.$(OBJ) 16_hc.$(OBJ) kitten.$(OBJ)

vgmtest.$(OBJ): $(SRC)vgmtest.c
	wcl $(FLAGS) -c $(SRC)vgmtest.c
	#====wcl -mc -c $(SRC)vgmtest.c

#
#non executable objects libraries
#
16.lib: $(16LIBOBJS)# doslib.lib vgmsnd.lib
	wlib -b $(WLIBQ) 16.lib $(16LIBOBJS)# doslib.lib vgmsnd.lib

gfx.lib: $(GFXLIBOBJS)
	wlib -b $(WLIBQ) gfx.lib $(GFXLIBOBJS)

vgmsnd.lib: $(VGMSNDOBJ)
	wlib -b $(WLIBQ) vgmsnd.lib $(VGMSNDOBJ)

# extdep:
# !include $(DOSLIBDIR)$(DIRSEP)extdep.mak

# library deps 16-bit huge
$(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)cpu$(DIRSEP)dos86h$(DIRSEP)cpu.lib:
	cd $(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)cpu && .$(DIRSEP)make.sh
$(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)dos$(DIRSEP)dos86h$(DIRSEP)dos.lib:
	cd $(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)dos && .$(DIRSEP)make.sh
$(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)vga$(DIRSEP)dos86h$(DIRSEP)vgatty.lib:
	cd $(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)vga && .$(DIRSEP)make.sh
$(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)vga$(DIRSEP)dos86h$(DIRSEP)vga.lib:
	cd $(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)vga && .$(DIRSEP)make.sh
$(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)8250$(DIRSEP)dos86h$(DIRSEP)8250.lib:
	cd $(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)8250 && .$(DIRSEP)make.sh

joytest.exe:
	cd $(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)joystick && .$(DIRSEP)make.sh && $(COPYCOMMAND) dos86h$(DIRSEP)test.exe $(PDIR)$(PDIR)$(PDIR)$(PDIR)$(PDIR)joytest.exe
#$(DOSLIBLIBS): .symbolic
#	@cd $(DOSLIB)
#	@.$(DIRSEP)buildall.sh
#	@cd $(PDIR)$(PDIR)$(PDIR)

modex16.$(OBJ): $(SRCLIB)modex16.h $(SRCLIB)modex16.c
	wcl $(FLAGS) -c $(SRCLIB)modex16.c

bakapee.$(OBJ): $(SRCLIB)bakapee.h $(SRCLIB)bakapee.c
	wcl $(FLAGS) -c $(SRCLIB)bakapee.c

16render.$(OBJ): $(MODEXLIB)16render.h $(MODEXLIB)16render.c
	wcl $(FLAGS) -c $(MODEXLIB)16render.c

16planar.$(OBJ): $(MODEXLIB)16planar.h $(MODEXLIB)16planar.c
	wcl $(FLAGS) -c $(MODEXLIB)16planar.c

16_vrs.$(OBJ): $(SRCLIB)16_vrs.h $(SRCLIB)16_vrs.c
	wcl $(FLAGS) -c $(SRCLIB)16_vrs.c

bitmap.$(OBJ): $(SRCLIB)bitmap.h $(SRCLIB)bitmap.c
	wcl $(FLAGS) -c $(SRCLIB)bitmap.c

planar.$(OBJ): $(SRCLIB)planar.h $(SRCLIB)planar.c
	wcl $(FLAGS) -c $(SRCLIB)planar.c

scroll16.$(OBJ): $(SRCLIB)scroll16.h $(SRCLIB)scroll16.c
	wcl $(FLAGS) -c $(SRCLIB)scroll16.c

wcpu.$(OBJ): $(WCPULIB)wcpu.h $(WCPULIB)wcpu.c
	wcl $(FLAGS) -c $(WCPULIB)wcpu.c

16text.$(OBJ): $(SRCLIB)16text.c
	wcl $(FLAGS) -c $(SRCLIB)16text.c

mapread.$(OBJ): $(SRCLIB)mapread.h $(SRCLIB)mapread.c
	wcl $(FLAGS) -c $(SRCLIB)mapread.c

16_timer.$(OBJ): $(SRCLIB)16_timer.h $(SRCLIB)16_timer.c
	wcl $(FLAGS) -c $(SRCLIB)16_timer.c

16_in.$(OBJ): $(SRCLIB)16_in.h $(SRCLIB)16_in.c
	wcl $(FLAGS) -c $(SRCLIB)16_in.c

16_mm.$(OBJ): $(SRCLIB)16_mm.h $(SRCLIB)16_mm.c
	wcl $(FLAGS) -c $(SRCLIB)16_mm.c

16_ca.$(OBJ): $(SRCLIB)16_ca.h $(SRCLIB)16_ca.c
	wcl $(FLAGS) -c $(SRCLIB)16_ca.c

16_dbg.$(OBJ): $(SRCLIB)16_dbg.h $(SRCLIB)16_dbg.c
	wcl $(FLAGS) -c $(SRCLIB)16_dbg.c

midi.$(OBJ): $(SRCLIB)midi.h $(SRCLIB)midi.c
	wcl $(FLAGS) -c $(SRCLIB)midi.c

16_head.$(OBJ): $(SRCLIB)16_head.h $(SRCLIB)16_head.c
	wcl $(FLAGS) -c $(SRCLIB)16_head.c

16_hc.$(OBJ): $(SRCLIB)16_hc.h $(SRCLIB)16_hc.c
	wcl $(FLAGS) -c $(SRCLIB)16_hc.c

16_snd.$(OBJ): $(SRCLIB)16_snd.h $(SRCLIB)16_snd.c
	wcl $(FLAGS) -c $(SRCLIB)16_snd.c
	#====wcl -mc -c $(SRCLIB)16_snd.c

jsmn.$(OBJ): $(JSMNLIB)jsmn.h $(JSMNLIB)jsmn.c
	wcl $(FLAGS) -c $(JSMNLIB)jsmn.c

kitten.$(OBJ): $(NYANLIB)kitten.h $(NYANLIB)kitten.c
	wcl $(FLAGS) -c $(NYANLIB)kitten.c

vgmSnd.$(OBJ): $(VGMSNDLIB)vgmSnd.h $(VGMSNDLIB)vgmSnd.c
	wcl $(FLAGS) -c $(VGMSNDLIB)vgmSnd.c
	#====wcl -c -mc $(VGMSNDLIB)vgmSnd.c

#memory.$(OBJ): $(EXMMLIB)memory.h $(EXMMLIB)memory.c
#	wcl $(FLAGS) $(MFLAGS) -c $(EXMMLIB)memory.c

c_utils.$(OBJ): $(MODEXLIB_)c_utils.asm
	wcl -c $(AFLAGS) $(MODEXLIB_)c_utils.asm
modex.$(OBJ): $(MODEXLIB_)modex.asm
	wcl -c $(AFLAGS) $(MODEXLIB_)modex.asm

#
#other~
#
clean: .symbolic
	@$(REMOVECOMMAND) $(EXEC)
	@$(REMOVECOMMAND) /var/www/$(EXEC)*
	@$(REMOVECOMMAND) *.$(OBJ)
	@$(REMOVECOMMAND) 16.lib
	@$(REMOVECOMMAND) gfx.lib
	@$(REMOVECOMMAND) vgmsnd.lib
	@wlib -n $(WLIBQ) 16.lib
	@wlib -n $(WLIBQ) gfx.lib
	@wlib -n $(WLIBQ) vgmsnd.lib
	@$(REMOVECOMMAND) *.16
	@$(REMOVECOMMAND) *.16W
	@$(REMOVECOMMAND) *.16B
	@$(REMOVECOMMAND) *.OBJ
	@$(REMOVECOMMAND) *.o
	@$(REMOVECOMMAND) *.BCO
	@$(REMOVECOMMAND) makefi~1
	@$(REMOVECOMMAND) makefile~
	@$(REMOVECOMMAND) __wcl__.LNK
#	@$(REMOVECOMMAND) *.smp
	@$(REMOVECOMMAND) *.SMP
	@$(REMOVECOMMAND) *.hed
	@$(REMOVECOMMAND) *.MAH
	@$(REMOVECOMMAND) *.mah
	@$(REMOVECOMMAND) *.err
	#@cd $(DOSLIB)
	#@./buildall.sh clean
	#@cd $(PDIR)$(PDIR)$(PDIR)
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
	@upx -9 $(EXEC)

comq: .symbolic
	@upx -9 $(UPXQ) $(EXEC)

www: .symbolic
	@ssh -p 26 sparky4@4ch.mooo.com 'rm -f /var/www/16/*exe.zip*'
	@rm -f /var/www/*.exe.zip*
	#@cp ./$(EXEC) $(DIRSEP)var$(DIRSEP)www$(DIRSEP)
	@./src/util/z.sh $(EXEC) $(EXEC)
	@scp -r -P 26 *.exe 4ch.mooo.com:/var/www/16/
	@scp -r -P 26 /var/www/*.exe.zip.* 4ch.mooo.com:/var/www/16/

getwww: .symbolic
	@x4get.bat $(EXEC)

vomitchan: .symbolic
	@$(DUMP) *.err

##
##	External library management~ ^^
##
#git submodule add <repo>
mkdl: .symbolic
	@cd $(DOSLIB)
	@./buildall.sh
	@cd $(PDIR)$(PDIR)$(PDIR)

uplibs: .symbolic
	@cd $(JSMNLIB)
	@git pull
	@cd $(PDIR)$(PDIR)$(PDIR)
	@cd $(DOSLIB)
	@git pull
	@cd $(PDIR)$(PDIR)$(PDIR)

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
	@cd $(PDIR)$(PDIR)
	@cd 16
	@git clone https://github.com/FlatRockSoft/CatacombApocalypse.git
	@git clone https://github.com/id-Software/wolf3d.git
	@git clone https://github.com/keendreams/keen.git
	@cd $(PDIR)

##
##	experimental libs
##
xlib: .symbolic
	@cd 16$(DIRSEP)xlib
	@wmake -h clean
	@wmake -h all
	@cd $(PDIR)$(PDIR)

mx: .symbolic
	@cd 16$(DIRSEP)xw
#	@wmake clean
	@wmake -h all
	@cd $(PDIR)$(PDIR)

mx_: .symbolic
	@cd 16$(DIRSEP)xw_
	@wmake -h -f makefile all
	@cd $(PDIR)$(PDIR)

vrs: .symbolic
	@cd $(DOSLIBDIR)$(DIRSEP)hw$(DIRSEP)vga
	#@make clean
	@make all
	@mv pcx2vrl ../../../../../
	@mv pcxsscut ../../../../../
	@mv vrl2vrs ../../../../../
	@mv vrsdump ../../../../../
	@cd ../../../../../
