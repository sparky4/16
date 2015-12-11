#!/bin/bash
if [ -z "$*" ]; then
		"$pee"="wwww"
	else
		"$pee"="$*"
fi
git add .
. ./boop.sh "$*"
cp -fp *.exe /var/www/
scp -rp -P 26 *.exe 4ch.mooo.com:/var/www/16/
#if [ -f "*.exe" ]
#	then
#zip /var/www/scroll scroll.exe
#zip /var/www/exmmtest exmmtest.exe
#fi
