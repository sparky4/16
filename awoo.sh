#!/bin/bash
. ./wbuild.sh
#if [ -f "*.exe" ]
#	then
#fi
#++++wmake -h www
wmake -h clean
if [ -z "$*" ]; then
		pee="wwww"
	else
		pee="$*"
fi
##echo "$pee"
git add .
. ./boop.sh "$pee"
. ./wbuild.sh
##. ./mkdld.sh# slow compile ww
