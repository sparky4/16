#! /bin/bash
cd data/spri
if [ -f "$1".sht ]; then
	../../pcxsscut -s "$1".sht -hc "$1".h -hp _"$1"_ -i ../"$1".pcx -p "$1".pal -tc 0x"$2" -y
	#echo "1st condition ok!"
else
	../../pcx2vrl -i ../"$1".pcx -o "$1".vrl -p "$1".pal -tc 0x"$2"
	#echo "2nd condition ok!"
	echo check if "$1".sht exist!
fi
cd ../../

