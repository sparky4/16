# Makefile for building Lua
# See ../doc/readme.html for installation and customization instructions.

# == CHANGE THE SETTINGS BELOW TO SUIT YOUR ENVIRONMENT =======================

# Your platform. See PLATS for possible values.
PLAT= none

CC= wcc
CFLAGS= -0 -d2 -DLUA_COMPAT_ALL $(SYSCFLAGS) $(MYCFLAGS)
#CFLAGS="-ml -DLUA_ANSI" LIBS= RANLIB=echo MYLDFLAGS=-ml
LDFLAGS= $(SYSLDFLAGS) $(MYLDFLAGS)
LIBS= -lm $(SYSLIBS) $(MYLIBS)

AR= ar rcu
RANLIB= ranlib
RM= rm -f

SYSCFLAGS=
SYSLDFLAGS=
SYSLIBS=

MYCFLAGS=
MYLDFLAGS=
MYLIBS=
MYOBJS=

# == END OF USER SETTINGS -- NO NEED TO CHANGE ANYTHING BELOW THIS LINE =======

PLATS= aix ansi bsd freebsd generic linux macosx mingw posix solaris

LUA_A=	liblua.a
CORE_O=	lapi.obj lcode.obj lctype.obj ldebug.obj ldo.obj ldump.obj lfunc.obj lgc.obj llex.obj lmem.obj lobject.obj lopcodes.obj lparser.obj lstate.obj lstring.obj ltable.obj ltm.obj lundump.obj lvm.obj lzio.o
LIB_O=	lauxlib.obj lbaselib.obj lbitlib.obj lcorolib.obj ldblib.obj liolib.obj lmathlib.obj loslib.obj lstrlib.obj ltablib.obj loadlib.obj linit.o
BASE_O= $(CORE_O) $(LIB_O) $(MYOBJS)

LUA_T=	lua
LUA_O=	lua.obj

LUAC_T=	luac
LUAC_O=	luac.obj

ALL_O= $(BASE_O) $(LUA_O) $(LUAC_O)
ALL_T= $(LUA_A) $(LUA_T) $(LUAC_T)
ALL_A= $(LUA_A)

# Targets start here.
default: $(PLAT)

all:	$(ALL_T)

o:	$(ALL_O)

a:	$(ALL_A)

$(LUA_A): $(BASE_O)
	$(AR) $@ $(BASE_O)
	$(RANLIB) $@

$(LUA_T): $(LUA_O) $(LUA_A)
	$(CC) -o $@ $(LDFLAGS) $(LUA_O) $(LUA_A) $(LIBS)

$(LUAC_T): $(LUAC_O) $(LUA_A)
	$(CC) -o $@ $(LDFLAGS) $(LUAC_O) $(LUA_A) $(LIBS)

clean:
	$(RM) $(ALL_T) $(ALL_O)

depend:
	@$(CC) $(CFLAGS) -MM l*.c

echo:
	@echo "PLAT= $(PLAT)"
	@echo "CC= $(CC)"
	@echo "CFLAGS= $(CFLAGS)"
	@echo "LDFLAGS= $(SYSLDFLAGS)"
	@echo "LIBS= $(LIBS)"
	@echo "AR= $(AR)"
	@echo "RANLIB= $(RANLIB)"
	@echo "RM= $(RM)"

# Convenience targets for popular platforms
ALL= all

none:
	@echo "Please do 'make PLATFORM' where PLATFORM is one of these:"
	@echo "   $(PLATS)"

aix:
	$(MAKE) $(ALL) CC="xlc" CFLAGS="-O2 -DLUA_USE_POSIX -DLUA_USE_DLOPEN" SYSLIBS="-ldl" SYSLDFLAGS="-brtl -bexpall"

ansi:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_ANSI"

bsd:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_POSIX -DLUA_USE_DLOPEN" SYSLIBS="-Wl,-E"

freebsd:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_LINUX" SYSLIBS="-Wl,-E -lreadline"

generic: $(ALL)

linux:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_LINUX" SYSLIBS="-Wl,-E -ldl -lreadline"

macosx:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_MACOSX" SYSLIBS="-lreadline" CC=cc

mingw:
	$(MAKE) "LUA_A=lua52.dll" "LUA_T=lua.exe" "AR=$(CC) -shared -o" "RANLIB=strip --strip-unneeded" "SYSCFLAGS=-DLUA_BUILD_AS_DLL" "SYSLIBS=" "SYSLDFLAGS=-s" lua.exe
	$(MAKE) "LUAC_T=luac.exe" luac.exe

posix:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_POSIX"

solaris:
	$(MAKE) $(ALL) SYSCFLAGS="-DLUA_USE_POSIX -DLUA_USE_DLOPEN" SYSLIBS="-ldl"

# list targets that do not create files (but not all makes understand .PHONY)
#.PHONY: all $(PLATS) default o a clean depend echo none

# DO NOT DELETE

