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

#%.C
#192x144
#wwww will add these
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
#!ifndef INCLUDE
#IN=..$(DIRSEP)..$(DIRSEP)fdos$(DIRSEP)watcom2$(DIRSEP)h
#IFLAGS=-i=$(IN)
#!endif

TARGET_OS = dos

#EXMMTESTDIR=16$(DIRSEP)exmmtest$(DIRSEP)
PDIR=..$(DIRSEP)
SRC=src$(DIRSEP)
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

WLIBQ=-q
WCLQ=-zq $(WLIBQ)
UPXQ=-qqq

AFLAGS=-mh -0 -d1
16FLAGS=-fh=16.hed
BAKAPIFLAGS=-fh=bakapi.hed
SFLAGS=-sg -st -of+ -zu -zdf -zff -zgf -k55808#60000#32768
DFLAGS=-DTARGET_MSDOS=16 -DMSDOS=1 $(SFLAGS)
ZFLAGS=-zk0 -zc -zp8 $(WCLQ) ## -zm
CFLAGS=$(AFLAGS) $(IFLAGS)-lr -l=dos -wo##wwww
OFLAGS=-obmiler -out -oh -ei -zp8 -fpi87  -onac -ol+ -ok####x
FLAGS=$(CFLAGS) $(OFLAGS) $(DFLAGS) $(ZFLAGS)

PCX2VRL=$(DOSLIBDIR)/hw/vga/pcx2vrl

DOSLIBEXMMOBJ = himemsys.$(OBJ) emm.$(OBJ)
VGMSNDOBJ = vgmSnd.$(OBJ) 16_snd.$(OBJ)
DOSLIBOBJ = adlib.$(OBJ) 8254.$(OBJ) 8259.$(OBJ) dos.$(OBJ) cpu.$(OBJ)
16LIBOBJS = 16_in.$(OBJ) 16_mm.$(OBJ) wcpu.$(OBJ) 16_head.$(OBJ) 16_ca.$(OBJ) kitten.$(OBJ) 16_hc.$(OBJ) 16_timer.$(OBJ)

GFXLIBOBJS = modex16.$(OBJ) bitmap.$(OBJ) planar.$(OBJ) 16text.$(OBJ) bakapee.$(OBJ) scroll16.$(OBJ) 16render.$(OBJ) 16planar.$(OBJ)

DOSLIBLIBS=dl_vga.lib dl_cpu.lib dl_dos.lib

TESTEXEC = exmmtest.exe test.exe pcxtest.exe pcxtest2.exe test2.exe palettec.exe maptest.exe fmemtest.exe fonttest.exe fontgfx.exe scroll.exe vgmtest.exe inputest.exe palettel.exe planrpcx.exe
# tsthimem.exe
#testemm.exe testemm0.exe fonttes0.exe miditest.exe sega.exe sountest.exe
EXEC = 16.exe bakapi.exe $(TESTEXEC) tesuto.exe

!include $(DOSLIBDIR)/extdep.mak

all: $(EXEC) datatest

cute.vrl: data/cute.pcx $(PCX2VRL)
	$(PCX2VRL) -i data/cute.pcx -o cute.vrl -p cute.pal -tc 254
	# you can then test this against TESUTO.EXE by typing: TESUTO CUTE.VRL CUTE.PAL

datatest: cute.vrl .symbolic

# doslib PCX to VRL converter native host binary
$(PCX2VRL): $(DOSLIBDIR)/hw/vga/pcx2vrl.c
	cd $(DOSLIBDIR)/hw/vga && make pcx2vrl

#$(16LIBOBJS) => 16.lib bug....

#
#game and bakapi executables
#
16.exe: 16.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) $(16LIBOBJS) gfx.lib
	wcl $(FLAGS) $(16FLAGS) 16.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) $(16LIBOBJS) gfx.lib -fm=16.map

bakapi.exe: bakapi.$(OBJ) gfx.lib modex.lib
	wcl $(FLAGS) $(BAKAPIFLAGS) bakapi.$(OBJ) gfx.lib modex.lib -fm=bakapi.map
#
#Test Executables!
#
scroll.exe: scroll.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) $(16LIBOBJS) gfx.lib
	wcl $(FLAGS) scroll.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) $(16LIBOBJS) gfx.lib -fm=scroll.map
scroll.$(OBJ): $(SRC)scroll.c
	wcl $(FLAGS) -c $(SRC)scroll.c


