#include <conio.h>

#include "xlib.h"
#include "xcircle.h"

void main(void)
{
	int i;

	x_set_mode(1, 320);
	for (i = 0; i < 120; i++) {
		x_circle(0, 0, i, 121 - i, 0);
		x_circle(319 - i, 0, i, i - 20, 0);
		x_circle(0, 239 - i, i, i - 20, 0);
		x_circle(319 - i, 239 - i, i, 121 - i, 0);
		x_filled_circle(40 + i, i, 241 - (i << 1), i+1, 0);
	}
	getch();

	x_text_mode();
}
