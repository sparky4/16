#!/bin/bash
git add .
. ./boop.sh "$*"
cp -fp *.exe /var/www/html/
if [ -e "*.exe" ]
	then
zip /var/www/html/scroll scroll.exe
zip /var/www/html/exmmtest exmmtest.exe
fi
