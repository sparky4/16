#include "modex16.h"

void
main() {
    byte *pal;

    modexEnter();

    pal = modexNewPal();
    modexPalSave(pal);

    modexSavePalFile("gfx.pal", pal);

    modexLeave();

}
