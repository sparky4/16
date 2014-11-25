@echo off
if exist vcc.exe del vcc.exe
gcc -s -Wall -Werror -m486 -O3 vcc.c preproc.c lexical.c compile.c funclib.c -o dj\vcc.exe
rem gcc -Wall -Werror -pg vcc.c preproc.c lexical.c compile.c funclib.c -o dj\vcc
if not exist dj\vcc.exe goto end
if exist ..\djp.exe djp dj\vcc.exe
:end
