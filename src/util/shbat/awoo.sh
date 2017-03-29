#!/bin/bash
. ./wbuild.sh
#if [ -f "*.exe" ]
#	then
#fi
wmake -s -h www
wmake -s -h clean
wmake -s -h backupscript
if [ -z "$*" ]; then
		pee="wwww"
	else
		pee="$*"
fi
##echo "$pee"
git add .
. src/util/shbat/boop.sh "$pee"
wmake -s -h initscript
. ./wbuild.sh
##. ./mkdld.sh# slow compile ww
