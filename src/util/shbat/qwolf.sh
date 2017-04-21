#! /bin/bash
dir1="16/wf3d8086"
dir2="16/wolf3d/WOLFSRC"
. ./src/util/qhead.sh $0
echo ====	$dir1	====
grep "$1" "$dir1"/*.c
grep "$1" "$dir1"/*.h
grep "$1" "$dir1"/*.asm
echo ====	$dir2	====
grep "$1" "$dir2"/*.C
grep "$1" "$dir2"/*.H
grep "$1" "$dir2"/*.ASM
. ./src/util/qfoot.sh $0