# NOTE: dos86h = 16-bit huge memory model. memory model must match!
tesuto.exe: tesuto.$(OBJ) $(DOSLIBLIBS) 16_head.$(OBJ)
#	%write tmp.cmd option quiet option map=tesuto.map $(DOSLIB_LDFLAGS_DOS16H) file tesuto.obj name tesuto.exe
#	%write tmp.cmd library $(DOSLIBDIR)/hw/cpu/dos86h/cpu.lib
#	%write tmp.cmd library $(DOSLIBDIR)/hw/dos/dos86h/dos.lib
#	@wlink @tmp.cmd
	wcl $(FLAGS) $(WCLQ) tesuto.$(OBJ) $(DOSLIBLIBS) 16_head.$(OBJ)
tesuto.$(OBJ): $(SRC)tesuto.c
	wcl $(FLAGS) $(DOSLIB_CINCLUDE) $(WCLQ) -c $(SRC)tesuto.c
#tesuto.exe: tesuto.$(OBJ)
#	wcl $(WCLQ) -mh -d2 tesuto.$(OBJ)
#tesuto.$(OBJ): $(SRC)tesuto.c
#	wcl $(WCLQ) -mh -d2 -c $(SRC)tesuto.c

#sega.exe: sega.$(OBJ)
#	wcl $(FLAGS) sega.$(OBJ)
#sega.$(OBJ): $(SRC)sega.c
#	wcl $(FLAGS) -c $(SRC)sega.c

test.exe: test.$(OBJ) gfx.lib 16_in.$(OBJ) 16_head.$(OBJ)
	wcl $(FLAGS) test.$(OBJ) gfx.lib 16_in.$(OBJ) 16_head.$(OBJ) -fm=test.map

test2.exe: test2.$(OBJ) gfx.lib
	wcl $(FLAGS) test2.$(OBJ) gfx.lib -fm=test2.map

fonttest.exe: fonttest.$(OBJ) $(16LIBOBJS) gfx.lib
	wcl $(FLAGS) fonttest.$(OBJ) $(16LIBOBJS) gfx.lib -fm=fonttest.map

#fonttes0.exe: fonttes0.$(OBJ) $(16LIBOBJS)
#	wcl $(FLAGS) fonttes0.$(OBJ) $(16LIBOBJS)

fontgfx.exe: fontgfx.$(OBJ) $(16LIBOBJS) gfx.lib
	wcl $(FLAGS) fontgfx.$(OBJ) $(16LIBOBJS) gfx.lib -fm=fontgfx.map

inputest.exe: inputest.$(OBJ) $(16LIBOBJS)
	wcl $(FLAGS) inputest.$(OBJ) $(16LIBOBJS) -fm=inputest.map

#sountest.exe: sountest.$(OBJ) $(16LIBOBJS)
#	wcl $(FLAGS) sountest.$(OBJ) $(16LIBOBJS)

#miditest.exe: miditest.$(OBJ) $(16LIBOBJS) $(DOSLIBEXMMOBJ) midi.$(OBJ)
#	wcl $(FLAGS) miditest.$(OBJ) $(16LIBOBJS) $(DOSLIBEXMMOBJ) midi.$(OBJ)

tsthimem.exe: tsthimem.$(OBJ) $(16LIBOBJS) $(DOSLIBEXMMOBJ)
	wcl $(FLAGS) tsthimem.$(OBJ) $(16LIBOBJS) $(DOSLIBEXMMOBJ) -fm=tsthimem.map

#testemm.exe: testemm.$(OBJ) $(16LIBOBJS) $(DOSLIBEXMMOBJ)
#	wcl $(FLAGS) testemm.$(OBJ) $(16LIBOBJS) $(DOSLIBEXMMOBJ)

#testemm0.exe: testemm0.$(OBJ) $(16LIBOBJS) $(DOSLIBEXMMOBJ)
#	wcl $(FLAGS) testemm0.$(OBJ) $(16LIBOBJS) $(DOSLIBEXMMOBJ)

pcxtest.exe: pcxtest.$(OBJ) gfx.lib
	wcl $(FLAGS) pcxtest.$(OBJ) gfx.lib -fm=pcxtest.map

palettec.exe: palettec.$(OBJ) gfx.lib #$(16LIBOBJS)
	wcl $(FLAGS) palettec.$(OBJ) gfx.lib -fm=palettec.map #$(16LIBOBJS)

palettel.exe: palettel.$(OBJ) gfx.lib #$(16LIBOBJS)
	wcl $(FLAGS) palettel.$(OBJ) gfx.lib -fm=palettel.map #$(16LIBOBJS)

