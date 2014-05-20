#include <conio.h>
#include <dos.h>
#include <mem.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "fixed32.hpp"
#include "modex.hpp"
#include "xprim.hpp"
#include "xpal.hpp"
#include "xblitbuf.hpp"

#define FIRE_DEMO 1
#define ROTATE_DEMO 1
#define MONSTER_DEMO_ONE 1
#define MONSTER_DEMO_TWO 1
#define MATH_DEMO 0

int
monster_demo1(void)
{
    clock_t begin, end;
    short int i;
    blitbuf sprite_image, blit_image;

    clearX(0);

    load_blitbufPCX("monster.pcx", &sprite_image);
    aligned_bitblitX(0, 20, &sprite_image);
    getch();

    blit_image.image = NULL;

    i=99;

    begin = clock();

    while (i > 0) {
        scale_blitbuf(120 + (i << 1), (i << 1), &sprite_image, &blit_image);
        boxX((99 - i), (119 - i), (220 + i), (120 + i), 0);
        vanilla_bitblitX(100 - i, 120 - i, &blit_image);
        i--;
    }

    end = clock();

    i=120;
    while (i > 0) {
        scale_blitbuf(i, 2, &sprite_image, &blit_image);

        putpixelX(159 - (i >> 1), 119, 0);
        putpixelX(159 - (i >> 1), 120, 0);
        putpixelX(160 + (i >> 1), 119, 0);
        putpixelX(160 + (i >> 1), 120, 0);

        vanilla_bitblitX(160 - (i >> 1), 119, &blit_image);
        delay(10);
        i -= 2;
    }

    filledboxX(156, 119, 163, 120, 0);
    putpixelX(159, 120, 1);
    delay(250);
    putpixelX(159, 120, 0);

    filledboxX(156, 119, 163, 120, 0);
    clear_blitbuf(&sprite_image);
    clear_blitbuf(&blit_image);

    getch();

    return (end - begin);
}


void
monster_demo2(void)
{
    short int i;
    blitbuf sprite_image, blit_image;

    clearX(0);

    load_blitbufPCX("monster.pcx", &sprite_image);
    aligned_bitblitX(0, 20, &sprite_image);
    getch();

    blit_image.image = NULL;

    i=99;
    while (i) {
        vertical_scale_blitbuf((i << 1), &sprite_image, &blit_image);
        boxX(0, (119 - i), 319, (120 + i), 0);
        aligned_bitblitX(0, 120 - i, &blit_image);
        i--;
    }

    filledboxX(0, 120, 319, 120, 0);

    i=318;
    while (i > 0) {
        scale_blitbuf(i, 1, &blit_image, &sprite_image);

        putpixelX(159 - (i >> 1), 119, 0);
        putpixelX(159 - (i >> 1), 120, 0);
        putpixelX(160 + (i >> 1), 119, 0);
        putpixelX(160 + (i >> 1), 120, 0);

        vanilla_bitblitX(160 - (i >> 1), 119, &sprite_image);
        delay(5);
        i -= 2;
    }

    filledboxX(156, 119, 163, 120, 0);
    putpixelX(159, 120, 1);
    delay(250);
    putpixelX(159, 120, 0);
    filledboxX(156, 119, 163, 120, 0);
    clear_blitbuf(&sprite_image);
    clear_blitbuf(&blit_image);

    getch();
}


