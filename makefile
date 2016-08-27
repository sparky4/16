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
OBJ=obj
DUMP=cat
WCL=wcl
WLIB=wlib
UPX=upx
!else		#DOS ^^
REMOVECOMMAND=*del
COPYCOMMAND=*copy /y
DIRSEP=\
OBJ=obj
DUMP=*type
WCL=*wcl
WLIB=*wlib
UPX=*upx
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
CFLAGS=$(AFLAGS) $(IFLAGS) -wo -i$(DOSLIB) $(LFLAGS) -fo=.$(OBJ)
OFLAGS=-obmilr -oe=24 -out -oh -ei -zp8 -fpi87  -onac -ol+ -ok##x
FLAGS=$(CFLAGS) $(OFLAGS) $(DFLAGS) $(ZFLAGS)

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
##planrpcx.exe test2.exe
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
	$(WCL) $(FLAGS) $(16FLAGS) 16.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) $(16LIB) gfx.lib -fm=16.mah

bakapi.exe: bakapi.$(OBJ) gfx.lib $(DOSLIBLIBS)
	$(WCL) $(FLAGS) $(BAKAPIFLAGS) bakapi.$(OBJ) gfx.lib $(DOSLIBLIBS) -fm=bakapi.mah

#
#Test Executables!
#
scroll.exe: scroll.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) $(16LIB) gfx.lib
	$(WCL) $(FLAGS) scroll.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) $(16LIB) gfx.lib -fm=scroll.mah
scroll.$(OBJ): $(SRC)scroll.c
	$(WCL) $(FLAGS) -c $(SRC)scroll.c

tesuto.exe: tesuto.$(OBJ) $(DOSLIBLIBS) 16_head.$(OBJ) gfx.lib
	$(WCL) $(FLAGS) $(WCLQ) tesuto.$(OBJ) $(DOSLIBLIBS) 16_head.$(OBJ) gfx.lib
tesuto.$(OBJ): $(SRC)tesuto.c
	$(WCL) $(FLAGS) $(WCLQ) -c $(SRC)tesuto.c

0.exe: 0.$(OBJ) $(DOSLIBLIBS) gfx.lib $(16LIB)
	$(WCL) $(FLAGS) $(WCLQ) 0.$(OBJ) $(DOSLIBLIBS) gfx.lib $(16LIB)
0.$(OBJ): $(SRC)0.c
	$(WCL) $(FLAGS) $(WCLQ) -c $(SRC)0.c

test.exe: test.$(OBJ) gfx.lib $(DOSLIBLIBS) $(16LIB)
	$(WCL) $(FLAGS) test.$(OBJ) gfx.lib $(DOSLIBLIBS) $(16LIB) -fm=test.mah

#test2.exe: test2.$(OBJ) $(DOSLIBLIBS) gfx.lib
#	$(WCL) $(FLAGS) test2.$(OBJ) $(DOSLIBLIBS) gfx.lib -fm=test2.mah

test0.exe: test0.$(OBJ)
	$(WCL) $(FLAGS) test0.$(OBJ) -fm=test0.mah

fonttest.exe: fonttest.$(OBJ) $(16LIB) gfx.lib
	$(WCL) $(FLAGS) fonttest.$(OBJ) $(16LIB) gfx.lib -fm=fonttest.mah

#fonttes0.exe: fonttes0.$(OBJ) $(16LIB)
#	$(WCL) $(FLAGS) fonttes0.$(OBJ) $(16LIB)

fontgfx.exe: fontgfx.$(OBJ) $(16LIB) gfx.lib $(DOSLIBLIBS)
	$(WCL) $(FLAGS) fontgfx.$(OBJ) $(16LIB) gfx.lib $(DOSLIBLIBS) -fm=fontgfx.mah

inputest.exe: inputest.$(OBJ) $(16LIB)
	$(WCL) $(FLAGS) inputest.$(OBJ) $(16LIB) -fm=inputest.mah

#sountest.exe: sountest.$(OBJ) $(16LIB)
#	$(WCL) $(FLAGS) sountest.$(OBJ) $(16LIB)

pcxtest.exe: pcxtest.$(OBJ) gfx.lib
	$(WCL) $(FLAGS) pcxtest.$(OBJ) gfx.lib -fm=pcxtest.mah

