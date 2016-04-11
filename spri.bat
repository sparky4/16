rem @echo off
@cd data\spri
@rem src\lib\doslib\hw\vga\dos86l\pcxsscut -s data\spri\%1.sht -hc %1.h -hp _%1_ -i data\%1.pcx -p data\spri\%1.pal -tc 0x84 -y
@rem src\lib\doslib\hw\vga\dos86l\vrl2vrs -s data\spri\%1.sht -hc %1s.h -hp _%1_ -o data\spri\%1.vrs
..\..\src\lib\doslib\hw\vga\dos86l\pcxsscut -s %1.sht -hc %1.h -hp _%1_ -i ..\%1.pcx -p %1.pal -tc 0x84 -y
..\..\src\lib\doslib\hw\vga\dos86l\vrl2vrs -s %1.sht -hc %1s.h -hp _%1_ -o %1.vrs
@cd ..\..\