pcxtest2.exe: pcxtest2.$(OBJ) gfx.lib
	wcl $(FLAGS) pcxtest2.$(OBJ) gfx.lib -fm=pcxtest2.map

planrpcx.exe: planrpcx.$(OBJ) gfx.lib
	wcl $(FLAGS) planrpcx.$(OBJ) gfx.lib -fm=planrpcx.map

maptest.exe: maptest.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) $(16LIBOBJS) gfx.lib
	wcl $(FLAGS) maptest.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) $(16LIBOBJS) gfx.lib -fm=maptest.map

#maptest0.exe: maptest0.$(OBJ) fmapread.$(OBJ) farjsmn.$(OBJ)
#	wcl $(FLAGS) $(MFLAGS) maptest0.$(OBJ) fmapread.$(OBJ) farjsmn.$(OBJ)

#emmtest.exe: emmtest.$(OBJ) memory.$(OBJ)
#	wcl $(FLAGS) $(MFLAGS) emmtest.$(OBJ) memory.$(OBJ)

#emsdump.exe: emsdump.$(OBJ) memory.$(OBJ)
#	wcl $(FLAGS) $(MFLAGS) emsdump.$(OBJ) memory.$(OBJ)

fmemtest.exe: fmemtest.$(OBJ) $(16LIBOBJS)
	wcl $(FLAGS) fmemtest.$(OBJ) $(16LIBOBJS) -fm=fmemtest.map

exmmtest.exe: exmmtest.$(OBJ) $(16LIBOBJS)
	wcl $(FLAGS) exmmtest.$(OBJ) -fm=exmmtest.map $(16LIBOBJS)

vgmtest.exe: vgmtest.$(OBJ) vgmsnd.lib $(16LIBOBJS)
	wcl $(FLAGS) vgmtest.$(OBJ) vgmsnd.lib -fm=vgmtest.map $(16LIBOBJS)
	#====wcl -mc vgmtest.$(OBJ) $(VGMSNDOBJ) -fm=vgmtest.map


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
	wcl $(FLAGS) -c $(SRC)exmmtest.c

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

doslib.lib: $(DOSLIBOBJ) # $(SRCLIB)cpu.lib
	wlib -b $(WLIBQ) doslib.lib $(DOSLIBOBJ) # $(SRCLIB)cpu.lib

vgmsnd.lib: $(VGMSNDOBJ)
	wlib -b $(WLIBQ) vgmsnd.lib $(VGMSNDOBJ)


# library deps 16-bit huge
dl_vga.lib:
	cd $(DOSLIBDIR)/hw/vga && ./make.sh
	cp $(DOSLIBDIR)/hw/vga/dos86h/vga.lib dl_vga.lib

dl_cpu.lib:
	cd $(DOSLIBDIR)/hw/cpu && ./make.sh
	cp $(DOSLIBDIR)/hw/cpu/dos86h/cpu.lib dl_cpu.lib

dl_dos.lib:
	cd $(DOSLIBDIR)/hw/dos && ./make.sh
	cp $(DOSLIBDIR)/hw/dos/dos86h/dos.lib dl_dos.lib

modex16.$(OBJ): $(SRCLIB)modex16.h $(SRCLIB)modex16.c
	wcl $(FLAGS) -c $(SRCLIB)modex16.c

bakapee.$(OBJ): $(SRCLIB)bakapee.h $(SRCLIB)bakapee.c
	wcl $(FLAGS) -c $(SRCLIB)bakapee.c

16render.$(OBJ): $(MODEXLIB)16render.h $(MODEXLIB)16render.c
	wcl $(FLAGS) -c $(MODEXLIB)16render.c

16planar.$(OBJ): $(MODEXLIB)16planar.h $(MODEXLIB)16planar.c
	wcl $(FLAGS) -c $(MODEXLIB)16planar.c

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

midi.$(OBJ): $(SRCLIB)midi.h $(SRCLIB)midi.c
	wcl $(FLAGS) -c $(SRCLIB)midi.c

#
# doslib stuff
#
adlib.$(OBJ): $(DOSLIB)adlib.h $(DOSLIB)adlib.c
	wcl $(FLAGS) -c $(DOSLIB)adlib.c

8254.$(OBJ): $(DOSLIB)8254.h $(DOSLIB)8254.c
	wcl $(FLAGS) -c $(DOSLIB)8254.c

8259.$(OBJ): $(DOSLIB)8259.h $(DOSLIB)8259.c
	wcl $(FLAGS) -c $(DOSLIB)8259.c

