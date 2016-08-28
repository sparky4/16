#! /bin/bash
wmake -h clean
wmake -h -f makefile.med
wmake -h comp
if [ -f *.err ]
then
	echo dumping *.err
	#cat *.err
	wmake vomitchan
fi
#if [ -f 16_head.o ]
#then
#	rm *.o
#fi
