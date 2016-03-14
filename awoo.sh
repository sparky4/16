#!/bin/bash
. ./build.sh
if [ -f "*.exe" ]
	then
wmake -h www
fi
wmake -h clean
if [ -z "$*" ]; then
		pee="wwww"
	else
		pee="$*"
fi
##echo "$pee"
git add .
. ./boop.sh "$pee"
. ./builq.sh
