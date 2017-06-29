#! /bin/bash
pee="-h"
# -s"
wmake ${pee} clean
wmake ${pee}
wmake ${pee} comp
#if [[ -f *.err ]]
#then
#	echo dumping *.err
	#cat *.err
	wmake ${pee} vomitchan
#fi
#if [ -f 16_head.o ]
#then
#	rm *.o
#fi
####wmake $pee inntest.exe
