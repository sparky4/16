#! /bin/bash
dir1="src"
dir2=$dir1"/lib"
dir3=$dir2"/vgmsnd"
. ./src/util/qhead.sh $0
#echo ====	$dir1	====
grep "$1" "$dir1"/*.c
grep "$1" "$dir1"/*.h
#echo ====	$dir2	====
grep "$1" "$dir2"/*.c
grep "$1" "$dir2"/*.h
#echo ====	$dir3	====
grep "$1" "$dir3"/*.c
grep "$1" "$dir3"/*.h
. ./src/util/qfoot.sh $0
