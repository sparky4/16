#!/bin/bash
git add .
. ./boop.sh "$*"
cp -fp *.exe /var/www/
if [ -e "*.exe" ]
	then
zip /var/www/scroll scroll.exe
zip /var/www/exmmtest exmmtest.exe
fi
