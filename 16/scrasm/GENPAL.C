#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>

#define COLORS  256
#define PALNAME "Diagonal"
#define FILENAME "%s.PAL"
char    fn[100] = FILENAME;
typedef unsigned char BYTE;
typedef unsigned short int WORD;
typedef struct COLOR {
        BYTE    r,g,b;
        } COLOR, far *LPCOLOR;

void main()             /* int argc, char *argv[]) */
        {
        FILE    *fp;
        int     i;
        int     colors = COLORS;
        COLOR   c;
        int     r,dr,g,dg,b,db;

        sprintf(fn,FILENAME,PALNAME);
        fp = fopen(fn,"wb");
        if (!fp) {
                printf("Couldn't open %s for write.\n",fn);
                exit(1);
                }

        r=0;    dr=2;
        g=0;    dg=3;
        b=0;    db=5;
        for (i = 0; i < colors; i++) {
                c.r = (BYTE)r;  r+=dr;
                if (r > 63) { r = 63; dr = -dr; }
                 else if (r < 0) { r = 0; dr = -dr; }
                c.g = (BYTE)g;  g+=dg;
                if (g > 63) { g = 63; dg = -dg; }
                 else if (g < 0) { g = 0; dg = -dg; }
                c.b = (BYTE)b;  b+=db;
                if (b > 63) { b = 63; db = -db; }
                 else if (b < 0) { b = 0; db = -db; }
                fwrite(&c, sizeof(c),1, fp);
                printf("Palette %d\r",i);
                }
        fclose(fp);
        printf("All done!    \n");
        exit(0);
        }
