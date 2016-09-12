@echo off
wmake -h clean
wmake -h
rem wmake -h comp
if exist *.err (wmake vomitchan)
rem if exist *.o (del *.o)
rem if exist *.obj (del *.obj)
