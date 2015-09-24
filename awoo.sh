#!/bin/bash
git add .
bash ./boop.sh
cp -fp *.exe /var/www/html/
zip /var/www/html/scroll scroll.exe
