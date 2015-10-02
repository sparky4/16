#!/bin/bash
git add .
. ./boop.sh "$*"
cp -fp *.exe /var/www/
#if [ -f "*.exe" ]
#	then
zip /var/www/scroll scroll.exe
zip /var/www/exmmtest exmmtest.exe
#fi
