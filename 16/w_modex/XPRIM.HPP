#ifndef X_PRIMITIVES_HPP
    #define X_PRIMITIVES_HPP

#include "xtypes.hpp"

// Waits for vertical retrace
void wait_for_retrace(void);

// Page setting functions
void setDrawPage(unsigned int page);
void setVisiblePage(unsigned int page);

// Screen clearing functions
void clearX(BYTE color);

// Drawing functions
void putpixelX(COORD x, COORD y, BYTE color);
BYTE getpixelX(COORD x, COORD y);
void boxX(COORD x1, COORD y1, COORD x2, COORD y2, BYTE color);
void filledboxX(COORD x1, COORD y1, COORD x2, COORD y2, BYTE color);
void circleX(COORD x, COORD y, DIST r, BYTE color);
void filledcircleX(COORD x, COORD y, DIST r, BYTE color);
void lineX(COORD lx1, COORD ly1, COORD lx2, COORD ly2, BYTE color);

// Text output functions
int  loadfontX(char *fname);
void putchX(COORD x, COORD y, char c, BYTE color);
void putstringX(COORD x, COORD y, char *str, BYTE color);

#endif

