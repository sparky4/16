#!/bin/bash
. ./wbuild.sh
#if [ -f "*.exe" ]
#	then
#fi
wmake -h www
wmake -h clean
wmake -h backupscript
if [ -z "$*" ]; then
		pee="wwww"
	else
		pee="$*"
fi
##echo "$pee"
git add .
. src/util/shbat/boop.sh "$pee"
wmake -h initscript
. ./wbuild.sh
##. ./mkdld.sh# slow compile ww
