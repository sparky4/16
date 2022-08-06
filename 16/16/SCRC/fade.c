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
	while (1)
	{
		fadeOff(1, palette);
		palBlack();
		fadeOn(1, palette);
		palUpdate(palette);

		if (kbhit)
		{
			// Terminates the loop
			// when escape is pressed
			if (getch() == 27)
				break;
		}
	}

    free(palette);
    return;
}
