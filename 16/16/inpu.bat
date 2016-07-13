wcc -0 -mc src\lib\lib_head.c
wcc -0 -mc src\lib\16_in.c
wcl -mc -0 -l=dos src\inputest.c 16_in.obj lib_head.obj
