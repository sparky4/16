rem bcc -mh -c memory.c
rem bcc -mh emm.c memory.obj
rem move emm.exe em0.exe
rem move memory.obj memory.ob
rem wdis -a memory.ob > memory.asm
rem wasm -c -mh -0 memory.asm
rem wcp -ecc -mh emm.c memory.obj \z\borlandc\lib\ch.lib
wcc -0 -mh memory.c
wcp -mh emm.c memory.obj
wcp -mh far.c memory.obj
