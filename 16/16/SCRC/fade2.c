#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include "palette.h"

void main() {
    byte *palette;
    int i;

    /* get our starting point */
    palette = newPal();
    palSave(palette);

    /* fade to black and back */
	while (!kbhit())
	{
		fadeOffHalf(1, palette);
		//palBlack();
		fadeOn(1, palette);
		palUpdate(palette);
	}

    free(palette);
    return;
}
