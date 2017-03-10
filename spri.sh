#! /bin/bash
cd data/spri
if [ ! -f "$1".vrs ] && [ -f "$1".vrl ] && [ -f "$1".sht ]; then
	../../vrl2vrs -s "$1".sht -o "$1".vrs -hc "$1".h -hp _"$1"_
	echo "$1".vrs created
#	echo "1st condition ok!"
fi
if [ -f "$1".vrl ] && [ -f "$1".sht ]; then
	../../pcxsscut -s "$1".sht -hc "$1".h -hp _"$1"_ -i ../"$1".pcx -p "$1".pal -tc 0x"$2" -y
# 	echo "2nd condition ok!"
else
	../../pcx2vrl -i ../"$1".pcx -o "$1".vrl -p "$1".pal -tc 0x"$2"
	echo "$1".vrl created
#	echo check if "$1".sht exist!
# 	echo "3rd condition ok!"
fi
cd ../../