vrstest.exe: vrstest.$(OBJ) gfx.lib $(16LIB)
	$(WCL) $(FLAGS) vrstest.$(OBJ) gfx.lib $(16LIB) -fm=vrstest.mah

palettec.exe: palettec.$(OBJ) gfx.lib #$(16LIB)
	$(WCL) $(FLAGS) palettec.$(OBJ) gfx.lib -fm=palettec.mah #$(16LIB)

palettel.exe: palettel.$(OBJ) gfx.lib #$(16LIB)
	$(WCL) $(FLAGS) palettel.$(OBJ) gfx.lib -fm=palettel.mah #$(16LIB)

pcxtest2.exe: pcxtest2.$(OBJ) gfx.lib
	$(WCL) $(FLAGS) pcxtest2.$(OBJ) gfx.lib -fm=pcxtest2.mah

##planrpcx.exe: planrpcx.$(OBJ) gfx.lib
##	$(WCL) $(FLAGS) planrpcx.$(OBJ) gfx.lib -fm=planrpcx.mah

maptest.exe: maptest.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) $(16LIB) gfx.lib
	$(WCL) $(FLAGS) maptest.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) $(16LIB) gfx.lib -fm=maptest.mah

fmemtest.exe: fmemtest.$(OBJ) $(16LIB)
	$(WCL) $(FLAGS) fmemtest.$(OBJ) $(16LIB) -fm=fmemtest.mah

exmmtest.exe: exmmtest.$(OBJ) 16_in.$(OBJ) 16_mm.$(OBJ) wcpu.$(OBJ) 16_head.$(OBJ) 16_ca.$(OBJ) 16_hc.$(OBJ) kitten.$(OBJ)
	$(WCL) $(FLAGS) exmmtest.$(OBJ) -fm=exmmtest.mah $(16LIB)

vgmtest.exe: vgmtest.$(OBJ) vgmsnd.lib $(16LIB)
	$(WCL) $(FLAGS) vgmtest.$(OBJ) vgmsnd.lib -fm=vgmtest.mah $(16LIB)


