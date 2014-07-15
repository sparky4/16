wpp /d2 /0 /ml dos_gfx
rem wcl -0 -d2 -ml -c lib\modex16.c
wlink debug c option cvp file 'dos_gfx' library 'lib\x\modex.lib' name dos_gfx
copy /y dos_gfx.exe c:\z\bakapi.exe
