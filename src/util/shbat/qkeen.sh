#! /bin/bash
dir="16/keen"
. ./src/util/qhead.sh $0
grep "$1" "$dir"/*.c
grep "$1" "$dir"/*.h
grep "$1" "$dir"/lib/*.c
grep "$1" "$dir"/lib/*.h
. ./src/util/qfoot.sh $0
