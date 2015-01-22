# Makefile
.autodepend

############################################## Main file name
NAME = roads
############################################## Object files
OBJ1 = anim.obj bookends.obj initroad.obj fx.obj
OBJ= $(OBJ1) roads.obj initw.obj tileio.obj
############################################## Memory Model (ex: s for small)
MODEL = s
############################################## Supplementary dependencies
SUP =
############################################## Path to headers
INCPATH = c:\borlandc\include
############################################## Path to libraries
LIBPATH = c:\borlandc\lib
############################################## Libraries to search
LIBS = c$(MODEL) fg$(MODEL)
############################# emu math$(MODEL)
############################################## Compilation Flags
COMPFLAGS = -Z -G -O2 -c -H -m$(MODEL) -I$(INCPATH)
############################################## Linking Flags
LINKFLAGS = -x -L$(LIBPATH)


#######
# Implicit Definitions -- compilation
#######
.c.obj:
        BCC $(COMPFLAGS) {$< }


#######
# Explicit Definitions -- linking
#######
$(NAME).exe: $(OBJ) $(SUP)
        TLINK $(LINKFLAGS) @&&!
c0$(MODEL) $(OBJ)
$(NAME)

$(LIBS)
!
