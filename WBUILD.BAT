@echo off
wmake -s -h clean
wmake -s -h
rem wmake -s -h comp
if exist *.err (wmake vomitchan)
rem if exist *.o (del *.o)
rem if exist *.obj (del *.obj)
