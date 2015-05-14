bcc -mh -c -lh memory.c
bcc -mh emm.c memory.obj
move emm.exe em0.exe
move memory.obj memory.ob
wdis -a memory.ob > memory.asm
wasm -c -mh -0 memory.asm
wcp -ecc -mh emm.c memory.obj \z\borlandc\lib\ch.lib