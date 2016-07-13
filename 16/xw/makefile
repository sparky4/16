#
# MODEX library makefile (for Borland MAKE)
# Copyright (c) 1993,1994 by Alessandro Scotti
#
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
LIBINCS = modex.def

LIBOBJS = mxbb.$(OBJ) mxcc.$(OBJ) mxcg.$(OBJ) mxcl.$(OBJ) mxcr.$(OBJ) mxfb.$(OBJ) mxfp.$(OBJ) mxgc.$(OBJ) mxgi.$(OBJ) mxgm.$(OBJ) mxgp.$(OBJ) mxgv.$(OBJ) mxhl.$(OBJ) mxit.$(OBJ) mxll.$(OBJ) mxln.$(OBJ) mxot.$(OBJ) mxpb.$(OBJ) mxpf.$(OBJ) mxpg.$(OBJ) mxpi.$(OBJ) mxpn.$(OBJ) mxpp.$(OBJ) mxra.$(OBJ) mxrp.$(OBJ) mxsa.$(OBJ) mxsc.$(OBJ) mxsi.$(OBJ) mxsl.$(OBJ) mxsm.$(OBJ) mxsp.$(OBJ) mxss.$(OBJ) mxtl.$(OBJ) mxvs.$(OBJ) mxwd.$(OBJ) mxwm.$(OBJ) mxwp.$(OBJ) mxwr.$(OBJ)

#
# ASM compiler
#
ASMC =wasm
ASMO =-mh -0

#
# PAS compiler
#
#PASC = tpc
#PASO = /m -$D- -$L- -$S-

#
# LIB maker, uses response file
#
LIBC = wlib

# .asm.obj:
# 		$(ASMC) $(ASMO) $<

mxbb.$(OBJ): mxbb.asm
	$(ASMC) $(ASMO) mxbb.asm
mxcc.$(OBJ): mxcc.asm
	$(ASMC) $(ASMO) mxcc.asm
mxcg.$(OBJ): mxcg.asm
	$(ASMC) $(ASMO) mxcg.asm
mxcl.$(OBJ): mxcl.asm
	$(ASMC) $(ASMO) mxcl.asm
mxcr.$(OBJ): mxcr.asm
	$(ASMC) $(ASMO) mxcr.asm
mxfb.$(OBJ): mxfb.asm
	$(ASMC) $(ASMO) mxfb.asm
mxfp.$(OBJ): mxfp.asm
	$(ASMC) $(ASMO) mxfp.asm
mxgc.$(OBJ): mxgc.asm
	$(ASMC) $(ASMO) mxgc.asm
mxgi.$(OBJ): mxgi.asm
	$(ASMC) $(ASMO) mxgi.asm
mxgm.$(OBJ): mxgm.asm
	$(ASMC) $(ASMO) mxgm.asm
mxgp.$(OBJ): mxgp.asm
	$(ASMC) $(ASMO) mxgp.asm
mxgv.$(OBJ): mxgv.asm
	$(ASMC) $(ASMO) mxgv.asm
mxhl.$(OBJ): mxhl.asm
	$(ASMC) $(ASMO) mxhl.asm
mxit.$(OBJ): mxit.asm
	$(ASMC) $(ASMO) mxit.asm
mxll.$(OBJ): mxll.asm
	$(ASMC) $(ASMO) mxll.asm
mxln.$(OBJ): mxln.asm
	$(ASMC) $(ASMO) mxln.asm
mxot.$(OBJ): mxot.asm
	$(ASMC) $(ASMO) mxot.asm
mxpb.$(OBJ): mxpb.asm
	$(ASMC) $(ASMO) mxpb.asm
mxpf.$(OBJ): mxpf.asm
	$(ASMC) $(ASMO) mxpf.asm
mxpg.$(OBJ): mxpg.asm
	$(ASMC) $(ASMO) mxpg.asm
mxpi.$(OBJ): mxpi.asm
	$(ASMC) $(ASMO) mxpi.asm
mxpn.$(OBJ): mxpn.asm
	$(ASMC) $(ASMO) mxpn.asm
mxpp.$(OBJ): mxpp.asm
	$(ASMC) $(ASMO) mxpp.asm
mxra.$(OBJ): mxra.asm
	$(ASMC) $(ASMO) mxra.asm
mxrp.$(OBJ): mxrp.asm
	$(ASMC) $(ASMO) mxrp.asm
mxsa.$(OBJ): mxsa.asm
	$(ASMC) $(ASMO) mxsa.asm
mxsc.$(OBJ): mxsc.asm
	$(ASMC) $(ASMO) mxsc.asm
mxsi.$(OBJ): mxsi.asm
	$(ASMC) $(ASMO) mxsi.asm
mxsl.$(OBJ): mxsl.asm
	$(ASMC) $(ASMO) mxsl.asm
mxsm.$(OBJ): mxsm.asm
	$(ASMC) $(ASMO) mxsm.asm
mxsp.$(OBJ): mxsp.asm
	$(ASMC) $(ASMO) mxsp.asm
mxss.$(OBJ): mxss.asm
	$(ASMC) $(ASMO) mxss.asm
mxtl.$(OBJ): mxtl.asm
	$(ASMC) $(ASMO) mxtl.asm
mxvs.$(OBJ): mxvs.asm
	$(ASMC) $(ASMO) mxvs.asm
mxwd.$(OBJ): mxwd.asm
	$(ASMC) $(ASMO) mxwd.asm
mxwm.$(OBJ): mxwm.asm
	$(ASMC) $(ASMO) mxwm.asm
mxwp.$(OBJ): mxwp.asm
	$(ASMC) $(ASMO) mxwp.asm
mxwr.$(OBJ): mxwr.asm
	$(ASMC) $(ASMO) mxwr.asm

all: $(LIBOBJS) modex.lib
# modex.tpu modex.tpp

#modex.tpu: $(LIBOBJS) modex.pas
#        $(PASC) $(PASO) modex
#        copy modex.tpu ..
#        copy modex.pas ..

#modex.tpp: $(LIBOBJS) modex.pas
#        $(PASC) /cp $(PASO) modex
#        copy modex.tpp ..

modex.lib: modex.lbr $(LIBOBJS)
	$(LIBC) modex.lib @modex.lbr

$(LIBOBJS):   modex.def
