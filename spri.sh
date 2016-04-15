#! /bin/bash

./pcxsscut -s data/spri/"$*".sht -hc "$1".h -hp _"$1"_ -i data/"$1".pcx -p data/spri/"$1".pal -tc 0x0 -y
./vrl2vrs -s data/spri/"$*".sht -hc "$1"s.h -hp _"$1"_ -o data/spri/"$1".vrs