void
fire_demo(void)
{
    #define V_WIDTH         80
    #define V_HEIGHT        50
    #define BUF_WIDTH       80
    #define BUF_HEIGHT      56
    #define REWIND          (V_WIDTH * 3)
    #define BUF_SIZE        (BUF_WIDTH * BUF_HEIGHT)

unsigned char fire_pal[768] = {
      0,   0,   0,       0,   0,  24,       0,   0,  24,       0,   0,  28,
      0,   0,  32,       0,   0,  32,       0,   0,  36,       0,   0,  40,
      8,   0,  40,      16,   0,  36,      24,   0,  36,      32,   0,  32,
     40,   0,  28,      48,   0,  28,      56,   0,  24,      64,   0,  20,
     72,   0,  20,      80,   0,  16,      88,   0,  16,      96,   0,  12,
    104,   0,   8,     112,   0,   8,     120,   0,   4,     128,   0,   0,
    128,   0,   0,     132,   0,   0,     136,   0,   0,     140,   0,   0,
    144,   0,   0,     144,   0,   0,     148,   0,   0,     152,   0,   0,
    156,   0,   0,     160,   0,   0,     160,   0,   0,     164,   0,   0,
    168,   0,   0,     172,   0,   0,     176,   0,   0,     180,   0,   0,
    184,   4,   0,     188,   4,   0,     192,   8,   0,     196,   8,   0,
    200,  12,   0,     204,  12,   0,     208,  16,   0,     212,  16,   0,
    216,  20,   0,     220,  20,   0,     224,  24,   0,     228,  24,   0,
    232,  28,   0,     236,  28,   0,     240,  32,   0,     244,  32,   0,
    252,  36,   0,     252,  36,   0,     252,  40,   0,     252,  40,   0,
    252,  44,   0,     252,  44,   0,     252,  48,   0,     252,  48,   0,
    252,  52,   0,     252,  52,   0,     252,  56,   0,     252,  56,   0,
    252,  60,   0,     252,  60,   0,     252,  64,   0,     252,  64,   0,
    252,  68,   0,     252,  68,   0,     252,  72,   0,     252,  72,   0,
    252,  76,   0,     252,  76,   0,     252,  80,   0,     252,  80,   0,
    252,  84,   0,     252,  84,   0,     252,  88,   0,     252,  88,   0,
    252,  92,   0,     252,  96,   0,     252,  96,   0,     252, 100,   0,
    252, 100,   0,     252, 104,   0,     252, 104,   0,     252, 108,   0,
    252, 108,   0,     252, 112,   0,     252, 112,   0,     252, 116,   0,
    252, 116,   0,     252, 120,   0,     252, 120,   0,     252, 124,   0,
    252, 124,   0,     252, 128,   0,     252, 128,   0,     252, 132,   0,
    252, 132,   0,     252, 136,   0,     252, 136,   0,     252, 140,   0,
    252, 140,   0,     252, 144,   0,     252, 144,   0,     252, 148,   0,
    252, 152,   0,     252, 152,   0,     252, 156,   0,     252, 156,   0,
    252, 160,   0,     252, 160,   0,     252, 164,   0,     252, 164,   0,
    252, 168,   0,     252, 168,   0,     252, 172,   0,     252, 172,   0,
    252, 176,   0,     252, 176,   0,     252, 180,   0,     252, 180,   0,
    252, 184,   0,     252, 184,   0,     252, 188,   0,     252, 188,   0,
    252, 192,   0,     252, 192,   0,     252, 196,   0,     252, 196,   0,
    252, 200,   0,     252, 200,   0,     252, 204,   0,     252, 208,   0,
    252, 208,   0,     252, 208,   0,     252, 208,   0,     252, 208,   0,
    252, 212,   0,     252, 212,   0,     252, 212,   0,     252, 212,   0,
    252, 216,   0,     252, 216,   0,     252, 216,   0,     252, 216,   0,
    252, 216,   0,     252, 220,   0,     252, 220,   0,     252, 220,   0,
    252, 220,   0,     252, 224,   0,     252, 224,   0,     252, 224,   0,
    252, 224,   0,     252, 228,   0,     252, 228,   0,     252, 228,   0,
    252, 228,   0,     252, 228,   0,     252, 232,   0,     252, 232,   0,
    252, 232,   0,     252, 232,   0,     252, 236,   0,     252, 236,   0,
    252, 236,   0,     252, 236,   0,     252, 240,   0,     252, 240,   0,
    252, 240,   0,     252, 240,   0,     252, 240,   0,     252, 244,   0,
    252, 244,   0,     252, 244,   0,     252, 244,   0,     252, 248,   0,
    252, 248,   0,     252, 248,   0,     252, 248,   0,     252, 252,   0,
    252, 252,   4,     252, 252,   8,     252, 252,  12,     252, 252,  16,
    252, 252,  20,     252, 252,  24,     252, 252,  28,     252, 252,  32,
    252, 252,  36,     252, 252,  40,     252, 252,  40,     252, 252,  44,
    252, 252,  48,     252, 252,  52,     252, 252,  56,     252, 252,  60,
    252, 252,  64,     252, 252,  68,     252, 252,  72,     252, 252,  76,
    252, 252,  80,     252, 252,  84,     252, 252,  84,     252, 252,  88,
    252, 252,  92,     252, 252,  96,     252, 252, 100,     252, 252, 104,
    252, 252, 108,     252, 252, 112,     252, 252, 116,     252, 252, 120,
    252, 252, 124,     252, 252, 124,     252, 252, 128,     252, 252, 132,
    252, 252, 136,     252, 252, 140,     252, 252, 144,     252, 252, 148,
    252, 252, 152,     252, 252, 156,     252, 252, 160,     252, 252, 164,
    252, 252, 168,     252, 252, 168,     252, 252, 172,     252, 252, 176,
    252, 252, 180,     252, 252, 184,     252, 252, 188,     252, 252, 192,
    252, 252, 196,     252, 252, 200,     252, 252, 204,     252, 252, 208,
    252, 252, 208,     252, 252, 212,     252, 252, 216,     252, 252, 220,
    252, 252, 224,     252, 252, 228,     252, 252, 232,     252, 252, 236,
    252, 252, 240,     252, 252, 244,     252, 252, 248,     252, 252, 252
};

    unsigned char *screen;
    unsigned char *flamebuf_ptr;
    unsigned char *flamebuf;
    unsigned char temp;
    unsigned short int i, j, skip;
    unsigned char temp_byte;

    set_paletteX(fire_pal);
    set_write_plane(ALL_PLANES);
    flamebuf = new unsigned char[BUF_SIZE];
    skip   = V_WIDTH;

    // Initialize the video buffer to 0's
    memset(flamebuf, 0, BUF_SIZE);

    while (!kbhit()) {
        // Transform current buffer
        flamebuf_ptr = flamebuf;
        i = (BUF_HEIGHT - 2);
        while (i--) {
            *flamebuf_ptr = (*(flamebuf_ptr + BUF_WIDTH) +
                       *(flamebuf_ptr + (BUF_WIDTH - 1)) +
                       *(flamebuf_ptr + (BUF_WIDTH + 1)) +
                       *(flamebuf_ptr + (BUF_WIDTH * 2))) >> 2;

            flamebuf_ptr += BUF_WIDTH;
            temp = *flamebuf_ptr;
            if (temp > 11) {
                *flamebuf_ptr -= 12;
            } else if (temp > 3) {
                *flamebuf_ptr -= 4;
            } else {
                *flamebuf_ptr = 0;
            }
            flamebuf_ptr += (1 - BUF_WIDTH);

            j = (BUF_WIDTH - 2);
            while (j--) {
                *flamebuf_ptr = (*(flamebuf_ptr + BUF_WIDTH) +
                           *(flamebuf_ptr + (BUF_WIDTH - 1)) +
                           *(flamebuf_ptr + (BUF_WIDTH + 1)) +
                           *(flamebuf_ptr + (BUF_WIDTH * 2))) >> 2;

                flamebuf_ptr += BUF_WIDTH;
                temp = *flamebuf_ptr;
                if (temp > 11) {
                    *flamebuf_ptr -= 12;
                } else if (temp > 3) {
                    *flamebuf_ptr -= 4;
                } else {
                    *flamebuf_ptr = 0;
                }
                flamebuf_ptr += (1 - BUF_WIDTH);
            }

            *flamebuf_ptr = (*(flamebuf_ptr + BUF_WIDTH) +
                       *(flamebuf_ptr + (BUF_WIDTH - 1)) +
                       *(flamebuf_ptr + (BUF_WIDTH * 2)) +
                       *(flamebuf_ptr + (BUF_WIDTH * 2) +
                                        (BUF_WIDTH - 1))) >> 2;

            flamebuf_ptr += BUF_WIDTH;
            temp = *flamebuf_ptr;
            if (temp > 11) {
                *flamebuf_ptr -= 12;
            } else if (temp > 3) {
                *flamebuf_ptr -= 4;
            } else {
                *flamebuf_ptr = 0;
            }
            flamebuf_ptr += (1 - BUF_WIDTH);
        }

        // Set new bottom line with random white or black
        temp = 0;
        flamebuf_ptr = flamebuf + (BUF_WIDTH * (BUF_HEIGHT - 2));

        j = BUF_WIDTH;
        temp = 0;
        while (j--) {
            // We change the value 1/4 of the time
            if ((rand() & 0x03) == 3) {
                temp = (255 - temp);
            }

            *(flamebuf_ptr + BUF_WIDTH) = temp;
            *flamebuf_ptr++             = temp;
        }

        // Write the buffer to the screen
//        wait_for_retrace();
        screen = RowsX[40];
        flamebuf_ptr = flamebuf;
        i = V_HEIGHT;
        while (i--) {
            j = V_WIDTH;
            while (j--) {
                temp_byte = *flamebuf_ptr++;
                *screen = temp_byte;
                screen += skip;
                *screen = temp_byte;
                screen += skip;
                *screen = temp_byte;
                screen += skip;
                *screen++ = temp_byte;
                screen -= REWIND;
            }
            screen += REWIND;
        }
    }

    getch();

    delete flamebuf;
}


