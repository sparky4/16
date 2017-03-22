#! /bin/bash
dir1="16/Catacomb3D"
dir2="16/CatacombApocalypse"
. ./src/util/qhead.sh $0
echo ====	$dir1	====
grep "$1" "$dir1"/*.C
grep "$1" "$dir1"/*.H
echo ====	$dir2	====
grep "$1" "$dir2"/*.C
grep "$1" "$dir2"/*.H
. ./src/util/qfoot.sh $0
