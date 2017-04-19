#! /bin/bash
pee="-h"
wmake -s "$pee" clean
wmake -s "$pee"
wmake -s "$pee" comp
#if [[ -f *.err ]]
#then
#	echo dumping *.err
	#cat *.err
	wmake -s "$pee" vomitchan
#fi
#if [ -f 16_head.o ]
#then
#	rm *.o
#fi
####wmake $pee inntest.exe
