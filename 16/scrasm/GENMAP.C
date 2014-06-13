#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>

#define WIDTH   255

#define MAPNAME "Diagonal"
#define FILENAME "%s.MAP"
char    fn[100] = FILENAME;
typedef unsigned char BYTE;
typedef unsigned short int WORD;
typedef BYTE    ROW[WIDTH];

ROW     r;

#define MAGIC_NUM       100
#define SQUARE_WIDTH    16
#define SQUARE_HEIGHT   16
#define SCREEN_WIDTH    320
#define SCREEN_HEIGHT   200
#define VIRTUAL_WIDTH   352
#define VIRTUAL_HEIGHT  240
typedef struct MAPHEADER {
        BYTE    name[12];       /* 12    Includes [n]=0 and [n+1]=26 */
        WORD    width;          /*  2                                */
        WORD    height;         /*  2                                */
        WORD    extent;         /*  2                                */
        WORD    off_x1;         /*  2                                */
        WORD    off_y1;         /*  2                                */
        WORD    off_x2;         /*  2                                */
        WORD    off_y2;         /*  2                                */
        WORD    x_wrap;         /*  2                                */
        WORD    y_wrap;         /*  2                                */
        WORD    magic;          /*  2                                */
        } MAPHEADER, far *LPMAPHEADER;
MAPHEADER mh;

void main(int argc, char *argv[])
        {
        FILE    *fp;
        int     i,j;
        BYTE    b;
        int     width = WIDTH;
        int     height = WIDTH;

        if (argc > 1) {
                width = atoi(argv[1]);
                if (width > WIDTH)
                        width = WIDTH;
                printf("Width = %d\n",width);
                height=width;
                if (argc > 2) {
                        height = atoi(argv[2]);
                        if (height > WIDTH)
                                height = WIDTH;
                        printf("Height = %d\n",height);
                        }
                }

        sprintf(fn,FILENAME,MAPNAME);
        fp = fopen(fn,"wb");
        if (!fp) {
                printf("Couldn't open %s for write.\n",fn);
                exit(1);
                }

        memset(&mh, 0xFF, sizeof(MAPHEADER));   /* Will reveal missing initializing */
        strcpy((char *)mh.name,MAPNAME);
        mh.name[8]=0;
        mh.name[9]=26;  /* Ctrl-Z */
        mh.width = (WORD)width;
        mh.height = (WORD)height;
        mh.extent = (WORD)((WORD)width * (WORD)height);
        mh.off_x1 = (WORD)0;
        mh.off_y1 = (WORD)0;
        mh.off_x2 = (WORD)(((VIRTUAL_WIDTH / SQUARE_WIDTH) - 1) % width);
        mh.off_y2 = (WORD)((((VIRTUAL_HEIGHT / SQUARE_HEIGHT) - 1) % height) * width);
        mh.x_wrap = (WORD)width;
        mh.y_wrap = (WORD)height;
        mh.magic = MAGIC_NUM;
        fwrite(&mh, 1, sizeof(MAPHEADER), fp);

        for (i = 0; i<width; i++) {
                b = (BYTE)(i%width);
                for (j = 0; j<width; j++) {
                        r[j] = b;
                        b = (BYTE)(((int)b+1) % width);
                        }
                r[0]=1;
                if (i == 0) r[0]=0;
                fwrite(r, width,1, fp);
                printf("Map row %d\r",i);
                }
        fclose(fp);
        printf("All done!    \n");
        exit(0);
        }
