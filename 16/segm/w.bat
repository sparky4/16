@echo off
echo ==This is the results of open watcom's base pointers== > wpee
echo These are the various memory models of the 8088 that are demonstrated here! >> wpee
@del pee.obj
echo ==tiny is not found so default== >> wpee
wcl -l=dos -0 pee.c
pee.exe >> wpee
echo ==small== >> wpee
wcl -l=dos -0 -ms pee.c
pee.exe >> wpee
echo ==medium== >> wpee
wcl -l=dos -0 -mm pee.c
pee.exe >> wpee
echo ==compact is what project 16 uses== >> wpee
wcl -l=dos -0 -mc pee.c
pee.exe >> wpee
echo ==large== >> wpee
wcl -l=dos -0 -ml pee.c
pee.exe >> wpee
echo ==HUGE GUTS!== >> wpee
wcl -l=dos -0 -mh pee.c
pee.exe >> wpee