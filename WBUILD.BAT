@echo off
set pee="-h"
rem -s"
wmake -h -s clean
wmake %pee
wmake %pee comp
if exist *.err (wmake %pee vomitchan)
rem if exist *.o (del *.o)
rem if exist *.obj (del *.obj)
