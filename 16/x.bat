wpp /d1 /0 /ml dos_gfx
wlink debug c option cvp file 'dos_gfx','lib\x\mxvs.obj' library 'lib\x\modex.lib' name dos_gfx
copy /y dos_gfx.exe c:\z\bakapi.exe
