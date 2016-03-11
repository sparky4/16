#!/bin/bash
wmake
wmake comp
if [ -f "*.exe" ]
        then
cp -f *.exe /var/www/
cp inputest.exe /var/www/
scp -r -P 26 *.exe 4ch.mooo.com:/var/www/16/
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
