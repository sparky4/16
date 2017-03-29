#! /bin/bash
wmake -s -h clean
wmake -s -h
wmake -s -h comp
#if [[ -f *.err ]]
#then
#	echo dumping *.err
	#cat *.err
	wmake -s -h vomitchan
#fi
#if [ -f 16_head.o ]
#then
#	rm *.o
#fi
####wmake -s -h inntest.exe
