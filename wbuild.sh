#! /bin/bash
wmake -h clean
wmake -h
wmake -h comp
if [ -f *.err ]
then
	echo dumping *.err
	cat *.err
fi
