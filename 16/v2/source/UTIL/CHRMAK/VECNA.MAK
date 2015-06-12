
/* CHRMAK makefile
   û aen */

pcx_name=vecna;                         // image source
chr_name=vecna;                         // output .CHR name

frame_w=16;     frame_h=32;             // frame dims

hot_x=0;        hot_y=16;               // hotspot coord
hot_w=16;       hot_h=16;               // hotspot dims

per_row=5;      total_frames=20;        // frames per row, total

lidle=10;       ridle=15;;              // left/right idle frames
uidle=5;        didle=0;                // up/down idle frames
 
lscript=F10W10F11W10F12W10F11W10F10W10F13W10F14W10F13W10;       // movement scripts
rscript=F15W10F16W10F17W10F16W10F15W10F18W10F19W10F18W10;
uscript=F5W10F6W10F7W10F6W10F5W10F8W10F9W10F8W10;
dscript=F0W10F1W10F2W10F2W10F0W10F3W10F4W10F3W10;
