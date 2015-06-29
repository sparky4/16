#-zk0u = translate kanji to unicode... wwww
#-zk0 = kanji support~
#-zkl = current codepage

MFLAGS=-mc# -zm
CFLAGS=-zkl -wo -x#### -mc# -zdp# -zp16 -zq
OFLAGS=-ot -ox -ob -oh -or# -om -ol -ol+
FLAGS=-0 -d2 -lr $(OFLAGS) $(CFLAGS)
REMOVECOMMAND=del
DIRSEP=\
OBJ=obj
SRC=src$(DIRSEP)
SRCLIB=$(SRC)lib$(DIRSEP)
JSMNLIB=$(SRCLIB)jsmn$(DIRSEP)
EXMMLIB=$(SRCLIB)exmm$(DIRSEP)
WCPULIB=$(SRCLIB)wcpu$(DIRSEP)

16LIBOBJS = 16_in.$(OBJ) wcpu.$(OBJ) lib_head.$(OBJ) scroll16.$(OBJ) 16text.$(OBJ)
GFXLIBOBJS = modex16.$(OBJ) bitmap.$(OBJ) planar.$(OBJ)

all: 16.exe test.exe pcxtest.exe test2.exe palettec.exe maptest.exe maptest0.exe emsdump.exe emmtest.exe fmemtest.exe fonttest.exe inputest.exe exmmtest.exe

#
#executables
#
16.exe: 16.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) 16.lib
	wcl $(FLAGS) 16.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) 16.lib

scroll.exe: scroll.$(OBJ) 16.lib mapread.$(OBJ) jsmn.$(OBJ) dos_kb.$(OBJ)
	wcl $(FLAGS) scroll.$(OBJ) 16.lib mapread.$(OBJ) jsmn.$(OBJ) dos_kb.$(OBJ)
scroll.$(OBJ): $(SRC)scroll.c
	wcl $(FLAGS) -c $(SRC)scroll.c
test.exe: test.$(OBJ) 16.lib
	wcl $(FLAGS) test.$(OBJ) 16.lib

test2.exe: test2.$(OBJ) 16.lib
	wcl $(FLAGS) test2.$(OBJ) 16.lib

fonttest.exe: fonttest.$(OBJ) 16.lib
	wcl $(FLAGS) fonttest.$(OBJ) 16.lib

inputest.exe: inputest.$(OBJ) 16.lib
	wcl $(FLAGS) inputest.$(OBJ) 16.lib

pcxtest.exe: pcxtest.$(OBJ) 16.lib
	wcl $(FLAGS) pcxtest.$(OBJ) 16.lib

palettec.exe: palettec.$(OBJ) modex16.$(OBJ)
	wcl $(FLAGS) palettec.$(OBJ) modex16.$(OBJ)

maptest.exe: maptest.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) 16.lib
	wcl $(FLAGS) maptest.$(OBJ) mapread.$(OBJ) jsmn.$(OBJ) 16.lib

maptest0.exe: maptest0.$(OBJ) fmapread.$(OBJ) farjsmn.$(OBJ)# 16.lib
	wcl $(FLAGS) $(MFLAGS) maptest0.$(OBJ) fmapread.$(OBJ) farjsmn.$(OBJ)# 16.lib

emmtest.exe: emmtest.$(OBJ) memory.$(OBJ)
	wcl $(FLAGS) $(MFLAGS) emmtest.$(OBJ) memory.$(OBJ)

emsdump.exe: emsdump.$(OBJ) memory.$(OBJ)
	wcl $(FLAGS) $(MFLAGS) emsdump.$(OBJ) memory.$(OBJ)

fmemtest.exe: fmemtest.$(OBJ)# memory.$(OBJ)
	wcl $(FLAGS) $(MFLAGS) fmemtest.$(OBJ)# memory.$(OBJ)

exmmtest.exe: exmmtest.$(OBJ) 16_mm.$(OBJ)
        wcl $(FLAGS) $(MFLAGS) exmmtest.$(OBJ) 16_mm.$(OBJ)

#
#executable's objects
#
16.$(OBJ): $(SRC)16.h $(SRC)16.c
	wcl $(FLAGS) -c $(SRC)16.c

test.$(OBJ): $(SRC)test.c $(SRCLIB)modex16.h
	wcl $(FLAGS) -c $(SRC)test.c

test2.$(OBJ): $(SRC)test2.c $(SRCLIB)modex16.h
	wcl $(FLAGS) -c $(SRC)test2.c

pcxtest.$(OBJ): $(SRC)pcxtest.c $(SRCLIB)modex16.h
	wcl $(FLAGS) -c $(SRC)pcxtest.c

palettec.$(OBJ): $(SRC)palettec.c
	wcl $(FLAGS) -c $(SRC)palettec.c

