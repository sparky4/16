#! /bin/bash
cd data/spri
#../../pcx2vrl -i ../"$1".pcx -o "$1".vrl -p "$1".pal -tc 0x"$2"
../../pcxsscut -s "$1".sht -hc "$1".h -hp _"$1"_ -i ../"$1".pcx -p "$1".pal -tc 0x"$2" -y
../../vrl2vrs -s "$1".sht -hc "$1"s.h -hp _"$1"_ -o "$1".vrs
cd ../../
