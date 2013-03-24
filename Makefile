OBJS=main.obj timer.obj eng_ai.obj eng_snd.obj eng_comm.obj &
     eng_core.obj eng_gfx.obj dos_comm.obj dos_gfx.obj &
     dos_opl2.obj lib_com.obj

CC=*wcc
CFLAGS=-i"C:\FDOS\WATCOM/H" -w4 -e25 -zkl -zq -otexan -ol -ol+ &
       -oh -om -d2 -bt=dos -ml

CXX=*wpp
CXXFLAGS=-i="C:\FDOS\WATCOM/H" -w4 -e25 -zkl -zq -otexan -ol -ol+ &
         -oh -om -d2 -bt=dos -ml -xs -xr

LD=*wlink
LDFLAGS=d all sys dos op m op maxe=25 op q op symf 

all : core16.exe

clean : .symbolic 
	@del *.obj
	@del core16.exe
	@del *.err
	@del *.smp

core16.exe : $(OBJS)
	$(LD) $(LDFLAGS) name core16 file {$(OBJS)} 

.c : src/;src/lib
.cpp : src/;src/lib

.cpp.obj : .autodepend
	$(CXX) $< -fo=$@ $(CXXFLAGS)
.c.obj : .autodepend
	$(CC) $< -fo=$@ $(CFLAGS)
$(OBJS) :