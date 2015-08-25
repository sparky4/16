#include <stdio.h>
#include "palette.h"

void main() {
    byte *palette;
    int i;

    /* get our starting point */
    palette = newPal();
    palSave(palette);

    /* fade to black and back */
    fadeOff(1, palette);
    palBlack();
    fadeOn(1, palette);
    palUpdate(palette);

    /*  fade to white and back */
    flashOff(1, palette);
    palWhite();
    flashOn(1, palette);
    palUpdate(palette);

    free(palette);
    return;
}
