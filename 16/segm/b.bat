@echo off
bcc -mt pee.c
pee.exe > bpee
echo "====" >> bpee
bcc -ms pee.c
pee.exe >> bpee
echo "====" >> bpee
bcc -mm pee.c
pee.exe >> bpee
echo "====" >> bpee
bcc -mc pee.c
pee.exe >> bpee
echo "====" >> bpee
bcc -ml pee.c
pee.exe >> bpee
echo "====" >> bpee
bcc -mh pee.c
pee.exe >> bpee