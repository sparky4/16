#!/bin/bash
if [ -z "$*" ]; then
		pee="wwww"
	else
		pee="$*"
fi
##echo "$pee"
git add .
. ./boop.sh "$pee"
cp -fp bin/*.exe /var/www/
scp -rp -P 26 bin/*.exe 4ch.mooo.com:/var/www/16/
#if [ -f "bin/*.exe" ]
#	then
#zip /var/www/scroll scroll.exe
#zip /var/www/exmmtest exmmtest.exe
#fi
