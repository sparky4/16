#!/bin/bash
if [ -z "$*" ]; then
		pee="wwww"
	else
		pee="$*"
fi
##echo "$pee"
git add .
. ./boop.sh "$pee"
if [ -f "*.exe" ]
	then
cp -fp *.exe /var/www/
scp -rp -P 26 *.exe 4ch.mooo.com:/var/www/16/
fi
