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

TARGET_OS = dos

DISS=wdis -a
REDR=">" ..$(DIRSEP)xw_$(DIRSEP)

all:
	@beep

pee: .symbolic
$(DISS) MXBB.OBJ $(REDR)mxbb.asm
$(DISS) MXCC.OBJ $(REDR)mxcc.asm
$(DISS) MXCG.OBJ $(REDR)mxcg.asm
$(DISS) MXCL.OBJ $(REDR)mxcl.asm
$(DISS) MXCR.OBJ $(REDR)mxcl.asm
$(DISS) MXFB.OBJ $(REDR)mxfb.asm
$(DISS) MXFP.OBJ $(REDR)mxfp.asm
$(DISS) MXGC.OBJ $(REDR)mxgc.asm
$(DISS) MXGI.OBJ $(REDR)mxgi.asm
$(DISS) MXGM.OBJ $(REDR)mxgm.asm
$(DISS) MXGP.OBJ $(REDR)mxgp.asm
$(DISS) MXGV.OBJ $(REDR)mxgv.asm
$(DISS) MXHL.OBJ $(REDR)mxhl.asm
$(DISS) MXIT.OBJ $(REDR)mxit.asm
$(DISS) MXLL.OBJ $(REDR)mxll.asm
$(DISS) MXLN.OBJ $(REDR)mxln.asm
$(DISS) MXOT.OBJ $(REDR)mxot.asm
$(DISS) MXPB.OBJ $(REDR)mxpb.asm
$(DISS) MXPF.OBJ $(REDR)mxpf.asm
$(DISS) MXPG.OBJ $(REDR)mxpg.asm
$(DISS) MXPI.OBJ $(REDR)mxpi.asm
$(DISS) MXPN.OBJ $(REDR)mxpn.asm
$(DISS) MXPP.OBJ $(REDR)mxpp.asm
$(DISS) MXRA.OBJ $(REDR)mxra.asm
$(DISS) MXRP.OBJ $(REDR)mxrp.asm
$(DISS) MXSA.OBJ $(REDR)mxsa.asm
$(DISS) MXSC.OBJ $(REDR)mxsc.asm
$(DISS) MXSI.OBJ $(REDR)mxsi.asm
$(DISS) MXSL.OBJ $(REDR)mxsl.asm
$(DISS) MXSM.OBJ $(REDR)mxsm.asm
$(DISS) MXSP.OBJ $(REDR)mxsp.asm
$(DISS) MXSS.OBJ $(REDR)mxss.asm
$(DISS) MXTL.OBJ $(REDR)mxtl.asm
$(DISS) MXVS.OBJ $(REDR)mxvs.asm
$(DISS) MXWD.OBJ $(REDR)mxwd.asm
$(DISS) MXWM.OBJ $(REDR)mxwm.asm
$(DISS) MXWP.OBJ $(REDR)mxwp.asm
$(DISS) MXWR.OBJ $(REDR)mxwr.asm
