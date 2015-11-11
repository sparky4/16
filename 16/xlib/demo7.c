/* Program to demonstrate Bezier curves  */

#include <conio.h>

#include "xlib.h"
#include "xbezier.h"
#include "xpal.h"

void main(void)
{
	int i, r, g, b;

	x_set_mode(X_MODE_360x480, 360);

	for (b = 0; b < 6; b++)
		for (g = 0; g < 6; g++)
			for (r = 0; r < 6; r++)
				x_set_rgb(b + 6 * g + 36 * r,
						  12 * b + 3, 12 * g + 3, 12 * r + 3);
	for (i = 216; i < 256; i++)
		x_set_rgb(i, i - 193, i - 193, 279 - i);

	for (i = 0; i <= 1080; i++) {
		x_bezier(i, 240, 90, i, 270, 479-i, 359 - i, 240,
				 6, i ,0);
	}
	for (i = 720; i >= 0; i--) {
		x_bezier(360 - i, i/3, 360 - i, i, i, i, i, i/3,
				 4, i ,0);
		x_bezier(360 - i, 479-i/3, 360 - i, 479-i, i, 479-i, i, 479-i/3,
				 4, i ,0);
	}

	getch();

	x_text_mode();
}

