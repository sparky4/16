#!/bin/bash
. ./w.sh
wmake
wmake comp
if [ -f "*.exe" ]
	then
wmake www
fi
wmake clean
if [ -z "$*" ]; then
		pee="wwww"
	else
		pee="$*"
fi
##echo "$pee"
git add .
. ./boop.sh "$pee"
. ./w.sh