maptest.$(OBJ): $(SRC)maptest.c $(SRCLIB)modex16.h
	wcl $(FLAGS) -c $(SRC)maptest.c

maptest0.$(OBJ): $(SRC)maptest0.c# $(SRCLIB)modex16.h
	wcl $(FLAGS) $(MFLAGS) -c $(SRC)maptest0.c

emmtest.$(OBJ): $(SRC)emmtest.c
	wcl $(FLAGS) $(MFLAGS) -c $(SRC)emmtest.c

emsdump.$(OBJ): $(SRC)emsdump.c
	wcl $(FLAGS) $(MFLAGS) -c $(SRC)emsdump.c

fmemtest.$(OBJ): $(SRC)fmemtest.c
	wcl $(FLAGS) $(MFLAGS) -c $(SRC)fmemtest.c

fonttest.$(OBJ): $(SRC)fonttest.c
	wcl -c $(SRC)fonttest.c

inputest.$(OBJ): $(SRC)inputest.c
	wcl $(FLAGS) -c $(SRC)inputest.c

exmmtest.$(OBJ): $(SRC)exmmtest.c
	wcl $(FLAGS) $(MFLAGS) -c $(SRC)exmmtest.c

#
#non executable objects libraries
#
16.lib: $(16LIBOBJS) gfx.lib
	wlib -b 16.lib $(16LIBOBJS) gfx.lib

gfx.lib: $(GFXLIBOBJS)
	wlib -b gfx.lib $(GFXLIBOBJS)

modex16.$(OBJ): $(SRCLIB)modex16.h $(SRCLIB)modex16.c
	wcl $(FLAGS) -c $(SRCLIB)modex16.c

dos_kb.$(OBJ): $(SRCLIB)dos_kb.h $(SRCLIB)dos_kb.c
	wcl $(FLAGS) -c $(SRCLIB)dos_kb.c

bitmap.$(OBJ): $(SRCLIB)bitmap.h $(SRCLIB)bitmap.c
	wcl $(FLAGS) -c $(SRCLIB)bitmap.c

planar.$(OBJ): $(SRCLIB)planar.h $(SRCLIB)planar.c
	wcl $(FLAGS) -c $(SRCLIB)planar.c

scroll16.$(OBJ): $(SRCLIB)scroll16.h $(SRCLIB)scroll16.c
	wcl $(FLAGS) -c $(SRCLIB)scroll16.c

wcpu.$(OBJ): $(WCPULIB)wcpu.h $(WCPULIB)wcpu.c
	wcl $(FLAGS) -c $(WCPULIB)wcpu.c

16text.$(OBJ): $(SRCLIB)16text.c
	wcl -c $(SRCLIB)16text.c

mapread.$(OBJ): $(SRCLIB)mapread.h $(SRCLIB)mapread.c 16.lib
	wcl $(FLAGS) -c $(SRCLIB)mapread.c 16.lib

fmapread.$(OBJ): $(SRCLIB)fmapread.h $(SRCLIB)fmapread.c 16.lib
	wcl $(FLAGS) $(MFLAGS) -c $(SRCLIB)fmapread.c 16.lib

16_in.$(OBJ): $(SRCLIB)16_in.h $(SRCLIB)16_in.c
	wcl $(FLAGS) -c $(SRCLIB)16_in.c

16_mm.$(OBJ): $(SRCLIB)16_mm.h $(SRCLIB)16_mm.c
	wcl $(FLAGS) $(MFLAGS) -c $(SRCLIB)16_mm.c

lib_head.$(OBJ): $(SRCLIB)lib_head.h $(SRCLIB)lib_head.c
	wcl $(FLAGS) -c $(SRCLIB)lib_head.c

jsmn.$(OBJ): $(JSMNLIB)jsmn.h $(JSMNLIB)jsmn.c
	wcl $(FLAGS) -c $(JSMNLIB)jsmn.c

farjsmn.$(OBJ): $(JSMNLIB)farjsmn.h $(JSMNLIB)farjsmn.c
	wcl $(FLAGS) $(MFLAGS) -c $(JSMNLIB)farjsmn.c

memory.$(OBJ): $(EXMMLIB)memory.h $(EXMMLIB)memory.c
	wcl $(FLAGS) $(MFLAGS) -c $(EXMMLIB)memory.c

#
#other~
#
clean: .symbolic
	@$(REMOVECOMMAND) *.$(OBJ)
#	@$(REMOVECOMMAND) *.OBJ
#	@$(REMOVECOMMAND) *.out
#	@$(REMOVECOMMAND) *.OUT
	@$(REMOVECOMMAND) makefi~1
	@$(REMOVECOMMAND) __WCL__.LNK
#	@$(REMOVECOMMAND) *.smp
	@$(REMOVECOMMAND) *.SMP