lapi.obj: lapi.c lua.h luaconf.h lapi.h llimits.h lstate.h lobject.h ltm.h lzio.h lmem.h ldebug.h ldo.h lfunc.h lgc.h lstring.h ltable.h lundump.h lvm.h
lauxlib.obj: lauxlib.c lua.h luaconf.h lauxlib.h
lbaselib.obj: lbaselib.c lua.h luaconf.h lauxlib.h lualib.h
lbitlib.obj: lbitlib.c lua.h luaconf.h lauxlib.h lualib.h
lcode.obj: lcode.c lua.h luaconf.h lcode.h llex.h lobject.h llimits.h lzio.h lmem.h lopcodes.h lparser.h ldebug.h lstate.h ltm.h ldo.h lgc.h lstring.h ltable.h lvm.h
lcorolib.obj: lcorolib.c lua.h luaconf.h lauxlib.h lualib.h
lctype.obj: lctype.c lctype.h lua.h luaconf.h llimits.h
ldblib.obj: ldblib.c lua.h luaconf.h lauxlib.h lualib.h
ldebug.obj: ldebug.c lua.h luaconf.h lapi.h llimits.h lstate.h lobject.h ltm.h lzio.h lmem.h lcode.h llex.h lopcodes.h lparser.h ldebug.h ldo.h lfunc.h lstring.h lgc.h ltable.h lvm.h
ldo.obj: ldo.c lua.h luaconf.h lapi.h llimits.h lstate.h lobject.h ltm.h lzio.h lmem.h ldebug.h ldo.h lfunc.h lgc.h lopcodes.h lparser.h lstring.h ltable.h lundump.h lvm.h
ldump.obj: ldump.c lua.h luaconf.h lobject.h llimits.h lstate.h ltm.h lzio.h lmem.h lundump.h
lfunc.obj: lfunc.c lua.h luaconf.h lfunc.h lobject.h llimits.h lgc.h lstate.h ltm.h lzio.h lmem.h
lgc.obj: lgc.c lua.h luaconf.h ldebug.h lstate.h lobject.h llimits.h ltm.h lzio.h lmem.h ldo.h lfunc.h lgc.h lstring.h ltable.h
linit.obj: linit.c lua.h luaconf.h lualib.h lauxlib.h
liolib.obj: liolib.c lua.h luaconf.h lauxlib.h lualib.h
llex.obj: llex.c lua.h luaconf.h lctype.h llimits.h ldo.h lobject.h lstate.h ltm.h lzio.h lmem.h llex.h lparser.h lstring.h lgc.h ltable.h
lmathlib.obj: lmathlib.c lua.h luaconf.h lauxlib.h lualib.h
lmem.obj: lmem.c lua.h luaconf.h ldebug.h lstate.h lobject.h llimits.h ltm.h lzio.h lmem.h ldo.h lgc.h
loadlib.obj: loadlib.c lua.h luaconf.h lauxlib.h lualib.h
lobject.obj: lobject.c lua.h luaconf.h lctype.h llimits.h ldebug.h lstate.h lobject.h ltm.h lzio.h lmem.h ldo.h lstring.h lgc.h lvm.h
lopcodes.obj: lopcodes.c lopcodes.h llimits.h lua.h luaconf.h
loslib.obj: loslib.c lua.h luaconf.h lauxlib.h lualib.h
lparser.obj: lparser.c lua.h luaconf.h lcode.h llex.h lobject.h llimits.h lzio.h lmem.h lopcodes.h lparser.h ldebug.h lstate.h ltm.h ldo.h lfunc.h lstring.h lgc.h ltable.h
lstate.obj: lstate.c lua.h luaconf.h lapi.h llimits.h lstate.h lobject.h ltm.h lzio.h lmem.h ldebug.h ldo.h lfunc.h lgc.h llex.h lstring.h ltable.h
lstring.obj: lstring.c lua.h luaconf.h lmem.h llimits.h lobject.h lstate.h ltm.h lzio.h lstring.h lgc.h
lstrlib.obj: lstrlib.c lua.h luaconf.h lauxlib.h lualib.h
ltable.obj: ltable.c lua.h luaconf.h ldebug.h lstate.h lobject.h llimits.h ltm.h lzio.h lmem.h ldo.h lgc.h lstring.h ltable.h lvm.h
ltablib.obj: ltablib.c lua.h luaconf.h lauxlib.h lualib.h
ltm.obj: ltm.c lua.h luaconf.h lobject.h llimits.h lstate.h ltm.h lzio.h lmem.h lstring.h lgc.h ltable.h
lua.obj: lua.c lua.h luaconf.h lauxlib.h lualib.h
luac.obj: luac.c lua.h luaconf.h lauxlib.h lobject.h llimits.h lstate.h ltm.h lzio.h lmem.h lundump.h ldebug.h lopcodes.h
lundump.obj: lundump.c lua.h luaconf.h ldebug.h lstate.h lobject.h llimits.h ltm.h lzio.h lmem.h ldo.h lfunc.h lstring.h lgc.h lundump.h
lvm.obj: lvm.c lua.h luaconf.h ldebug.h lstate.h lobject.h llimits.h ltm.h lzio.h lmem.h ldo.h lfunc.h lgc.h lopcodes.h lstring.h ltable.h lvm.h
lzio.obj: lzio.c lua.h luaconf.h llimits.h lmem.h lstate.h lobject.h ltm.h lzio.h

