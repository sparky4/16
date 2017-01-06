#!/bin/bash
mv src/scroll.c src/scroll_.c
mv src/_scroll.c src/scroll.c
mv src/scroll_.c src/_scroll.c

mv src/lib/scroll16.c src/lib/scroll1_.c
mv src/lib/_croll16.c src/lib/scroll16.c
mv src/lib/scroll1_.c src/lib/_croll16.c