dos.$(OBJ): $(DOSLIB)dos.h $(DOSLIB)dos.c
	wcl $(FLAGS) -c $(DOSLIB)dos.c

cpu.$(OBJ): $(DOSLIB)cpu.h $(DOSLIB)cpu.c
	wcl $(FLAGS) -c $(DOSLIB)cpu.c

himemsys.$(OBJ): $(DOSLIB)himemsys.h $(DOSLIB)himemsys.c
	wcl $(FLAGS) -c $(DOSLIB)himemsys.c

emm.$(OBJ): $(DOSLIB)emm.h $(DOSLIB)emm.c
	wcl $(FLAGS) -c $(DOSLIB)emm.c

# end of doslib stuff

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

#3812intf.$(OBJ): $(VGMSNDLIB)3812intf.h $(VGMSNDLIB)3812intf.c
#	wcl $(FLAGS) -c $(VGMSNDLIB)3812intf.c

#farjsmn.$(OBJ): $(JSMNLIB)farjsmn.h $(JSMNLIB)farjsmn.c
#	wcl $(FLAGS) $(MFLAGS) -c $(JSMNLIB)farjsmn.c

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
	@$(REMOVECOMMAND) *.$(OBJ)
	@$(REMOVECOMMAND) 16.lib
	@$(REMOVECOMMAND) gfx.lib
	@$(REMOVECOMMAND) vgmsnd.lib
	@$(REMOVECOMMAND) $(DOSLIBLIBS)
	@wlib -n $(WLIBQ) 16.lib
	@wlib -n $(WLIBQ) gfx.lib
	@wlib -n $(WLIBQ) vgmsnd.lib
	@$(REMOVECOMMAND) dl_cpu.lib
	@$(REMOVECOMMAND) dl_dos.lib
	@$(REMOVECOMMAND) dl_vga.lib
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
	@$(REMOVECOMMAND) *.MAP
	@$(REMOVECOMMAND) *.map
	@$(REMOVECOMMAND) *.err
	@$(COPYCOMMAND) .git/config git_con.fig
#	@$(COPYCOMMAND) $(SRC)exmmtest.c $(EXMMTESTDIR)$(SRC)
#	@$(COPYCOMMAND) $(SRCLIB)16_mm.* $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(SRCLIB)16_head.* $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(SRCLIB)16_ca.* $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(SRCLIB)16_hc.* $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(SRCLIB)types.h $(EXMMTESTDIR)$(SRCLIB)
#	@$(COPYCOMMAND) $(NYANLIB)* $(EXMMTESTDIR)$(NYANLIB)
#	@echo $(watcom)
#	@echo $(INCLUDE)

comp: .symbolic
	@upx -9 $(EXEC)

comq: .symbolic
	@upx -9 $(UPXQ) $(EXEC)

www: .symbolic
#       @rm /var/www/$(EXEC)
	@cp ./$(EXEC) /var/www/
	@./z.sh $(EXEC) $(EXEC)
	@scp -r -P 26 *.exe 4ch.mooo.com:/var/www/16/
	@scp -r -P 26 /var/www/*.exe.zip.* 4ch.mooo.com:/var/www/16/

getwww: .symbolic
	@x4get.bat $(EXEC)

#git submodule add <repo>
uplibs: .symbolic
	@wmake -h updatelibs

updatelibs: .symbolic
	@cd $(JSMNLIB)
	@git pull
	@cd $(PDIR)$(PDIR)$(PDIR)
	@cd $(DOSLIB)
	@git pull
	@./buildall.sh
	@cd $(PDIR)$(PDIR)$(PDIR)

reinitlibs: .symbolic
	@rm -rf $(SRCLIB)doslib
	@rm -rf $(SRCLIB)jsmn
	@mkdir $(SRCLIB)doslib
	@mkdir $(SRCLIB)jsmn
	@wmake -h initlibs

initlibs: .symbolic
	@cd $(SRCLIB)
	@git clone https://github.com/joncampbell123/doslib.git
	@git clone https://github.com/zserge/jsmn.git
	@cd $(PDIR)$(PDIR)

##
##	experimental libs
##
xlib: .symbolic
	@cd 16/xlib
	@wmake -h clean
	@wmake -h all
	@cd $(PDIR)$(PDIR)

mx: .symbolic
	@cd 16/xw
#	@wmake clean
	@wmake -h all
	@cd $(PDIR)$(PDIR)

mx_: .symbolic
	@cd 16/xw_
	@wmake -h -f makefile all
	@cd $(PDIR)$(PDIR)
