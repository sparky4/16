@echo off
wmake -h clean -f makefile.wat
wmake -h -f makefile.wat
rem wmake -h comp
if exist *.err (wmake vomitchan)
rem if exist *.o (del *.o)
rem if exist *.obj (del *.obj)
