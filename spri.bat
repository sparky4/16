@echo off
pcxsscut -s data\spri\%1.sht -hc %1.h -hp _%1_ -i data\%1.pcx -p data\spri\%1.pal -tc 0x84 -y
vrl2vrs -s data\spri\%1.sht -hc %1s.h -hp _%1_ -o data\spri\%1.vrs
