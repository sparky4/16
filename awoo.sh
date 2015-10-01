#!/bin/bash
git add .
. ./boop.sh "$1"
cp -fp *.exe /var/www/html/
zip /var/www/html/scroll scroll.exe
zip /var/www/html/exmmtest exmmtest.exe
