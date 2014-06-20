/*
 * Headers and definitions for the palette routines.
 */
#include "constant.h"

/* fade and flash */
void fadeOn(word fade, byte *palette);
void fadeOff(word fade, byte *palette);
void flashOn(word fade, byte *palette);
void flashOff(word fade, byte *palette);

/* save and load */
void palSave(byte *palette);
byte *newPal();
void loadPaletteFile(char *filename, byte **palette);

/* blanking */
void palBlack();
void palWhite();

/* utility */
void palUpdate();
void waitborder();

