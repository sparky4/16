#ifndef X_PALETTE_HPP
    #define X_PALETTE_HPP

#include "xtypes.hpp"

// Palette setting/getting functions
void set_paletteX(BYTE *pal, FLAG downgrade = 1);
void get_paletteX(BYTE *pal, FLAG upgrade = 1);
void get_BIOSpaletteX(BYTE *pal, FLAG upgrade = 1);

// Palette adjusting functions
void photo_negativeX(void);
void grey_paletteX(void);
void RGB_paletteX(void);
void smooth64_paletteX(BYTE r, BYTE g, BYTE b);
void brighten_paletteX(SBYTE r, SBYTE g, SBYTE b);
void stretch_paletteX(BYTE r, BYTE g, BYTE b);  // 1/16 units

// Misc palette functions
void rot_palette(BYTE distance);
BYTE find_RGB(BYTE r, BYTE g, BYTE b);

#endif

