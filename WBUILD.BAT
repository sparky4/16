@echo off
wmake -h clean
wmake -h
rem wmake -h comp
if exist *.err (wmake vomitchan)
if exist *.o (del *.o)
if exist *.obj (del *.obj)
