#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>

#define WIDTH   256

#define FILENAME "DIAGONAL.TIL"
char    fn[100] = FILENAME;
typedef unsigned char BYTE;
typedef BYTE    ROW[16];
typedef ROW     BITMAP[16];

BITMAP  b;
BITMAP  c;

BITMAP  pattern={{1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0},
                 {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
                 {1,0,0,0,0,0,1,1,1,0,0,0,0,0,0,0},
                 {1,0,0,0,1,1,0,0,0,1,1,0,0,0,0,0},
                 {1,0,0,1,0,0,0,0,0,0,0,2,0,0,0,0},
                 {0,0,1,0,0,1,1,0,1,1,0,0,2,0,0,0},
                 {0,0,1,0,0,1,2,0,1,2,0,0,2,0,0,0},
                 {0,1,0,0,0,0,0,0,0,0,0,0,0,2,0,0},
                 {0,1,0,0,0,0,0,0,0,0,0,0,0,2,0,0},
                 {0,1,0,0,1,2,0,0,0,1,2,0,0,2,0,0},
                 {0,0,1,0,1,2,1,1,1,1,2,0,2,0,0,0},
                 {0,0,1,0,0,2,2,2,2,2,0,0,2,0,0,2},
                 {0,0,0,1,0,0,0,0,0,0,0,2,0,0,0,2},
                 {0,0,0,0,2,2,0,0,0,2,2,0,0,0,0,2},
                 {0,0,0,0,0,0,2,2,2,0,0,0,0,0,2,2},
                 {0,0,0,0,0,0,0,0,0,0,0,2,2,2,2,2}};

void copy_pattern(BITMAP b,BITMAP patt, BYTE l, BYTE m, BYTE h)
        {
        int     x,y;

        for (y=0; y<16; y++) {
                for (x=0; x<16; x++) {
                        switch (patt[y][x]) {
                                case 0:
                                        b[y][x] = m;
                                        break;
                                case 1:
                                        b[y][x] = l;
                                        break;
                                case 2:
                                        b[y][x] = h;
                                        break;
                                }
                        }
                }
        }

/* Transforms linear to planar */
void transform(BITMAP b,BITMAP c)
        {
        int     x,y,p;
        BYTE    *pb = (BYTE *)c;

        for (p=0; p<4; p++) {
                for (y=0; y<16; y++) {
                        for (x=0; x<16; x+=4) {
                                *(pb++) = b[y][x+p];
                                }
                        }
                }
        }

void main(int argc,char *argv[])
        {
        FILE    *fp;
        int     i;
        int     width = WIDTH;

        fp = fopen(fn,"wb");
        if (!fp) {
                printf("Couldn't open %s for write.\n",fn);
                exit(1);
                }
        if (argc > 1) {
                width = atoi(argv[1]);
                if (width > WIDTH)
                        width = WIDTH;
                printf("Width = %d\n",width);
                }

        for (i = 0; i< width; i++) {
                BYTE    less,more;

                less = (BYTE)((i + width - 1) % width);
                more = (BYTE)((i + 1) % width);
                copy_pattern(b, pattern, less, (BYTE)i, more);
                transform(b,c);
                fwrite(c, 16,16, fp);
                printf("Square %d\r",i);
                }
        fclose(fp);
        printf("All done!     \n");
        exit(0);
        }
