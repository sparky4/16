 
#ifndef __MODEX_H
#define __MODEX_H
 
    /* ===== SCREEN RESOLUTIONS ===== */
 
#define Mode_320x200  0
#define Mode_320x400  1
#define Mode_360x200  2
#define Mode_360x400  3
#define Mode_320x240  4
#define Mode_320x480  5
#define Mode_360x240  6
#define Mode_360x480  7
 
    /* ===== MODE X SETUP ROUTINES ===== */
 
int far pascal set_vga_modex (int Mode, int MaxXpos, int MaxYpos, int Pages);
int far pascal set_modex (int Mode);
 
    /* ===== BASIC GRAPHICS PRIMITIVES ===== */
 
void far pascal clear_vga_screen (int Color);
void far pascal set_point (int Xpos, int Ypos, int Color);
int  far pascal read_point (int Xpos, int Ypos);
void far pascal fill_block (int Xpos1, int Ypos1, int Xpos2, int Ypos2,
                            int Color);
void far pascal draw_line (int Xpos1, int Ypos1, int Xpos2, int Ypos2,
                           int Color);
 
    /* ===== DAC COLOR REGISTER ROUTINES ===== */
 
void far pascal set_dac_register (int RegNo, int Red, int Green, int Blue);
void far pascal get_dac_register (int RegNo, int* Red, int* Green, int* Blue);
void far pascal load_dac_registers (char far *PalData, int StartReg,
                                    int EndReg, int VSync);
void far pascal readd_dac_registers (char far *PalData, int StartReg,
                                    int EndReg);
 
    /* ===== PAGE FLIPPING AND SCROLLING ROUTINES ===== */
 
void far pascal set_active_page (int PageNo);
int  far pascal get_active_page (void);
void far pascal set_display_page (int PageNo);
int  far pascal get_display_page (void);
void far pascal set_window (int DisplayPage, int XOffset, int YOffset);
int  far pascal get_x_offset (void);
int  far pascal get_y_offset (void);
void far pascal sync_display (void);
 
    /* ===== TEXT DISPLAY ROUTINES ===== */
 
void far pascal gprintc (int CharNum, int Xpos, int Ypos, int ColorF,
                         int ColorB);
void far pascal tgprintc (int CharNum, int Xpos, int Ypos, int ColorF);
void far pascal print_str (char far *Text, int MaxLen, int Xpos, int Ypos,
                           int ColorF, int ColorB);
void far pascal tprint_str (char far *Text, int MaxLen, int Xpos, int Ypos,
                            int ColorF);
void far pascal set_display_font (char far *FontData, int FontNumber);
 
    /* ===== BITMAP (SPRITE) DISPLAY ROUTINES ===== */
 
void far pascal draw_bitmap (char far *Image, int Xpos, int Ypos,
                             int Width, int Height);
void far pascal tdraw_bitmap (char far *Image, int Xpos, int Ypos,
                              int Width, int Height);
 
    /* ==== VIDEO MEMORY to VIDEO MEMORY COPY ROUTINES ===== */
 
void far pascal copy_page (int SourcePage, int DestPage);
void far pascal copy_bitmap (int SourcePage, int X1, int Y1, int X2, int Y2,
                             int DestPage, int DestX1, int DestY1);
 
 
#endif
