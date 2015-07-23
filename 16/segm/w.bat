@echo off
wcc -0 -mt pee.c
pee.exe > wpee
echo "====" >> wpee
wcc -0 -ms pee.c
pee.exe >> wpee
echo "====" >> wpee
wcc -0 -mm pee.c
pee.exe >> wpee
echo "====" >> wpee
wcc -0 -mc pee.c
pee.exe >> wpee
echo "====" >> wpee
wcc -0 -ml pee.c
pee.exe >> wpee
echo "====" >> wpee
wcl -0 -mh pee.c
pee.exe >> wpee