@echo off
echo ==This is the results of borland c's segment pointers== > bpee
echo These are the various memory models of the 8088 that are demonstrated here! >> bpee
@del pee.obj
echo ==tiny== >> bpee
bcc -mt pee.c
pee.exe >> bpee
echo ==small== >> bpee
bcc -ms pee.c
pee.exe >> bpee
echo ==medium== >> bpee
bcc -mm pee.c
pee.exe >> bpee
echo ==compact is what project 16 uses== >> bpee
bcc -mc pee.c
pee.exe >> bpee
echo ==large== >> bpee
bcc -ml pee.c
pee.exe >> bpee
echo ==HUGE GUTS!== >> bpee
bcc -mh pee.c
pee.exe >> bpee