void
main(void)
{
    FILE *fp;
    char ch;
    unsigned char pal[768];
    COORD x1, y1, x2, y2;
    Iangle theta1, theta2;
    Fixed32 trigSin, trigCos;
    blitbuf blit_image, sprite_image;
    clock_t begin, end;
    short int x, y, temp, done;
    long i, time1, time2, time3, count, mtime;
    BYTE *ptr1;
    BYTE *ptr2;

    set320x240x256_X();
    clearX(0);

    getch();

    temp = loadfontX("vga8x8.fnt");

    if (temp) {
        putstringX(0, 0, "Hello!", 2);
    } else {
        return;
    }

    getch();

#if ROTATE_DEMO
    get_BIOSpaletteX(pal, 0);
    set_paletteX(pal, 0);

    clearX(0);
    for (x=0; x < 320; x++) {
        for (y=0; y < 240; y++) {
            putpixelX(x, y, (x & 0xFF));
        }
    }

    getch();

    while (!kbhit()) {
        wait_for_retrace();
        rot_palette(-1);
    }

    getch();
#endif

    load_blitbufPCX("spock.pcx", &blit_image);
    scale_blitbuf(160, 100, &blit_image);
    aligned_bitblitX(0, 0, &blit_image);

    getch();

    clearX(0);

    getch();

    scale_blitbuf(224, 140, &blit_image);
    aligned_bitblitX(0, 0, &blit_image);

    getch();

    greyscale_blitbuf(&blit_image);
    grey_paletteX();
    aligned_bitblitX(0, 0, &blit_image);

    getch();

    clearX(0);

    getch();

    scale_blitbuf(160, 100, &blit_image);
    aligned_bitblitX(0, 0, &blit_image);

    getch();

    clear_blitbuf(&blit_image);
    load_blitbufPCX("spock.pcx", &blit_image);
    greyscale_blitbuf(&blit_image);
    smooth64_paletteX(1, 0, 1);
    aligned_bitblitX(0, 0, &blit_image);

    getch();

    flip_vertical_blitbuf(&blit_image);
    aligned_bitblitX(0, 0, &blit_image);

    getch();

    clear_blitbuf(&blit_image);
    load_blitbufPCX("spock.pcx", &blit_image);
    aligned_bitblitX(0, 0, &blit_image);

    done = 0;
    while (!done) {
        ch = getch();
        switch (ch) {
            case 'q':
            case 'Q':
                done = 1;
                break;

            case '+':
                brighten_paletteX(1, 1, 1);
                break;

            case '-':
                brighten_paletteX(-1, -1, -1);
                break;

            case '<':
                stretch_paletteX(11, 11, 11);
                break;

            case '>':
                stretch_paletteX(24, 24, 24);
                break;

            case 'S':
            case 's':
                save_blitbufPCX("dump.pcx", &blit_image);
                break;

            case 'L':
            case 'l':
                load_blitbufPCX("spock.pcx", &blit_image);
                break;

            case 'R':
                brighten_paletteX(1, 0, 0);
                break;

            case 'G':
                brighten_paletteX(0, 1, 0);
                break;

            case 'B':
                brighten_paletteX(0, 0, 1);
                break;

            case 'r':
                brighten_paletteX(-1, 0, 0);
                break;

            case 'g':
                brighten_paletteX(0, -1, 0);
                break;

            case 'b':
                brighten_paletteX(0, 0, -1);
                break;

        }
    }

    clearX(0);
    srand(0);
    grey_paletteX();

    begin = clock();

    for (i=0; i < 10000; i++) {
        x1 = rand() % 320;
        y1 = rand() % 200;
        x2 = rand() % 320;
        y2 = rand() % 200;

        lineX(x1, y1, x2, y2, (i & 0x3F));
    }

    end = clock();

    time1 = (end - begin);

    begin = end;

    for (i=0; i < 10000; i++) {
        x1 = rand() % 320;
        y1 = rand() % 200;
        x2 = rand() % 320;
        y2 = rand() % 200;

        temp = ((x1 + x2 + y1 + y2) & 0x3F);
    }

    end = clock();

    time2 = (end - begin);

    getch();

    for (i=0; i < 120; i++) {
        filledboxX(i, i, (319-i), (239-i), (i & 0x3F));
    }

    getch();

    load_blitbufPCX("buddha.pcx", &sprite_image);
    transparent_bitblitX(100, 100, &sprite_image);

    getch();

    clearX(0);
    clear_blitbuf(&blit_image);
    scale_blitbuf(152, 168, &sprite_image);
    alloc_blitbuf(&blit_image, 152, 168);

    // 152x168 image
    aligned_bitblitX(84, 36, &sprite_image);
    getch();

    initFixed32();

    theta1=0;
    count=0;

    begin = clock();

    while (!kbhit()) {
        ptr1 = sprite_image.image;
        ptr2 = blit_image.image;
        theta2 = theta1;

        y=168;
        while (y--) {
            CosSin(theta2, &trigCos, &trigSin);
            scale_scanline(ptr1, ptr2, 152, 152, (trigCos >> 10) + 88);
#if 0
            memcpy(ptr2+152, ptr2, 152);
            ptr1 += 304;
            ptr2 += 304;
            theta2 += 4;
            y--;
#else
            ptr1 += 152;
            ptr2 += 152;
            theta2 += 2;
#endif
        }

        theta1 += 2;

        aligned_bitblitX(84, 36, &blit_image);
        count++;
    }

    end = clock();

    getch();
    getch();

    clear_blitbuf(&blit_image);
    clear_blitbuf(&sprite_image);

#if FIRE_DEMO
    clearX(0);
    fire_demo();
#endif

#if MONSTER_DEMO_ONE
    mtime = monster_demo1();
#endif

#if MONSTER_DEMO_TWO
    monster_demo2();
#endif

    set80x25();

#if MATH_DEMO
    Fixed32 c1, c2, c3, a1, a2, a3;

    c1 = INT_TO_FIXED(50);
    c2 = INT_TO_FIXED(70);
    c3 = INT_TO_FIXED(50 * 70);

    a1 = FixedMul(c1, c2);
    a2 = FixedMulASM(c1, c2);

    printf("MUL C version   = %d\n", FIXED_TO_INT(a1));
    printf("MUL ASM version = %d\n", FIXED_TO_INT(a2));

    getch();

    a1 = FixedDiv(c3, c1);
    a2 = FixedDivASM(c3, c1);

    printf("DIV C version   = %d\n", FIXED_TO_INT(a1));
    printf("DIV ASM version = %d\n", FIXED_TO_INT(a2));

    getch();
#endif

    temp = (time1 - time2);
    printf("10000 lines took %4d ticks\n", time1);
    printf("rand() overhead = %4d ticks\n", time2);
    printf("Time in lineX   = %4d ticks\n", temp);
    printf("%d lines per second!\n\n", (10000 * 1000) / (55 * temp));

    temp = (end-begin);
    printf("Buddha = %4d blits\n", count);
    printf("       = %4d per second\n", (count * 1000) / (55 * temp));
    printf("Buddha = %4d scanline stretches\n", (count * 168));
    printf("       = %4d per second!\n\n", (count * 168000) / (55 * temp));

    printf("Monster took %d ticks for 99 frames\n", mtime);
}