#
#executable's objects
#
{$(SRC)}.C.$(OBJ):
	$(WCL) $(FLAGS) -c $[@

16.$(OBJ): $(SRC)16.h $(SRC)16.c
####	$(WCL) $(FLAGS) -c $(SRC)16.c

bakapi.$(OBJ): $(SRC)bakapi.h $(SRC)bakapi.c
####	$(WCL) $(FLAGS) -c $(SRC)bakapi.c

test.$(OBJ): $(SRC)test.c $(SRCLIB)modex16.h
####	$(WCL) $(FLAGS) -c $(SRC)test.c

#test2.$(OBJ): $(SRC)test2.c $(SRCLIB)modex16.h
#####	$(WCL) $(FLAGS) -c $(SRC)test2.c

test0.$(OBJ): $(SRC)test0.c
####	$(WCL) $(FLAGS) -c $(SRC)test0.c

pcxtest.$(OBJ): $(SRC)pcxtest.c $(SRCLIB)modex16.h
####	$(WCL) $(FLAGS) -c $(SRC)pcxtest.c

vrstest.$(OBJ): $(SRC)vrstest.c $(SRCLIB)modex16.h
####	$(WCL) $(FLAGS) -c $(SRC)vrstest.c

##planrpcx.$(OBJ): $(SRC)planrpcx.c $(SRCLIB)modex16.h
######	$(WCL) $(FLAGS) -c $(SRC)planrpcx.c

pcxtest2.$(OBJ): $(SRC)pcxtest2.c $(SRCLIB)modex16.h
####	$(WCL) $(FLAGS) -c $(SRC)pcxtest2.c

palettec.$(OBJ): $(SRC)palettec.c
####	$(WCL) $(FLAGS) -c $(SRC)palettec.c

palettel.$(OBJ): $(SRC)palettel.c
####	$(WCL) $(FLAGS) -c $(SRC)palettel.c

maptest.$(OBJ): $(SRC)maptest.c $(SRCLIB)modex16.h
####	$(WCL) $(FLAGS) -c $(SRC)maptest.c

#emmtest.$(OBJ): $(SRC)emmtest.c
#####	$(WCL) $(FLAGS) $(MFLAGS) -c $(SRC)emmtest.c

#emsdump.$(OBJ): $(SRC)emsdump.c
#####	$(WCL) $(FLAGS) $(MFLAGS) -c $(SRC)emsdump.c

fmemtest.$(OBJ): $(SRC)fmemtest.c
####	$(WCL) $(FLAGS) -c $(SRC)fmemtest.c

fonttest.$(OBJ): $(SRC)fonttest.c
####	$(WCL) $(FLAGS) -c $(SRC)fonttest.c

#fonttes0.$(OBJ): $(SRC)fonttes0.c
#####	$(WCL) $(FLAGS) -c $(SRC)fonttes0.c

fontgfx.$(OBJ): $(SRC)fontgfx.c
####	$(WCL) $(FLAGS) -c $(SRC)fontgfx.c

inputest.$(OBJ): $(SRC)inputest.c
####	$(WCL) $(FLAGS) -c $(SRC)inputest.c

#sountest.$(OBJ): $(SRC)sountest.c
#	$(WCL) $(FLAGS) -c $(SRC)sountest.c

#miditest.$(OBJ): $(SRC)miditest.c
#	$(WCL) $(FLAGS) -c $(SRC)miditest.c

#testemm.$(OBJ): $(SRC)testemm.c
#	$(WCL) $(FLAGS) -c $(SRC)testemm.c

#testemm0.$(OBJ): $(SRC)testemm0.c
#	$(WCL) $(FLAGS) -c $(SRC)testemm0.c

tsthimem.$(OBJ): $(SRC)tsthimem.c
####	$(WCL) $(FLAGS) -c $(SRC)tsthimem.c

exmmtest.$(OBJ): $(SRC)exmmtest.c
####	$(WCL) $(FLAGS) -c $(SRC)exmmtest.c 16_in.$(OBJ) 16_mm.$(OBJ) wcpu.$(OBJ) 16_head.$(OBJ) 16_ca.$(OBJ) 16_hc.$(OBJ) kitten.$(OBJ)

vgmtest.$(OBJ): $(SRC)vgmtest.c
####	$(WCL) $(FLAGS) -c $(SRC)vgmtest.c
	#====$(WCL) -mc -c $(SRC)vgmtest.c

#
#non executable objects libraries
#
16.lib: $(16LIBOBJS)# doslib.lib vgmsnd.lib
	$(WLIB) -b $(WLIBQ) 16.lib $(16LIBOBJS)# doslib.lib vgmsnd.lib

gfx.lib: $(GFXLIBOBJS)
	$(WLIB) -b $(WLIBQ) gfx.lib $(GFXLIBOBJS)

vgmsnd.lib: $(VGMSNDOBJ)
	$(WLIB) -b $(WLIBQ) vgmsnd.lib $(VGMSNDOBJ)

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
	$(WCL) $(FLAGS) -c $(SRCLIB)modex16.c

bakapee.$(OBJ): $(SRCLIB)bakapee.h $(SRCLIB)bakapee.c
	$(WCL) $(FLAGS) -c $(SRCLIB)bakapee.c

16render.$(OBJ): $(MODEXLIB)16render.h $(MODEXLIB)16render.c
	$(WCL) $(FLAGS) -c $(MODEXLIB)16render.c

##16planar.$(OBJ): $(MODEXLIB)16planar.h $(MODEXLIB)16planar.c
##	$(WCL) $(FLAGS) -c $(MODEXLIB)16planar.c

16_vrs.$(OBJ): $(SRCLIB)16_vrs.h $(SRCLIB)16_vrs.c $(DOSLIBLIBS)
	$(WCL) $(FLAGS) -c $(SRCLIB)16_vrs.c $(DOSLIBLIBS)
16_sprit.$(OBJ): $(SRCLIB)16_sprit.h $(SRCLIB)16_sprit.c
	$(WCL) $(FLAGS) -c $(SRCLIB)16_sprit.c


bitmap.$(OBJ): $(SRCLIB)bitmap.h $(SRCLIB)bitmap.c
	$(WCL) $(FLAGS) -c $(SRCLIB)bitmap.c

##planar.$(OBJ): $(SRCLIB)planar.h $(SRCLIB)planar.c
##	$(WCL) $(FLAGS) -c $(SRCLIB)planar.c

scroll16.$(OBJ): $(SRCLIB)scroll16.h $(SRCLIB)scroll16.c
	$(WCL) $(FLAGS) -c $(SRCLIB)scroll16.c

wcpu.$(OBJ): $(WCPULIB)wcpu.h $(WCPULIB)wcpu.c
	$(WCL) $(FLAGS) -c $(WCPULIB)wcpu.c

16text.$(OBJ): $(SRCLIB)16text.c
	$(WCL) $(FLAGS) -c $(SRCLIB)16text.c

mapread.$(OBJ): $(SRCLIB)mapread.h $(SRCLIB)mapread.c
	$(WCL) $(FLAGS) -c $(SRCLIB)mapread.c

16_timer.$(OBJ): $(SRCLIB)16_timer.h $(SRCLIB)16_timer.c
	$(WCL) $(FLAGS) -c $(SRCLIB)16_timer.c

16_in.$(OBJ): $(SRCLIB)16_in.h $(SRCLIB)16_in.c
	$(WCL) $(FLAGS) -c $(SRCLIB)16_in.c

16_mm.$(OBJ): $(SRCLIB)16_mm.h $(SRCLIB)16_mm.c
	$(WCL) $(FLAGS) -c $(SRCLIB)16_mm.c

16_ca.$(OBJ): $(SRCLIB)16_ca.h $(SRCLIB)16_ca.c
	$(WCL) $(FLAGS) -c $(SRCLIB)16_ca.c

16_dbg.$(OBJ): $(SRCLIB)16_dbg.h $(SRCLIB)16_dbg.c
	$(WCL) $(FLAGS) -c $(SRCLIB)16_dbg.c

midi.$(OBJ): $(SRCLIB)midi.h $(SRCLIB)midi.c
	$(WCL) $(FLAGS) -c $(SRCLIB)midi.c

16_head.$(OBJ): $(SRCLIB)16_head.h $(SRCLIB)16_head.c
	$(WCL) $(FLAGS) -c $(SRCLIB)16_head.c

16_hc.$(OBJ): $(SRCLIB)16_hc.h $(SRCLIB)16_hc.c
	$(WCL) $(FLAGS) -c $(SRCLIB)16_hc.c

16_snd.$(OBJ): $(SRCLIB)16_snd.h $(SRCLIB)16_snd.c
	$(WCL) $(FLAGS) -c $(SRCLIB)16_snd.c
	#====$(WCL) -mc -c $(SRCLIB)16_snd.c

jsmn.$(OBJ): $(JSMNLIB)jsmn.h $(JSMNLIB)jsmn.c
	$(WCL) $(FLAGS) -c $(JSMNLIB)jsmn.c

kitten.$(OBJ): $(NYANLIB)kitten.h $(NYANLIB)kitten.c
	$(WCL) $(FLAGS) -c $(NYANLIB)kitten.c

vgmSnd.$(OBJ): $(VGMSNDLIB)vgmSnd.h $(VGMSNDLIB)vgmSnd.c
	$(WCL) $(FLAGS) -c $(VGMSNDLIB)vgmSnd.c
	#====$(WCL) -c -mc $(VGMSNDLIB)vgmSnd.c

#memory.$(OBJ): $(EXMMLIB)memory.h $(EXMMLIB)memory.c
#	$(WCL) $(FLAGS) $(MFLAGS) -c $(EXMMLIB)memory.c

c_utils.$(OBJ): $(MODEXLIB_)c_utils.asm
	$(WCL) -c $(AFLAGS) $(MODEXLIB_)c_utils.asm
modex.$(OBJ): $(MODEXLIB_)modex.asm
	$(WCL) -c $(AFLAGS) $(MODEXLIB_)modex.asm

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
	@$(WLIB) -n $(WLIBQ) 16.lib
	@$(WLIB) -n $(WLIBQ) gfx.lib
	@$(WLIB) -n $(WLIBQ) vgmsnd.lib
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
	@$(UPX) -9 $(EXEC)

comq: .symbolic
	@$(UPX) -9 $(UPXQ) $(EXEC)

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
	@cp $(SRCLIB)doslib/make-lowercase .

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
