/*
Copyright (C) 1998 BJ Eirich (aka vecna)
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public Lic
See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

// Additional code by Richard Lau (aka Ric)

/* Date started: 08/Jul/98 */
/* -- 29/Jul/98 --
 * Heh, I've actually done quite a lot of work on this even though I intend
 * to replace it. Oh well :)
 *
 * I've now made clicking on the mask indicator button (labelled with a small
 * "m") drop back to draw mode if you're in select mode (since it makes no
 * sense to select an area with the mask turned off). Holding down shift
 * and dragging the mouse when you're in select mode will allow you to select
 * a rectangle. The PgUp/PgDn colour shifters will now work in paste mode
 * (affecting the image in the copy buffer).
 *
 * The fill routine is now in place and I've put a button in for the pick
 * colour/eyedropper tool so that I don't get asked to put in a function for
 * like I continuously did with ACE. Due to the extra buttons I've had to
 * shift some of the other buttons around to make them all fit onto the
 * screen. It's probably not the best arrangement but if anyone comes up with
 * a better layout I'm open to suggestions.
 *
 * The wierd button labelling problem I had last time has been corrected - the
 * problem was traced back to the pixels routine in SMALFONT.C. Aen pointed
 * out that I wasn't allocating space for the NULL terminator for all the
 * strings so that has now been fixed (by adding 1 to strlen).
 *
 * - Ric
 */
/*
 * Keys:
 *      CTRL/S + mouse click - (over edit window) Pick up colour under mouse
 *      SHIFT+mb down+drag mouse - select rectangle (select mode only)
 *      SPACEBAR    - toggle between draw mode and select mode
 *      CTRL+Z or U - one level undo
 *      CTRL+A      - select all
 *      CTRL+U      - unselect all
 *      CTRL+C      - copy selected area
 *      CTRL+X      - cut selected area
 *      CTRL+V      - paste
 *      DEL         - clears selected area
 *      CTRL+R      - reverts the tile to the original (like undo in maped 1)
 *                    (self note: modify to ask for confirmation)
 *      G           - toggles grid on/off
 *      H           - toggles hilight on/off
 *      M           - toggles mask on/off
 *      I           - inverts mask
 *      X           - mirrors image
 *      Y           - flips image
 *      L/R         - Rotates tile 90 degrees anti-clockwise/clockwise
 *      Q/W         - Steps through animation (in tile edit mode) if any
 *      A/Z and S/X - changes the left or right drawing colours
 *      PGUP/PGDN   - Colour shift up/down
 *      [ and ]     - toggle screen resolutions
 *      ALT+X       - quits to DOS
 */
/* -- 26/Jul/98 --
 * I've actually decided to start the editor over in a new file so that the
 * code is a bit more elegant. As it stands some bits of this are messy but
 * it currently works ;). Anyway I've decided that what I'll do is to in
 * effect write two tile editors - continuing work on this one and working
 * on the rewrite as a "side project". The eventual aim will be to replace
 * this editor with the rewrite but if factors intervene preventing me from
 * completing the rewrite then there'll always be this editor to fall back on.
 *
 * New in this release is a copy/cut/paste system. New buttons have been added.
 * For some reason I haven't figured out yet I can't call the two rotate
 * buttons "Rotate ACW" and "Rotate CW" (it causes MapEd to crash) so I've had
 * to resort to calling them "Turn_L" and "Turn_R".
 * - Ric
 */

#include <string.h>
#include <malloc.h>

#include "config.h"
#include "guicomp.h"
#include "keyboard.h"
#include "maped.h"
#include "mouse.h"
#include "render.h"
#include "vdriver.h"

#include "smalfont.h"

#undef free

#define TRUE  1
#define FALSE 0
// #define NULL ((void *)0)

#define MOUSENONE    0    // No mouse buttons pressed
#define MOUSERELEASE 1    // Mouse button released
#define MOUSEPRESSED 2    // Mouse button held down
#define MOUSECLICKED 4    // Mouse button pressed

// Setup some defines for the regions/areas/windows where everything goes
#define MAX_REGIONS 29    // Number of regions
#define BASEREG 0         // Window coords + title
#define HELPBAR 1         // Helpbar - put helpful messages here :)
#define PALETTE 2         // The palette
#define EDITREG 3         // Main edit window
#define PREVIEW 4         // Preview of image being edited
#define SCOLBOX 5         // Selected colour box
#define SCTXTBX 6         // Text box to show info on the selected colours
#define QUITBUT 7         // Quit button
#define DRAWBUT 8         // Draw mode indicator
#define FILLBUT 9         // Fill mode indicator
#define PICKBUT 10        // Pick mode indicator
#define MASKBUT 11        // Mask mode indicator
#define PSTEBUT 12        // Paste mode indicator
#define EDMDFRM 13        // Frame for edit mode radio buttons
#define MASKIND 14        // Indicator showing if mask is shown
#define GRIDIND 15        // Indicator showing if grid is shown
#define ANIMIND 16        // Indicator showing if animations are shown
#define PXHLIND 17        // Indicator showing if pixel highlight shown
#define IND_FRM 18        // Frame for indicators (push buttons)
#define BUT_FRM 19        // Frame for buttons
#define UNDOBUT 20        // Undo button
#define COPYBUT 21        // Copy button
#define CUT_BUT 22        // Cut button
#define CLR_BUT 23        // Clear button
#define TOOLFRM 24        // Frame for tools
#define FLIPBUT 25        // Flip image button
#define MIRRBUT 26        // Mirror image button
#define RIACBUT 27        // Rotate anticlockwise button
#define RIC_BUT 28        // Rotate clockwise button

// Define TEbitflags values
#define TEQUITCUR 1
#define TEANIMATE 2
#define TEGRID    4
#define TEHILIGHT 8
#define TESHOWMSK 16

// Define TEREGION->bitflags values
#define REGVISIBLE  1
#define REGACTIVE   2
#define REGNOCLICK  4
#define REGTWOCLICK 8          // wait for a second click

// TEeditmode defs
#define EDDRAW  1
#define EDFILL  2
#define EDPICK  4
#define EDMASK  8
#define EDPASTE 16

// TEtype defs
#define TEUNKNOWN 0
#define TETILE    1

// DATABUTTON buttontypes:
#define CLICKBUTTON 1          // normal button
#define PUSHBUTTON  2          // can be pushed in/out (toggle bit bitvalue)
#define RADIOBUTTON 4          // for button banks (set *bitflag=bitvalue)

// DATABASEREGION windowtypes:
#define DBRSTDWINDOW 0         // window and title bar
#define DBRPANEL     1         // no title bar

// INFOTYPEs:
#define DITREGION     1
#define DITBASEREGION 2
#define DITBUTTON     4
#define DITEDITREG    8

// Button flag action - has this button been clicked?
#define BFANONE   0
#define BFAUNDO   1
#define BFACOPY   2
#define BFACUT    4
#define BFACLEAR  8
#define BFARIAC   16
#define BFARIC    32
#define BFAMIRROR 64
#define BFAFLIP   128

// Variables
typedef struct TEREGION
  {
  int infotype;                 // type of struct
  unsigned int x1, x2, y1, y2;  // bounding coordinates
  char *caption;                // helpbar text
  int bitflags;                 // see defines above
  void (*drawproc)(struct TEREGION *);  // region's draw procedure
                                        // - allows nice and short RenderTileEdit proc ;)
  void *data;        // extended information
  } TEREGION;
typedef struct DATABASEREGION
  {
  int infotype;                 // type of struct
  int numofregs;                // total number of regions (including base)
  int windowtype;               // type of window to draw
  } DATABASEREGION;
typedef struct DATABUTTON
  {
  int infotype;                 // type of struct
  int *bitflag;                 // address of bitflag
  char *text;                   // addition text (eg helptext)
  int bitvalue;                 // 1st bit/value
  int buttontype;               // type
  } DATABUTTON;
typedef struct DATAEDITREG
  {
  int infotype;                 // type of struct
  int firstx, firsty;           // first coords for two-click operation
  } DATAEDITREG;


static TEREGION *TEregs=NULL;       // pointer to the regions
static unsigned char lcolor=255;    // left  mouse colour
static unsigned char rcolor=0;      // right mouse colour
static TEREGION *TEOldReg=NULL;
static unsigned int MBStatus[3]={MOUSENONE,MOUSENONE,MOUSENONE};
static int TEanimstrand=0;        // Animation strand
static int TEbitflags=TEANIMATE;  // Set default bitflags
static int TEbfa=BFANONE;         // Set default button action flags
static int TEeditmode=EDDRAW;     // Default edit region mode
static int TExoffs=16;            // X offset for region "windows"
static int TEyoffs=16;            // Y offset for region "windows"
unsigned int TEtype=TEUNKNOWN;    // type of graphic being edited
unsigned int TEtile; // tile to edit - may possibly expand this to an array for simultaneous editing
unsigned int TExsize=0;   // width of graphic being edited
unsigned int TEysize=0;   // height of graphic being edited
unsigned int TEcopyxsize=0;   // width of copy buffer
unsigned int TEcopyysize=0;   // height of copy buffer
unsigned char *TEsource=NULL; // pointer to original image being edited
static unsigned char *TEimage=NULL;  // pointer to image being edited
static unsigned char *TEmask=NULL;   // pointer to image mask
static unsigned char *TEundo=NULL;   // pointer to undo buffer
static unsigned char *TEcopy=NULL;   // pointer to copy buffer
static unsigned char *TEcmsk=NULL;   // pointer to copy mask
static char TEtempstr[256];          // Temporary string buffer

// Function prototypes
static void *TEAlloc(unsigned long amount, char *whatfor);

#define TEFree(ptr) if (ptr) free(ptr); ptr=NULL;
#define TEFreeReg(ptr) DestroyRegions(&ptr); if (ptr) free(ptr); ptr=NULL;
#define TENewText(text, whatfor) (char *) strcpy(TEAlloc(strlen(text)+1, whatfor), text);
//static void TEFree(void *TETmpBuf);
static void MBClickHandler(TEREGION *TEBaseReg);
static void AboutTE(void);
static void DestroyRegions(TEREGION **TEBaseReg);
static void SetupRegions(void);
static void InitTEImage(void);
static void DeinitTEImage(void);
static int  MouseOverTEreg(TEREGION *TEr);
static void RenderTileEdit(TEREGION *TEBaseReg);
static void RestartTileEditor(void);
static void SetCaption(TEREGION *TEr, char *helptext);
static void TEDrawHilight(int x1, int y1, int width, int height, unsigned char colour);
static void TEDrawCheckeredBox(int x1, int y1, int width, int height, unsigned char colour);
static void TERegFill(TEREGION *TEr, int colour);
static void TERegText(TEREGION *TEr, char *message);
static void TEClearImage(void);
static void TECopyImage(void);
static void TEFlipImage(unsigned char *TERILsrc, int TERILxsize, int TERILysize);
static void TEMirrorImage(unsigned char *TERILsrc, int TERILxsize, int TERILysize);
static void TERotateImageAntiClockwise(unsigned char *TERILsrc, int TERILxsize, int TERILysize);
static void TERotateImageClockwise(unsigned char *TERILsrc, int TERILxsize, int TERILysize);
static void TEFillImage(char *TEFIimage, char *TEFImask, int TEFIxsize, int TEFIysize, int TEFIx, int TEFIy, char TEFIcolour);
static void TERFillImage(char *TERFIimage, char *TERFImask, int TERFIxsize, int TERFIysize, int TERFIx, int TERFIy);
static void TEShiftColours(char *TESCimage, char *TESCmask, int TESCxsize, int TESCysize, int amount);
static void TEUndo(void);
static void DrawButton(TEREGION *TEr);
static void DrawEditReg(TEREGION *TEr);
static void DrawHelpBar(TEREGION *TEr);
static void DrawPalette(TEREGION *TEr);
static void DrawPreview(TEREGION *TEr);
static void DrawSColBox(TEREGION *TEr);
static void DrawSCTxtBx(TEREGION *TEr);
static void DrawSunkRegion(TEREGION *TEr);
void TileEdit(void);


static int MouseOverTEreg(TEREGION *TEr)
  {
  unsigned int testx=mx-TExoffs;
  unsigned int testy=my-TEyoffs;
  unsigned int width=(TEr->x2)-(TEr->x1);
  unsigned int height=(TEr->y2)-(TEr->y1);
  testx-=(TEr->x1);
  testy-=(TEr->y1);

  if (testx >= width || testy >= height) return FALSE;
  return TRUE;
  }

static void DrawButton(TEREGION *TEr)
// -- ric: 20/Jul/98 - generic button routine
  {
  unsigned int x=TEr->x1+TExoffs;
  unsigned int y=TEr->y1+TEyoffs;
  unsigned int width=(TEr->x2)-(TEr->x1);
  unsigned int height=(TEr->y2)-(TEr->y1);

  strcpy(TEtempstr,"Button data not declared.");
  if (!TEr->data)
    err(TEtempstr);
  if (((struct DATABUTTON *)(TEr->data))->infotype!=DITBUTTON)
    err(TEtempstr);

  // draw raised button
  stdwindow(x-1, y-1, x+width+1, y+height+1);
  if (TEr->caption)
    {
    GotoXY(x+(width-(pixels(TEr->caption)))/2, y+(height-6)/2);
    printstring(TEr->caption);
    }
  switch (((struct DATABUTTON *)(TEr->data))->buttontype)
    {
    case CLICKBUTTON:
      if (MouseOverTEreg(TEr) && TEr->bitflags&REGACTIVE)
        {
        if (((struct DATABUTTON *)(TEr->data))->text)
          SetCaption(&TEregs[HELPBAR], (((struct DATABUTTON *)(TEr->data))->text));

        // Handle mouse clicks
        if (TEOldReg==TEr)
          {
          if ((MBStatus[0]|MBStatus[1])==MOUSEPRESSED)
            {
            // draw button down
            FilledBox(x, y, width, height, winbg);
            HLine(x, y, width, darkw);
            VLine(x, y, height, darkw);
            if (TEr->caption)
              {
              GotoXY(x+1+(width-(pixels(TEr->caption)))/2, y+1+(height-6)/2);
              printstring(TEr->caption);
              }
            }
          if ((MBStatus[0]|MBStatus[1])==MOUSERELEASE)
            {
            *(((struct DATABUTTON *)(TEr->data))->bitflag)^=((struct DATABUTTON *)(TEr->data))->bitvalue;
            }
          }
        }
      break;
    case PUSHBUTTON:
      if (*(((struct DATABUTTON *)(TEr->data))->bitflag)&(((struct DATABUTTON *)(TEr->data))->bitvalue))
        {
        // draw button down
        FilledBox(x, y, width, height, winbg);
        HLine(x, y, width, darkw);
        VLine(x, y, height, darkw);
        if (TEr->caption)
          {
          GotoXY(x+1+(width-(pixels(TEr->caption)))/2, y+1+(height-6)/2);
          printstring(TEr->caption);
          }
        }
      if (MouseOverTEreg(TEr) && TEr->bitflags&REGACTIVE)
        {
        if (((struct DATABUTTON *)(TEr->data))->text)
          {
          sprintf(TEtempstr, "Toggle %s on/off", ((struct DATABUTTON *)(TEr->data))->text);
          SetCaption(&TEregs[HELPBAR], TEtempstr);
          }

        // Handle mouse clicks
        if (TEOldReg==TEr)
          {
          if ((MBStatus[0]|MBStatus[1])==MOUSECLICKED)
            {
            *(((struct DATABUTTON *)(TEr->data))->bitflag)^=((struct DATABUTTON *)(TEr->data))->bitvalue;
            }
          }
        }
      break;
    case RADIOBUTTON:
      if (*(((struct DATABUTTON *)(TEr->data))->bitflag)==((struct DATABUTTON *)(TEr->data))->bitvalue)
        {
        // draw button down
        FilledBox(x, y, width, height, winbg);
        HLine(x, y, width, darkw);
        VLine(x, y, height, darkw);
        if (TEr->caption)
          {
          GotoXY(x+1+(width-(pixels(TEr->caption)))/2, y+1+(height-6)/2);
          printstring(TEr->caption);
          }
        }
      if (MouseOverTEreg(TEr) && TEr->bitflags&REGACTIVE)
        {
        if (((struct DATABUTTON *)(TEr->data))->text)
          SetCaption(&TEregs[HELPBAR], (((struct DATABUTTON *)(TEr->data))->text));

        // Handle mouse clicks
        if (TEOldReg==TEr)
          {
          if ((MBStatus[0]|MBStatus[1])==MOUSECLICKED)
            {
            *(((struct DATABUTTON *)(TEr->data))->bitflag)=((struct DATABUTTON *)(TEr->data))->bitvalue;
            }
          }
        }
      break;
    }
  }

static void DrawSunkRegion(TEREGION *TEr)
  {
  unsigned int x=TEr->x1+TExoffs;
  unsigned int y=TEr->y1+TEyoffs;
  unsigned int width=(TEr->x2)-(TEr->x1);
  unsigned int height=(TEr->y2)-(TEr->y1);

//  FilledBox(x, y, width, height, black);
  HLine(x, y, width, darkw);
  VLine(x, y, height, darkw);
  HLine(x+1, y+height-1, width-1, brightw);
  VLine(x+width-1, y+1, height-1, brightw);
  }

static void DrawEditReg(TEREGION *TEr)
  {
  int i,j,i2,j2,c;
  int minx,miny,maxx,maxy;
  int blocksize=8;
  int visblocksize=blocksize;
  char *img;
  TEREGION tempr;
  tempr.x1=TEr->x1+2;
  tempr.x2=TEr->x2-2;
  tempr.y1=TEr->y1+2;
  tempr.y2=TEr->y2-2;

  img=TEimage; //vsp+(256*TEtile);

  TERegFill(TEr, black);
  DrawSunkRegion(TEr);
  FilledBox(TEr->x1+TExoffs+2, TEr->y1+TEyoffs+2, (blocksize<<4), (blocksize<<4), titlebg);

  if (TEbitflags&TEGRID) visblocksize-=1;

  for (j=0; j<TEysize; j++)
    for (i=0; i<TExsize; i++)
      {
      FilledBox(TEr->x1+TExoffs+2+i*blocksize, TEr->y1+TEyoffs+2+j*blocksize, visblocksize, visblocksize, *img++);
      if (TEbitflags&TESHOWMSK && !TEmask[j*TExsize+i])
        TEDrawCheckeredBox(TEr->x1+TExoffs+2+i*blocksize, TEr->y1+TEyoffs+2+j*blocksize, visblocksize, visblocksize, titlebg);
      }

  img=TEimage;
  if (TEr->bitflags&REGACTIVE && !(MouseOverTEreg(&tempr)) && (MBStatus[0]|MBStatus[1])==MOUSERELEASE)
    if (TEr->bitflags&REGTWOCLICK) TEr->bitflags^=REGTWOCLICK;
  if (MouseOverTEreg(&tempr) && TEr->bitflags&REGACTIVE)
    {
    i=(mx-TExoffs-(tempr.x1))/blocksize;
    j=(my-TEyoffs-(tempr.y1))/blocksize;
    c=img[(j*TExsize)+i];
    sprintf(TEtempstr, "Tile: %d, (%02d, %02d), Color: %03d (rgb: %03d/%03d/%03d)", TEtile, i, j, c, pal[(c*3)], pal[(c*3)+1], pal[(c*3)+2]);
    SetCaption(&TEregs[HELPBAR], TEtempstr);
    if (TEbitflags&TEHILIGHT)
      TEDrawHilight(TExoffs+i*blocksize+tempr.x1, TEyoffs+j*blocksize+tempr.y1, visblocksize, visblocksize, white);

    // if pasting draw the image being pasted
    if (TEeditmode==EDPASTE)
      {
      for (j2=0; j2<TEcopyysize; j2++)
        if ((j+j2)<TEysize)
          for (i2=0; i2<TEcopyxsize; i2++)
            if ((i+i2)<TExsize)
              if (TEcmsk[j2*TEcopyxsize+i2])
                if (!(TEbitflags&TESHOWMSK))
                  FilledBox(TEr->x1+TExoffs+2+(i+i2)*blocksize, TEr->y1+TEyoffs+2+(j+j2)*blocksize, visblocksize, visblocksize, TEcopy[j2*TEcopyxsize+i2]);
                else
                  if (TEmask[(j+j2)*TExsize+i+i2])
                    FilledBox(TEr->x1+TExoffs+2+(i+i2)*blocksize, TEr->y1+TEyoffs+2+(j+j2)*blocksize, visblocksize, visblocksize, TEcopy[j2*TEcopyxsize+i2]);
      }
    if (TEeditmode==EDMASK)
      {
      // draw reg
      if (TEr->bitflags&REGTWOCLICK)
        for (j2=0; j2<TEysize; j2++)
          for (i2=0; i2<TEysize; i2++)
            if (((j2>=j && j2<=((struct DATAEDITREG *)(TEr->data))->firsty) ||
                (j2>=((struct DATAEDITREG *)(TEr->data))->firsty && j2<=j)) &&
                ((i2>=i && i2<=((struct DATAEDITREG *)(TEr->data))->firstx) ||
                (i2>=((struct DATAEDITREG *)(TEr->data))->firstx && i2<=i)))
              TEDrawCheckeredBox(TEr->x1+TExoffs+2+i2*blocksize, TEr->y1+TEyoffs+2+j2*blocksize, visblocksize, visblocksize, darkred);

      if ((MBStatus[0]|MBStatus[1])==MOUSECLICKED && (key[SCAN_LSHIFT] || key[SCAN_RSHIFT]) && !(TEr->bitflags&REGTWOCLICK))
        {
        ((struct DATAEDITREG *)(TEr->data))->firstx=i;
        ((struct DATAEDITREG *)(TEr->data))->firsty=j;
        TEr->bitflags^=REGTWOCLICK;
        }
      if ((MBStatus[0]|MBStatus[1])==MOUSERELEASE && TEr->bitflags&REGTWOCLICK)
        {
        if (MBStatus[1]==MOUSERELEASE) c=0x0;
        else c=0xf;
        miny=j;
        maxy=((struct DATAEDITREG *)(TEr->data))->firsty;
        minx=i;
        maxx=((struct DATAEDITREG *)(TEr->data))->firstx;
        if (j>((struct DATAEDITREG *)(TEr->data))->firsty)
          {
          maxy=j;
          miny=((struct DATAEDITREG *)(TEr->data))->firsty;
          }
        if (i>((struct DATAEDITREG *)(TEr->data))->firstx)
          {
          maxx=i;
          minx=((struct DATAEDITREG *)(TEr->data))->firstx;
          }
        for (j2=miny; j2<=maxy; j2++)
          for (i2=minx; i2<=maxx; i2++)
            TEmask[j2*TExsize+i2]=(char)c;
        TEr->bitflags^=REGTWOCLICK;
        }
      }
    // Handle mouse clicks

#ifdef JUNK
    if ((MBStatus[0]|MBStatus[1])==MOUSECLICKED && (key[SCAN_CTRL]||key[SCAN_S]))
      {
      // put code to lock out draw mode here
      }
#endif
    if (TEOldReg==TEr)
      {
      // Update undo buffer
      if ((MBStatus[0]|MBStatus[1])==MOUSECLICKED && TEeditmode!=EDMASK && TEeditmode!=EDPICK && !(key[SCAN_CTRL]||key[SCAN_S]))
        memcpy(TEundo, TEimage, TExsize*TEysize);
      if (MBStatus[0]==MOUSECLICKED && TEeditmode==EDFILL)
        TEFillImage(TEimage, (TEbitflags&TESHOWMSK ? TEmask : NULL), TExsize, TEysize, i, j, lcolor);
      if (MBStatus[1]==MOUSECLICKED && TEeditmode==EDFILL)
        TEFillImage(TEimage, (TEbitflags&TESHOWMSK ? TEmask : NULL), TExsize, TEysize, i, j, rcolor);

      // Draw
      if ((MBStatus[0]|MBStatus[1])==MOUSEPRESSED)
        {
        if (TEeditmode==EDPASTE)
          {
          for (j2=0; j2<TEcopyysize; j2++)
            if ((j+j2)<TEysize)
              for (i2=0; i2<TEcopyxsize; i2++)
                if ((i+i2)<TExsize)
                  if (TEcmsk[j2*TEcopyxsize+i2])
                    if (!(TEbitflags&TESHOWMSK))
                      TEimage[(j+j2)*TExsize+i+i2]=TEcopy[j2*TEcopyxsize+i2];
                    else
                      if (TEmask[(j+j2)*TExsize+i+i2])
                        TEimage[(j+j2)*TExsize+i+i2]=TEcopy[j2*TEcopyxsize+i2];
          }
        }
      if (MBStatus[0]==MOUSEPRESSED)
        if (key[SCAN_CTRL]||key[SCAN_S]||(TEeditmode==EDPICK))
          lcolor=TEimage[(j*TExsize)+i];
        else
          if ((TEeditmode==EDDRAW)&&!(TEbitflags&TESHOWMSK && !TEmask[j*TExsize+i]))
            TEimage[(j*TExsize)+i]=lcolor;
          else
            if (TEeditmode==EDMASK && !(TEr->bitflags&REGTWOCLICK))
              TEmask[(j*TExsize)+i]=0xf;
      if (MBStatus[1]==MOUSEPRESSED)
        if (key[SCAN_CTRL]||key[SCAN_S]||(TEeditmode==EDPICK))
          rcolor=TEimage[(j*TExsize)+i];
        else
          if ((TEeditmode==EDDRAW)&&!(TEbitflags&TESHOWMSK && !TEmask[j*TExsize+i]))
            TEimage[(j*TExsize)+i]=rcolor;
          else
            if (TEeditmode==EDMASK && !(TEr->bitflags&REGTWOCLICK))
              TEmask[(j*TExsize)+i]=0x0;
      }
    }
  }

static void DrawHelpBar(TEREGION *TEr)
  {
  //int i, htext=0;
  TERegFill(TEr, black);
  DrawSunkRegion(TEr);
  if (TEr->caption) TERegText(TEr, TEr->caption);
  TEFree(TEr->caption);               // Blank caption
  //TEr->caption=NULL;                  // why doesn't this work in TEFree?
#ifdef JUNK
  i=MAX_REGIONS;
  do
    {
    i--;
    if (MouseOverTEreg(&TEregs[i])) htext=i;
    } while (i>0);
  if (htext && TEregs[htext].helptext) TERegText(TEr, TEr->caption);
#endif
  }

static void DrawPalette(TEREGION *TEr)
  {
  int i,j;
  unsigned int width=(TEr->x2)-(TEr->x1);
  unsigned int height=(TEr->y2)-(TEr->y1);
  int xscale=(width-4)>>6;
  int yscale=(height-4)>>2;
  int x=(TEr->x1)+TExoffs;
  int y=(TEr->y1)+TEyoffs;
  TEREGION tempr;
  tempr.x1=TEr->x1+2;
  tempr.x2=TEr->x2-2;
  tempr.y1=TEr->y1+2;
  tempr.y2=TEr->y2-2;

  x+=2;
  y+=2;
  TERegFill(TEr, black);
  DrawSunkRegion(TEr);
  i=64;
  do
    {
    i--;
    FilledBox(x+(i*xscale), y, xscale, yscale, i);
    FilledBox(x+(i*xscale), y+yscale, xscale, yscale, i+64);
    FilledBox(x+(i*xscale), y+yscale*2, xscale, yscale, i+128);
    FilledBox(x+(i*xscale), y+yscale*3, xscale, yscale, i+192);
    } while (i>0);

  i=lcolor&63;
  j=lcolor>>6;
  TEDrawHilight(x+i*xscale, y+j*yscale, xscale, yscale, titlebg);

  i=rcolor&63;
  j=rcolor>>6;
  TEDrawHilight(x+i*xscale, y+j*yscale, xscale, yscale, titlebg);

  if (MouseOverTEreg(&tempr) && TEr->bitflags&REGACTIVE)
    {
    i=(mx-TExoffs-(tempr.x1))/xscale;
    j=(my-TExoffs-(tempr.y1))/yscale;
    i+=(j<<6);
    j=i*3;
    sprintf(TEtempstr, "Color: %03d (rgb: %03d/%03d/%03d)", i, pal[j], pal[j+1], pal[j+2]);
    SetCaption(&TEregs[HELPBAR], TEtempstr);
    // Handle mouse clicks
    if (TEOldReg==TEr)
      {
      if (MBStatus[0]==MOUSEPRESSED) lcolor=i;
      if (MBStatus[1]==MOUSEPRESSED) rcolor=i;
      }
    }
  }

static void DrawPreview(TEREGION *TEr)
  {
  char *img;
  int i;
  TERegFill(TEr, black);
  DrawSunkRegion(TEr);
//  img=vsp+(256*TEtile);
  img=TEimage;
  i=tileidx[TEtile];
  if ((TEbitflags&TEANIMATE) && i!=TEtile) img=vsp+(256*i);
  CopyTile(TEr->x1+TExoffs+2, TEr->y1+TEyoffs+2, img);

  if (MouseOverTEreg(TEr) && TEr->bitflags&REGACTIVE)
    {
    strcpy(TEtempstr, (TEbitflags&TEANIMATE ? "Click to halt animation" : "Click to animate tile"));
    SetCaption(&TEregs[HELPBAR], TEtempstr);

    // Handle mouse clicks
    if (TEOldReg==TEr)
      if ((MBStatus[0]|MBStatus[1])==MOUSECLICKED && MouseOverTEreg(TEr))
        TEbitflags^=TEANIMATE;
    }
  }

static void DrawSColBox(TEREGION *TEr)
// -- ric: 15/Jul/98 - Draw selected colour box
  {
  int halfwidth=TEr->x2-TEr->x1-4;
  int height=TEr->y2-TEr->y1-4;
  char DSCBtempcolor;
  TERegFill(TEr, black);
  DrawSunkRegion(TEr);
  FilledBox(TExoffs+2+TEr->x1, TEyoffs+2+TEr->y1, halfwidth, height, titlebg);
  height-=2;
  halfwidth-=2;
  halfwidth>>=1;
  FilledBox(TExoffs+3+TEr->x1, TEyoffs+3+TEr->y1, halfwidth, height, lcolor);
  FilledBox(TExoffs+3+TEr->x1+halfwidth, TEyoffs+3+TEr->y1, halfwidth, height, rcolor);

  if (MouseOverTEreg(TEr) && TEr->bitflags&REGACTIVE)
    {
    strcpy(TEtempstr, "Click to swap colors");
    SetCaption(&TEregs[HELPBAR], TEtempstr);

    // Handle mouse clicks
    if (TEOldReg==TEr)
    if ((MBStatus[0]|MBStatus[1])==MOUSECLICKED && MouseOverTEreg(TEr))
      {
      DSCBtempcolor=lcolor;
      lcolor=rcolor;
      rcolor=DSCBtempcolor;
      }
    }
  }

static void DrawSCTxtBx(TEREGION *TEr)
// -- ric: 15/Jul/98 - Draw selected colour text box
  {
//  char DSCTBTextBuf[256];
  int i;
  int DSCTBx=TEr->x1+TExoffs+2;
  int DSCTBy=TEr->y1+TEyoffs+2;
  TERegFill(TEr, black);
  DrawSunkRegion(TEr);
  i=lcolor*3;
  sprintf(TEtempstr, "Left: %03d (rgb: %03d/%03d/%03d)", lcolor, pal[i], pal[i+1], pal[i+2]);
  GotoXY(DSCTBx, DSCTBy);
  printstring(TEtempstr);
  i=rcolor*3;
  sprintf(TEtempstr, "Right: %03d (rgb: %03d/%03d/%03d)", rcolor, pal[i], pal[i+1], pal[i+2]);
  GotoXY(DSCTBx, DSCTBy+7);
  printstring(TEtempstr);
  }

static void SetCaption(TEREGION *TEr, char *helptext)
  {
  TEFree(TEr->caption);
  TEr->caption=(char *)strcpy((char *)TEAlloc(strlen(helptext)+1,helptext), helptext);
  }

static void TEDrawHilight(int x1, int y1, int width, int height, unsigned char colour)
// -- ric: 15/Jul/98 - Draws hilight around given rectangle
  {
  VLine(x1-1, y1, height, colour);
  VLine(x1+width, y1, height, colour);
  HLine(x1-1, y1-1, width+2, colour);
  HLine(x1-1, y1+height, width+2, colour);
  }

static void TEDrawCheckeredBox(int x1, int y1, int width, int height, unsigned char colour)
// -- ric: 17/Jul/98 - Draws a checkered box
  {
  int i,j;
  unsigned char c;
  for (j=0; j<height; j++)
   {
   c=(y1+j+1)&1;
   if (c) c=colour;
   for (i=0; i<width; i++)
     {
     if (c) screen[(y1+j)*tsx+x1+i]=c;
     c^=colour;
     }
   }
  }

static void TERegFill(TEREGION *TEr, int colour)
// -- ric: 20/Jul/98 - Fills region with colour (colour==-1 for transparent)
  {
  if (colour>-1)
    FilledBox(TEr->x1+TExoffs, TEr->y1+TEyoffs, (TEr->x2)-(TEr->x1), (TEr->y2)-(TEr->y1), colour);
  }

static void TERegText(TEREGION *TEr, char *message)
// Prints given message in the specified region
  {
  int tempx, tempy;
  tempx=(TEr->x1)+TExoffs+2;

  tempy=(TEr->y2)-(TEr->y1);
  tempy>>=1;
  tempy+=(TEr->y1)-3;

  GotoXY(tempx,tempy+TEyoffs);
  printstring(message);

  }

static void TEClearImage(void)
// -- ric: 26/Jul/98 - clear selected image
  {
  int j;
  if (TEbitflags&TESHOWMSK)
    {
    j=TEysize*TExsize;
    memcpy(TEundo, TEimage, j); // backup image to undo buffer
    do
      {
      j--;
      if (*(TEmask+j)) *(TEimage+j)=rcolor;
      } while (j>0);
    }
  }

static void TECopyImage(void)
// -- ric: 26/Jul/98 - copy selected image to copy buffer
  {
  int i, j, TECIxstart, TECIystart, TECIxsize, TECIysize;
  if (TEbitflags&TESHOWMSK)  // only makes sense if selected region is shown
    {
    TECIxstart=TExsize;
    TECIystart=TEysize;
    TECIxsize=-1;
    TECIysize=-1;
    // loop through image to find the starting and ending x and y coords
    // of the image to copy
    for (j=0; j<TEysize; j++)
      for (i=0; i<TExsize; i++)
        {
        if (TEmask[j*TExsize+i])   // selected
          {
          if (i<TECIxstart) TECIxstart=i;
          if (i>TECIxsize) TECIxsize=i;
          if (j<TECIystart) TECIystart=j;
          if (j>TECIysize) TECIysize=j;
          }
        }

    TECIxsize-=TECIxstart-1;
    TECIysize-=TECIystart-1;

    if (TECIxsize>0 && TECIysize>0)
      {
      // free and create TEcopy and TEcmsk buffers
      TEFree(TEcopy);
      TEFree(TEcmsk);

      // make copy buffer square (allows for rotation)
      TEcopyxsize=TECIysize;
      TEcopyysize=TECIysize;
      if (TECIxsize>TECIysize)
        {
        TEcopyxsize=TECIxsize;
        TEcopyysize=TECIxsize;
        }
      TEcopy=(char *)TEAlloc(TEcopyxsize*TEcopyysize, "copy buffer");
      TEcmsk=(char *)TEAlloc(TEcopyxsize*TEcopyysize, "copy mask");

      // copy image into buffer
      for (j=0; j<TECIysize; j++)
        for (i=0; i<TECIxsize; i++)
          {
          if (TEmask[(j+TECIystart)*TExsize+(i+TECIxstart)])
            {
            TEcopy[j*TEcopyxsize+i]=TEimage[(j+TECIystart)*TExsize+(i+TECIxstart)];
            TEcmsk[j*TEcopyxsize+i]=0xf;
            }
          }
      }
    }
  }

static void TERotateImageAntiClockwise(unsigned char *TERILsrc, int TERILxsize, int TERILysize)
// -- ric: 15/Jul/98 - rotates image (must be square) 90 degrees anti-clockwise
  {
//  unsigned char *TERILsrc=TEimage;
  unsigned char *TERILimg=NULL;
  int i, j, amount;
  if (TERILxsize==TERILysize)
    {
    amount=TERILxsize*TERILysize;
    TERILimg=(char *) TEAlloc(amount, "temporary image buffer for TERotateImageAntiClockwise");
    memcpy(TERILimg, TERILsrc, amount);
    j=TERILysize;
    do
      {
      j--;
      i=TERILxsize;
      do
        {
        i--;
        TERILsrc[(j*TERILxsize)+i]=TERILimg[(i*TERILxsize)+(TERILysize-1-j)];
        } while (i>0);
      } while (j>0);
    TEFree(TERILimg);
    }
  }

static void TERotateImageClockwise(unsigned char *TERILsrc, int TERILxsize, int TERILysize)
// -- ric: 16/Jul/98 - rotates image (must be square) 90 degrees clockwise
  {
  unsigned char *TERILimg=NULL;
  int i, j, amount;
  if (TERILxsize==TERILysize)
    {
    amount=TERILxsize*TERILysize;
    TERILimg=(char *) TEAlloc(amount, "temporary image buffer for TERotateImageClockwise");
    memcpy(TERILimg, TERILsrc, amount);
    j=TERILysize;
    do
      {
      j--;
      i=TERILxsize;
      do
        {
        i--;
        TERILsrc[(j*TERILxsize)+i]=TERILimg[((TERILxsize-1-i)*TERILxsize)+(j)];
        } while (i>0);
      } while (j>0);
    TEFree(TERILimg);
    }
  }

static void TEFlipImage(unsigned char *TERILsrc, int TERILxsize, int TERILysize)
// -- ric: 15/Jul/98 - flips image
  {
  unsigned char *TERILimg=NULL;
  int i, j, amount;
  amount=TERILxsize*TERILysize;
  TERILimg=(char *) TEAlloc(amount, "temporary image buffer for TEFlipImage");
  memcpy(TERILimg, TERILsrc, amount);
  j=TERILysize;
  do
    {
    j--;
    i=TERILxsize;
    do
      {
      i--;
      TERILsrc[(j*TERILxsize)+i]=TERILimg[((TERILysize-1-j)*TERILxsize)+i];
      } while (i>0);
    } while (j>0);
  TEFree(TERILimg);
  }

static void TEMirrorImage(unsigned char *TERILsrc, int TERILxsize, int TERILysize)
// -- ric: 15/Jul/98 - mirrors image
  {
  unsigned char *TERILimg=NULL;
  int i, j, amount;
  amount=TERILxsize*TERILysize;
  TERILimg=(char *) TEAlloc(amount, "temporary image buffer for TEMirrorImage");
  memcpy(TERILimg, TERILsrc, amount);
  j=TERILysize;
  do
    {
    j--;
    i=TERILxsize;
    do
      {
      i--;
      TERILsrc[(j*TERILxsize)+i]=TERILimg[(j*TERILxsize)+(TERILxsize-1-i)];
      } while (i>0);
    } while (j>0);
  TEFree(TERILimg);
  }

static void TEFillImage(char *TEFIimage, char *TEFImask, int TEFIxsize, int TEFIysize, int TEFIx, int TEFIy, char TEFIcolour)
// -- ric: 29/Jul/98 - Fill procedure - sets up and calls recursive procedure below
  {
  int i,j;
  unsigned char *TEFIfillmask=NULL;  // will eventually contain 0 for ignore and 1 for fill
  TEFIfillmask=(char *) TEAlloc(TEFIxsize*TEFIysize, "temporary fill buffer for TEFillImage");
  TERFillImage(TEFIimage, TEFIfillmask, TEFIxsize, TEFIysize, TEFIx, TEFIy);

  if (TEFImask)
    for (j=0; j<TEFIysize; j++)
      for (i=0; i<TEFIxsize; i++)
        TEFIfillmask[j*TEFIxsize+i]&=TEFImask[j*TEFIxsize+i];

  for (j=0; j<TEFIysize; j++)
    for (i=0; i<TEFIxsize; i++)
      if (TEFIfillmask[j*TEFIxsize+i]) TEFIimage[j*TEFIxsize+i]=TEFIcolour;
  TEFree(TEFIfillmask);
  }

static void TERFillImage(char *TERFIimage, char *TERFImask, int TERFIxsize, int TERFIysize, int TERFIx, int TERFIy)
// -- ric: 29/Jul/98 - Recursive fill procedure
  {
  int x,y;
  char TERFIcc;
  TERFImask[TERFIy*TERFIxsize+TERFIx]=0xf;
  TERFIcc=TERFIimage[TERFIy*TERFIxsize+TERFIx];
  x=TERFIx-1;
  y=TERFIy;
  if (x>=0 && x<TERFIxsize && y>=0 && y<TERFIysize)
    if (!(TERFImask[y*TERFIxsize+x]) && TERFIimage[y*TERFIxsize+x]==TERFIcc)
      TERFillImage(TERFIimage, TERFImask, TERFIxsize, TERFIysize, x, y);
  x=TERFIx;
  y=TERFIy-1;
  if (x>=0 && x<TERFIxsize && y>=0 && y<TERFIysize)
    if (!(TERFImask[y*TERFIxsize+x]) && TERFIimage[y*TERFIxsize+x]==TERFIcc)
      TERFillImage(TERFIimage, TERFImask, TERFIxsize, TERFIysize, x, y);
  x=TERFIx+1;
  y=TERFIy;
  if (x>=0 && x<TERFIxsize && y>=0 && y<TERFIysize)
    if (!(TERFImask[y*TERFIxsize+x]) && TERFIimage[y*TERFIxsize+x]==TERFIcc)
      TERFillImage(TERFIimage, TERFImask, TERFIxsize, TERFIysize, x, y);
  x=TERFIx;
  y=TERFIy+1;
  if (x>=0 && x<TERFIxsize && y>=0 && y<TERFIysize)
    if (!(TERFImask[y*TERFIxsize+x]) && TERFIimage[y*TERFIxsize+x]==TERFIcc)
      TERFillImage(TERFIimage, TERFImask, TERFIxsize, TERFIysize, x, y);
  }

static void TEShiftColours(char *TESCimage, char *TESCmask, int TESCxsize, int TESCysize, int amount)
// -- ric: 17/Jul/98 - Colour shift idea by McGrue and Tarkuss
  {
  int i,j;
  j=TESCysize;
  do
    {
    j--;
    i=TESCxsize;
    do
      {
      i--;
      if (!(TESCmask))
        TESCimage[j*TESCxsize+i]+=amount;
      else
        if (TESCmask[j*TESCxsize+i])
          TESCimage[j*TESCxsize+i]+=amount;
      } while (i>0);
    } while (j>0);
  }


static void TEUndo (void)
  {
  unsigned char *tempimg=TEimage;
  TEimage=TEundo;
  TEundo=tempimg;
  }

static void *TEAlloc(unsigned long amount, char *whatfor)
// -- ric: 14/Jul/98 - TE's custom, error-handling memory allocation routine
  {
  char *TETmpBuf=NULL;

  amount += 3; // aen
  amount &= ~3;

//  static char DebugBuf[256];
  TETmpBuf=(char *) malloc(amount);
  if (!TETmpBuf)
    {
    sprintf(TEtempstr, "Unable to allocate %ld bytes for %s.", amount, whatfor);
    err(TEtempstr);
    }
  memset(TETmpBuf,0, amount); // Clear allocated mem
  /*
  __asm {  // aen
    mov edi,TETmpBuf
    mov ecx,amount
    shr ecx,2
    xor eax,eax
    cld
    rep stosd
    } ;
  */
  return TETmpBuf;
  }

#ifdef JUNK
static void TEFree(void *TETmpBuf)
// -- ric: 14/Jul/98 - TE's custom memory freeing routine
  {
  if (TETmpBuf) free(TETmpBuf);  // Only free memory if previously allocated
  TETmpBuf=NULL;                 // Set freed memory to point to NULL
  }
#endif

static void MBClickHandler(TEREGION *TEBaseReg)
// -- ric: 20/Jul/98 - Mouse button click handler
  {
  int i,j;
  ReadMouse();
  if (TEBaseReg)
    {
    if (!TEBaseReg->data) err("No region data defined.");
    if (((struct DATABASEREGION *)TEBaseReg->data)->infotype!=DITBASEREGION)
      err("MBClickHandler called with invalid base region.");
    }

  for (i=0; i<3; ++i)
    {
    if (mb & (i+1))
      {
      if (MBStatus[i] == MOUSECLICKED)
        MBStatus[i]=MOUSEPRESSED;
      else if (MBStatus[i] == MOUSENONE)
        {
        MBStatus[i] = MOUSECLICKED;
        if (TEBaseReg)
          {
          TEOldReg = NULL;
          for (j=0; j<((struct DATABASEREGION *)TEBaseReg->data)->numofregs; ++j)
            if (MouseOverTEreg(TEBaseReg+j)&&!((TEBaseReg+j)->bitflags&REGNOCLICK))
              TEOldReg = TEBaseReg+j;
          }
        }
      }
    else
      {
      if (MBStatus[i] <= MOUSERELEASE)
        MBStatus[i] = MOUSENONE;
      else if (MBStatus[i]>MOUSERELEASE)
        MBStatus[i] = MOUSERELEASE;
      }
    }
  }

static void AboutTE (void)
// Just here to test the mouse input loop - remove this later
  {
  TEREGION *TEr=NULL;
  TEREGION *TEaboutwin=NULL;
//  int amxsize=150, amysize=82;
  int amxofs=60, amyofs=60;
  int i,i2,j,j2;
  int QuitAW=FALSE;
  int moving=FALSE;

  // Allocate memory
  TEaboutwin=(struct TEREGION *) TEAlloc((sizeof (struct TEREGION))*4,"about window regions");
  TEr=TEaboutwin;
  TEr->infotype=DITREGION;
  TEr->x1=amxofs;
  TEr->x2=TEr->x1+150;
  TEr->y1=amyofs;
  TEr->y2=TEr->y1+82;
  TEr->bitflags=REGACTIVE|REGVISIBLE|REGNOCLICK;
//  TEr->drawproc=NULL;
  sprintf(strbuf, "About MapEd %s Tile Editor", ME2_VERSION);
  TEr->caption=TENewText(strbuf, "About window");
  TEr->data=(struct DATABASEREGION *) TEAlloc(sizeof (struct DATABASEREGION), "base region data");
  ((struct DATABASEREGION *)(TEr->data))->infotype=DITBASEREGION;
  ((struct DATABASEREGION *)(TEr->data))->numofregs=4;
  ((struct DATABASEREGION *)(TEr->data))->windowtype=DBRSTDWINDOW;

  TEr+=1;
  TEr->infotype=DITREGION;
  TEr->x1=0;
  TEr->x2=TEaboutwin->x2-TEaboutwin->x1-9;
  TEr->y1=0;
  TEr->y2=8;
  TEr->bitflags=REGACTIVE;
//  TEr->drawproc=NULL;
//  TEr->caption=NULL;

  TEr+=1;
  TEr->infotype=DITREGION;
  TEr->x1=TEaboutwin->x2-TEaboutwin->x1-9;
  TEr->x2=TEaboutwin->x2-TEaboutwin->x1-3;
  TEr->y1=3;
  TEr->y2=6;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawButton;
//  TEr->caption=NULL;
  TEr->data=(struct DATABUTTON *) TEAlloc(sizeof (struct DATABUTTON), "quit button data");
  ((struct DATABUTTON *)(TEr->data))->infotype=DITBUTTON;
  ((struct DATABUTTON *)(TEr->data))->text=NULL;
  ((struct DATABUTTON *)(TEr->data))->buttontype=CLICKBUTTON;
  ((struct DATABUTTON *)(TEr->data))->bitflag=&QuitAW;
  ((struct DATABUTTON *)(TEr->data))->bitvalue=TRUE;

  TEr+=1;
  TEr->infotype=DITREGION;
  TEr->x1=60;
  TEr->x2=90;
  TEr->y1=62;
  TEr->y2=72;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawButton;
  TEr->caption=TENewText("ok","ok button caption");
  TEr->data=(struct DATABUTTON *) TEAlloc(sizeof (struct DATABUTTON), "quit button data");
  ((struct DATABUTTON *)(TEr->data))->infotype=DITBUTTON;
  ((struct DATABUTTON *)(TEr->data))->text=NULL;
  ((struct DATABUTTON *)(TEr->data))->buttontype=CLICKBUTTON;
  ((struct DATABUTTON *)(TEr->data))->bitflag=&QuitAW;
  ((struct DATABUTTON *)(TEr->data))->bitvalue=TRUE;
  TEr=TEaboutwin+1; // point to title bar area

  while (!QuitAW)
    {
    // Mouse button click handler routines
    MBClickHandler(TEaboutwin);

    amxofs=TEaboutwin->x1;
    amyofs=TEaboutwin->y1;
    if ((MBStatus[0]|MBStatus[1])==MOUSERELEASE)
      moving=FALSE;

    // Draw loop
    RenderTileEdit(TEregs);
    RenderTileEdit(TEaboutwin);

    sprintf(strbuf, "MapEd v.%s", ME2_VERSION);
    GotoXY(amxofs+52, amyofs+16); printstring(strbuf);
    GotoXY(amxofs+31, amyofs+22); printstring("Copyright (C) 1998 vecna");
    GotoXY(amxofs+40, amyofs+28); printstring("All Rights Reserved");
    sprintf(strbuf, "MapEd v.%s Tile Editor", ME2_VERSION);
    GotoXY(amxofs+29, amyofs+41); printstring(strbuf);
    GotoXY(amxofs+20, amyofs+47); printstring("Additional code by Richard Lau");
    DrawMouse();
    ShowPage();

    // Move window?
    if (MouseOverTEreg(TEr) && TEOldReg==TEr)
      {
      if ((MBStatus[0]|MBStatus[1])==MOUSECLICKED)
        {
        i2=TEaboutwin->x2-TEaboutwin->x1;
        j2=TEaboutwin->y2-TEaboutwin->y1;
        i=mx-TEaboutwin->x1;
        j=my-TEaboutwin->y1;
        moving=TRUE;
        }
      }
      if ((MBStatus[0]|MBStatus[1])==MOUSEPRESSED && moving)
        {
        if (((mx-i)<16)) TEaboutwin->x1=16;
        else if (((mx-i+i2)>=(16+sx))) TEaboutwin->x1=16+(sx-i2);
        else TEaboutwin->x1=mx-i;
        if (((my-j)<16)) TEaboutwin->y1=16;
        else if (((my-j+j2)>=(16+sy))) TEaboutwin->y1=16+(sy-j2);
        else TEaboutwin->y1=my-j;
        TEaboutwin->x2=TEaboutwin->x1+i2;
        TEaboutwin->y2=TEaboutwin->y1+j2;
        }

    if (key[SCAN_ESC])
      {
      key[SCAN_ESC]=0;
      QuitAW=TRUE;
      }

    }
  // free TEaboutwin
  TEFreeReg(TEaboutwin);
  }

static void RestartTileEditor(void)
// -- ric: 17/Jul/98 - reinit the tile editor
  {
  int i,j, amount;
  unsigned char *TERTEmask=NULL;

  switch (TEtype)
    {
    case TETILE:                 // edit tile TEtile
          i=TExsize;
          j=TEysize;
          amount=i*j;
          TERTEmask=(unsigned char *) TEAlloc(amount, "tile editor mask backup buffer");

          // backup mask
          memcpy(TERTEmask, TEmask, amount);

          // Restart editor
          DeinitTEImage();
          TEFreeReg(TEregs);
          TEtype=TETILE;
          TExsize=i;
          TEysize=j;
          TEsource=vsp+(256*TEtile);
          InitTEImage();
          SetupRegions();

          // Restore mask
          memcpy(TEmask, TERTEmask, amount);
          TEFree(TERTEmask);
          break;
    }
  }

static void InitTEImage (void)
// Copy the image data being edited and allocate memory for the image buffers
  {
  int i;
  int amount=TExsize*TEysize;
  if (TEtype==TEUNKNOWN) err("Unrecognised tile editor graphics type.");
  if (!TEsource) err("Invalid input image.");
  TEimage=(unsigned char *) TEAlloc(amount, "tile editor image buffer");
  TEmask=(unsigned char *) TEAlloc(amount, "tile editor mask buffer");
  TEundo=(unsigned char *) TEAlloc(amount, "tile editor undo buffer");
//  if (!TEimage || !TEundo) err("Cannot allocate memory for tile editor image buffers.");
  memcpy(TEimage, TEsource, amount); // copy original image into edit buffer
  memcpy(TEundo, TEimage, amount);   // copy original image into undo buffer

  // if editing tile find the animation strand (if any) tile belongs to
  TEanimstrand=0;       // Does not belong to an animation strand
  if (TEtype==TETILE)
    {
    i=100;
    do
      {
      i--;
      if (vspanim[i].delay && TEtile<=vspanim[i].finish && TEtile>=vspanim[i].start)
        TEanimstrand=i+1;
      } while (i>0);
    }
  }

static void DeinitTEImage(void)
// Unallocate allocated image buffers - copy edited buffer into original
  {
  memcpy(TEsource, TEimage, (TExsize*TEysize));
  TEFree(TEimage);
  TEFree(TEmask);
  TEFree(TEundo);
  TEtype=TEUNKNOWN;         // Clear type info for error checking purposes
  TExsize=0;
  TEysize=0;
  }

static void SetupRegions (void)
  {
  int temp;
  TEREGION *TEr=NULL;

  // Allocate memory
  TEregs=(struct TEREGION *) TEAlloc((sizeof (struct TEREGION))*MAX_REGIONS,"tile edit regions");

  TEr=TEregs+BASEREG;
  TEr->infotype=DITREGION;
  TEr->x1=16;
  TEr->x2=TEr->x1+sx;
  TEr->y1=16;
  TEr->y2=TEr->y1+sy;
  TEr->bitflags=REGACTIVE|REGVISIBLE|REGNOCLICK;
  TEr->drawproc=NULL;
  TEr->caption=NULL;
  switch (TEtype)
    {
    case TETILE:
      sprintf(TEtempstr, "MapEd %s Tile Editor - Tile: %d", ME2_VERSION, TEtile);
      if (TEanimstrand)
        sprintf(TEtempstr, "MapEd %s Tile Editor - Tile: %d (anim strand: %d, frame: %d/%d)",
          ME2_VERSION,
          TEtile, TEanimstrand-1, TEtile-vspanim[TEanimstrand-1].start+1,
          vspanim[TEanimstrand-1].finish-vspanim[TEanimstrand-1].start+1);
      SetCaption(TEr, TEtempstr);
      break;
    }
  TEr->data=(struct DATABASEREGION *) TEAlloc(sizeof (struct DATABASEREGION), "base region data");
  ((struct DATABASEREGION *)(TEr->data))->infotype=DITBASEREGION;
  ((struct DATABASEREGION *)(TEr->data))->numofregs=MAX_REGIONS;
  ((struct DATABASEREGION *)(TEr->data))->windowtype=DBRSTDWINDOW;

  TEr=TEregs+HELPBAR;
  TEr->infotype=DITREGION;
  TEr->x1=3;
  TEr->x2=TEregs->x2-TEregs->x1-3;
  TEr->y1=TEregs->y2-TEregs->y1-13;
  TEr->y2=TEregs->y2-TEregs->y1-3;
  TEr->bitflags=REGACTIVE|REGVISIBLE|REGNOCLICK;
  TEr->drawproc=&DrawHelpBar;
  TEr->caption=NULL;
  TEr->data=NULL;

  temp=(((TEregs->x2-TEregs->x1-8)>>6)<<5)+2;
  TEr=TEregs+PALETTE;
  TEr->infotype=DITREGION;
  TEr->x1=((TEregs->x2-TEregs->x1)>>1)-temp;
  TEr->x2=((TEregs->x2-TEregs->x1)>>1)+temp;
  TEr->y1=TEregs->y2-TEregs->y1-36;
  TEr->y2=TEregs->y2-TEregs->y1-16;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawPalette;
  TEr->caption=NULL;
  TEr->data=NULL;

  TEr=TEregs+EDITREG;
  TEr->infotype=DITREGION;
  TEr->x1=3;
  TEr->x2=3+128+4;
  TEr->y1=10;
  TEr->y2=10+128+4;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawEditReg;
  TEr->caption=NULL;
  TEr->data=(struct DATAEDITREG *) TEAlloc(sizeof (struct DATAEDITREG), "edit region data");
  ((struct DATAEDITREG *)(TEr->data))->infotype=DITEDITREG;

  TEr=TEregs+PREVIEW;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs->x2-TEregs->x1-23;
  TEr->x2=TEregs->x2-TEregs->x1-3;
  TEr->y1=10;
  TEr->y2=10+20;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawPreview;
  TEr->caption=NULL;
  TEr->data=NULL;

  TEr=TEregs+SCOLBOX;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[EDITREG].x1;
  TEr->x2=TEr->x1+20;
  TEr->y1=TEregs[EDITREG].y2+2;
  TEr->y2=TEr->y1+16;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawSColBox;
  TEr->caption=NULL;
  TEr->data=NULL;

  TEr=TEregs+SCTXTBX;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[SCOLBOX].x2+2;
  TEr->x2=TEregs[EDITREG].x2;
  TEr->y1=TEregs[SCOLBOX].y1;
  TEr->y2=TEregs[SCOLBOX].y2;
  TEr->bitflags=REGACTIVE|REGVISIBLE|REGNOCLICK;
  TEr->drawproc=&DrawSCTxtBx;
  TEr->caption=NULL;
  TEr->data=NULL;

  TEr=TEregs+QUITBUT;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs->x2-TEregs->x1-9;
  TEr->x2=TEregs->x2-TEregs->x1-3;
  TEr->y1=3;
  TEr->y2=6;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawButton;
  TEr->caption=NULL;
  TEr->data=(struct DATABUTTON *) TEAlloc(sizeof (struct DATABUTTON), "quit button data");
  ((struct DATABUTTON *)(TEr->data))->infotype=DITBUTTON;
  ((struct DATABUTTON *)(TEr->data))->text=TENewText("Exit", "quit button text");
  ((struct DATABUTTON *)(TEr->data))->buttontype=CLICKBUTTON;
  ((struct DATABUTTON *)(TEr->data))->bitflag=&TEbitflags;
  ((struct DATABUTTON *)(TEr->data))->bitvalue=TEQUITCUR;

  TEr=TEregs+EDMDFRM;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[EDITREG].x2+3;
  TEr->x2=TEr->x1+34;
  TEr->y1=TEregs[EDITREG].y1;
  TEr->y2=TEr->y1+11*5+3;     // 10*number of buttons
  TEr->bitflags=REGACTIVE|REGVISIBLE|REGNOCLICK;
  TEr->drawproc=&DrawSunkRegion;
  TEr->caption=NULL;
  TEr->data=NULL;

  TEr=TEregs+DRAWBUT;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[EDMDFRM].x1+2;
  TEr->x2=TEregs[EDMDFRM].x2-2;
  TEr->y1=TEregs[EDMDFRM].y1+2;
  TEr->y2=TEr->y1+10;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawButton;
  TEr->caption=NULL;
  SetCaption(TEr,"Draw");
  TEr->data=(struct DATABUTTON *) TEAlloc(sizeof (struct DATABUTTON), "draw button data");
  ((struct DATABUTTON *)(TEr->data))->infotype=DITBUTTON;
  ((struct DATABUTTON *)(TEr->data))->text=TENewText("Draw mode", "draw button text");
  ((struct DATABUTTON *)(TEr->data))->buttontype=RADIOBUTTON;
  ((struct DATABUTTON *)(TEr->data))->bitflag=&TEeditmode;
  ((struct DATABUTTON *)(TEr->data))->bitvalue=EDDRAW;

  TEr=TEregs+FILLBUT;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[EDMDFRM].x1+2;
  TEr->x2=TEregs[EDMDFRM].x2-2;
  TEr->y1=TEregs[EDMDFRM].y1+2+11;
  TEr->y2=TEr->y1+10;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawButton;
  TEr->caption=NULL;
  SetCaption(TEr,"Fill");
  TEr->data=(struct DATABUTTON *) TEAlloc(sizeof (struct DATABUTTON), "fill button data");
  ((struct DATABUTTON *)(TEr->data))->infotype=DITBUTTON;
  ((struct DATABUTTON *)(TEr->data))->text=TENewText("Fill mode", "fill button text");
  ((struct DATABUTTON *)(TEr->data))->buttontype=RADIOBUTTON;
  ((struct DATABUTTON *)(TEr->data))->bitflag=&TEeditmode;
  ((struct DATABUTTON *)(TEr->data))->bitvalue=EDFILL;

  TEr=TEregs+PICKBUT;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[EDMDFRM].x1+2;
  TEr->x2=TEregs[EDMDFRM].x2-2;
  TEr->y1=TEregs[EDMDFRM].y1+2+22;
  TEr->y2=TEr->y1+10;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawButton;
  TEr->caption=NULL;
  SetCaption(TEr,"Pick");
  TEr->data=(struct DATABUTTON *) TEAlloc(sizeof (struct DATABUTTON), "pick button data");
  ((struct DATABUTTON *)(TEr->data))->infotype=DITBUTTON;
  ((struct DATABUTTON *)(TEr->data))->text=TENewText("Pick color mode", "pick button text");
  ((struct DATABUTTON *)(TEr->data))->buttontype=RADIOBUTTON;
  ((struct DATABUTTON *)(TEr->data))->bitflag=&TEeditmode;
  ((struct DATABUTTON *)(TEr->data))->bitvalue=EDPICK;

  TEr=TEregs+MASKBUT;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[EDMDFRM].x1+2;
  TEr->x2=TEregs[EDMDFRM].x2-2;
  TEr->y1=TEregs[EDMDFRM].y1+2+33;
  TEr->y2=TEr->y1+10;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawButton;
  TEr->caption=NULL;
  SetCaption(TEr,"Select");
  TEr->data=(struct DATABUTTON *) TEAlloc(sizeof (struct DATABUTTON), "select button data");
  ((struct DATABUTTON *)(TEr->data))->infotype=DITBUTTON;
  ((struct DATABUTTON *)(TEr->data))->text=TENewText("Select mode", "select button text");
  ((struct DATABUTTON *)(TEr->data))->buttontype=RADIOBUTTON;
  ((struct DATABUTTON *)(TEr->data))->bitflag=&TEeditmode;
  ((struct DATABUTTON *)(TEr->data))->bitvalue=EDMASK;

  TEr=TEregs+PSTEBUT;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[EDMDFRM].x1+2;
  TEr->x2=TEregs[EDMDFRM].x2-2;
  TEr->y1=TEregs[EDMDFRM].y1+2+44;
  TEr->y2=TEr->y1+10;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawButton;
  TEr->caption=NULL;
  SetCaption(TEr,"Paste");
  TEr->data=(struct DATABUTTON *) TEAlloc(sizeof (struct DATABUTTON), "paste button data");
  ((struct DATABUTTON *)(TEr->data))->infotype=DITBUTTON;
  ((struct DATABUTTON *)(TEr->data))->text=TENewText("Paste mode", "paste button text");
  ((struct DATABUTTON *)(TEr->data))->buttontype=RADIOBUTTON;
  ((struct DATABUTTON *)(TEr->data))->bitflag=&TEeditmode;
  ((struct DATABUTTON *)(TEr->data))->bitvalue=EDPASTE;

  TEr=TEregs+IND_FRM;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[SCTXTBX].x2+2;
  TEr->x2=TEr->x1+12*4-1; // 10*number of buttons
  TEr->y1=TEregs[SCTXTBX].y1;
  TEr->y2=TEr->y1+13;
  TEr->bitflags=REGACTIVE|REGVISIBLE|REGNOCLICK;
  TEr->drawproc=&DrawSunkRegion;
  TEr->caption=NULL;
  TEr->data=NULL;

  TEr=TEregs+MASKIND;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[IND_FRM].x1+2;
  TEr->x2=TEr->x1+10;
  TEr->y1=TEregs[IND_FRM].y1+2;
  TEr->y2=TEregs[IND_FRM].y2-2;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawButton;
  TEr->caption=NULL;
  SetCaption(TEr,"m");
  TEr->data=(struct DATABUTTON *) TEAlloc(sizeof (struct DATABUTTON), "mask indicator data");
  ((struct DATABUTTON *)(TEr->data))->infotype=DITBUTTON;
  ((struct DATABUTTON *)(TEr->data))->text=TENewText("mask", "mask indicator text");
  ((struct DATABUTTON *)(TEr->data))->buttontype=PUSHBUTTON;
  ((struct DATABUTTON *)(TEr->data))->bitflag=&TEbitflags;
  ((struct DATABUTTON *)(TEr->data))->bitvalue=TESHOWMSK;

  TEr=TEregs+GRIDIND;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[MASKIND].x2+1;
  TEr->x2=TEr->x1+10;
  TEr->y1=TEregs[IND_FRM].y1+2;
  TEr->y2=TEregs[IND_FRM].y2-2;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawButton;
  TEr->caption=NULL;
  SetCaption(TEr,"g");
  TEr->data=(struct DATABUTTON *) TEAlloc(sizeof (struct DATABUTTON), "grid button data");
  ((struct DATABUTTON *)(TEr->data))->infotype=DITBUTTON;
  ((struct DATABUTTON *)(TEr->data))->text=TENewText("grid", "grid indicator text");
  ((struct DATABUTTON *)(TEr->data))->buttontype=PUSHBUTTON;
  ((struct DATABUTTON *)(TEr->data))->bitflag=&TEbitflags;
  ((struct DATABUTTON *)(TEr->data))->bitvalue=TEGRID;

  TEr=TEregs+ANIMIND;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[GRIDIND].x2+1;
  TEr->x2=TEr->x1+10;
  TEr->y1=TEregs[IND_FRM].y1+2;
  TEr->y2=TEregs[IND_FRM].y2-2;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawButton;
  TEr->caption=NULL;
  SetCaption(TEr,"a");
  TEr->data=(struct DATABUTTON *) TEAlloc(sizeof (struct DATABUTTON), "animation button data");
  ((struct DATABUTTON *)(TEr->data))->infotype=DITBUTTON;
  ((struct DATABUTTON *)(TEr->data))->text=TENewText("animation", "animation indicator text");
  ((struct DATABUTTON *)(TEr->data))->buttontype=PUSHBUTTON;
  ((struct DATABUTTON *)(TEr->data))->bitflag=&TEbitflags;
  ((struct DATABUTTON *)(TEr->data))->bitvalue=TEANIMATE;

  TEr=TEregs+PXHLIND;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[ANIMIND].x2+1;
  TEr->x2=TEr->x1+10;
  TEr->y1=TEregs[IND_FRM].y1+2;
  TEr->y2=TEregs[IND_FRM].y2-2;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawButton;
  TEr->caption=NULL;
  SetCaption(TEr,"h");
  TEr->data=(struct DATABUTTON *) TEAlloc(sizeof (struct DATABUTTON), "highlight button data");
  ((struct DATABUTTON *)(TEr->data))->infotype=DITBUTTON;
  ((struct DATABUTTON *)(TEr->data))->text=TENewText("pixel highlight", "highlight indicator text");
  ((struct DATABUTTON *)(TEr->data))->buttontype=PUSHBUTTON;
  ((struct DATABUTTON *)(TEr->data))->bitflag=&TEbitflags;
  ((struct DATABUTTON *)(TEr->data))->bitvalue=TEHILIGHT;

  TEr=TEregs+BUT_FRM;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[EDMDFRM].x1;
  TEr->x2=TEregs[EDMDFRM].x2;
  TEr->y1=TEregs[EDMDFRM].y2+2;
  TEr->y2=TEr->y1+11*4+3;     // 10*number of buttons
  TEr->bitflags=REGACTIVE|REGVISIBLE|REGNOCLICK;
  TEr->drawproc=&DrawSunkRegion;
  TEr->caption=NULL;
  TEr->data=NULL;

  TEr=TEregs+UNDOBUT;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[BUT_FRM].x1+2;
  TEr->x2=TEregs[BUT_FRM].x2-2;
  TEr->y1=TEregs[BUT_FRM].y1+2;
  TEr->y2=TEr->y1+10;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawButton;
  TEr->caption=NULL;
  SetCaption(TEr, "Undo");
  TEr->data=(struct DATABUTTON *) TEAlloc(sizeof (struct DATABUTTON), "undo button data");
  ((struct DATABUTTON *)(TEr->data))->infotype=DITBUTTON;
  ((struct DATABUTTON *)(TEr->data))->text=TENewText("Undo last action", "undo button text");
  ((struct DATABUTTON *)(TEr->data))->buttontype=CLICKBUTTON;
  ((struct DATABUTTON *)(TEr->data))->bitflag=&TEbfa;
  ((struct DATABUTTON *)(TEr->data))->bitvalue=BFAUNDO;

  TEr=TEregs+COPYBUT;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[BUT_FRM].x1+2;
  TEr->x2=TEregs[BUT_FRM].x2-2;
  TEr->y1=TEregs[BUT_FRM].y1+2+11;
  TEr->y2=TEr->y1+10;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawButton;
  TEr->caption=NULL;
  SetCaption(TEr, "Copy");
  TEr->data=(struct DATABUTTON *) TEAlloc(sizeof (struct DATABUTTON), "copy button data");
  ((struct DATABUTTON *)(TEr->data))->infotype=DITBUTTON;
  ((struct DATABUTTON *)(TEr->data))->text=TENewText("Copy selected area", "copy button text");
  ((struct DATABUTTON *)(TEr->data))->buttontype=CLICKBUTTON;
  ((struct DATABUTTON *)(TEr->data))->bitflag=&TEbfa;
  ((struct DATABUTTON *)(TEr->data))->bitvalue=BFACOPY;

  TEr=TEregs+CUT_BUT;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[BUT_FRM].x1+2;
  TEr->x2=TEregs[BUT_FRM].x2-2;
  TEr->y1=TEregs[BUT_FRM].y1+2+22;
  TEr->y2=TEr->y1+10;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawButton;
  TEr->caption=NULL;
  SetCaption(TEr, "Cut");
  TEr->data=(struct DATABUTTON *) TEAlloc(sizeof (struct DATABUTTON), "cut button data");
  ((struct DATABUTTON *)(TEr->data))->infotype=DITBUTTON;
  ((struct DATABUTTON *)(TEr->data))->text=TENewText("Cut selected area", "cut button text");
  ((struct DATABUTTON *)(TEr->data))->buttontype=CLICKBUTTON;
  ((struct DATABUTTON *)(TEr->data))->bitflag=&TEbfa;
  ((struct DATABUTTON *)(TEr->data))->bitvalue=BFACUT;

  TEr=TEregs+CLR_BUT;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[BUT_FRM].x1+2;
  TEr->x2=TEregs[BUT_FRM].x2-2;
  TEr->y1=TEregs[BUT_FRM].y1+2+33;
  TEr->y2=TEr->y1+10;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawButton;
  TEr->caption=NULL;
  SetCaption(TEr, "Clear");
  TEr->data=(struct DATABUTTON *) TEAlloc(sizeof (struct DATABUTTON), "clear button data");
  ((struct DATABUTTON *)(TEr->data))->infotype=DITBUTTON;
  ((struct DATABUTTON *)(TEr->data))->text=TENewText("Clear selected area", "clear button text");
  ((struct DATABUTTON *)(TEr->data))->buttontype=CLICKBUTTON;
  ((struct DATABUTTON *)(TEr->data))->bitflag=&TEbfa;
  ((struct DATABUTTON *)(TEr->data))->bitvalue=BFACLEAR;

  TEr=TEregs+TOOLFRM;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[BUT_FRM].x2+2;
  TEr->x2=TEr->x1+34;
  TEr->y1=TEregs[BUT_FRM].y1;
  TEr->y2=TEr->y1+11*4+3;     // 10*number of buttons
  TEr->bitflags=REGACTIVE|REGVISIBLE|REGNOCLICK;
  TEr->drawproc=&DrawSunkRegion;
  TEr->caption=NULL;
  TEr->data=NULL;

  TEr=TEregs+RIACBUT;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[TOOLFRM].x1+2;
  TEr->x2=TEregs[TOOLFRM].x2-2;
  TEr->y1=TEregs[TOOLFRM].y1+2;
  TEr->y2=TEr->y1+10;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawButton;
  TEr->caption=NULL;
  SetCaption(TEr, "Turn L");
  TEr->data=(struct DATABUTTON *) TEAlloc(sizeof (struct DATABUTTON), "rotate button data");
  ((struct DATABUTTON *)(TEr->data))->infotype=DITBUTTON;
  ((struct DATABUTTON *)(TEr->data))->text=TENewText("Rotate anticlockwise", "rotate button text");
  ((struct DATABUTTON *)(TEr->data))->buttontype=CLICKBUTTON;
  ((struct DATABUTTON *)(TEr->data))->bitflag=&TEbfa;
  ((struct DATABUTTON *)(TEr->data))->bitvalue=BFARIAC;

  TEr=TEregs+RIC_BUT;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[TOOLFRM].x1+2;
  TEr->x2=TEregs[TOOLFRM].x2-2;
  TEr->y1=TEregs[TOOLFRM].y1+2+11;
  TEr->y2=TEr->y1+10;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawButton;
  TEr->caption=NULL;
  SetCaption(TEr, "Turn R");
  TEr->data=(struct DATABUTTON *) TEAlloc(sizeof (struct DATABUTTON), "rotate button data");
  ((struct DATABUTTON *)(TEr->data))->infotype=DITBUTTON;
  ((struct DATABUTTON *)(TEr->data))->text=TENewText("Rotate clockwise", "rotate button text");
  ((struct DATABUTTON *)(TEr->data))->buttontype=CLICKBUTTON;
  ((struct DATABUTTON *)(TEr->data))->bitflag=&TEbfa;
  ((struct DATABUTTON *)(TEr->data))->bitvalue=BFARIC;

  TEr=TEregs+FLIPBUT;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[TOOLFRM].x1+2;
  TEr->x2=TEregs[TOOLFRM].x2-2;
  TEr->y1=TEregs[TOOLFRM].y1+2+22;
  TEr->y2=TEr->y1+10;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawButton;
  TEr->caption=NULL;
  SetCaption(TEr, "Flip");
  TEr->data=(struct DATABUTTON *) TEAlloc(sizeof (struct DATABUTTON), "flip button data");
  ((struct DATABUTTON *)(TEr->data))->infotype=DITBUTTON;
  ((struct DATABUTTON *)(TEr->data))->text=TENewText("Flip", "flip button text");
  ((struct DATABUTTON *)(TEr->data))->buttontype=CLICKBUTTON;
  ((struct DATABUTTON *)(TEr->data))->bitflag=&TEbfa;
  ((struct DATABUTTON *)(TEr->data))->bitvalue=BFAFLIP;

  TEr=TEregs+MIRRBUT;
  TEr->infotype=DITREGION;
  TEr->x1=TEregs[TOOLFRM].x1+2;
  TEr->x2=TEregs[TOOLFRM].x2-2;
  TEr->y1=TEregs[TOOLFRM].y1+2+33;
  TEr->y2=TEr->y1+10;
  TEr->bitflags=REGACTIVE|REGVISIBLE;
  TEr->drawproc=&DrawButton;
  TEr->caption=NULL;
  SetCaption(TEr, "Mirror");
  TEr->data=(struct DATABUTTON *) TEAlloc(sizeof (struct DATABUTTON), "mirror button data");
  ((struct DATABUTTON *)(TEr->data))->infotype=DITBUTTON;
  ((struct DATABUTTON *)(TEr->data))->text=TENewText("Mirror", "mirror button text");
  ((struct DATABUTTON *)(TEr->data))->buttontype=CLICKBUTTON;
  ((struct DATABUTTON *)(TEr->data))->bitflag=&TEbfa;
  ((struct DATABUTTON *)(TEr->data))->bitvalue=BFAMIRROR;

  }

static void DestroyRegions (TEREGION **TEBaseReg)
  {
  TEREGION *TEr=NULL;
  int i;
  if (!(*TEBaseReg)->data) err("No region data defined.");
  if (((struct DATABASEREGION *)(*TEBaseReg)->data)->infotype!=DITBASEREGION)
    err("DestroyRegions called with invalid base region.");

  i=((struct DATABASEREGION *)(*TEBaseReg)->data)->numofregs;
  do
    {
    i--;
    TEr=(*TEBaseReg)+i;
    TEFree(TEr->caption);
    if (TEr->data)
      {
      switch (((struct DATABASEREGION *)(TEr->data))->infotype)
        {
        case DITBASEREGION:
          break;
        case DITBUTTON:
          TEFree(((struct DATABUTTON *)TEr->data)->text);
          break;
        }
      }
    TEFree(TEr->data);
    } while (i>0);
  TEFree(*TEBaseReg);
//  if (*TEBaseReg) err("Not freed");
  }

static void RenderTileEdit (TEREGION *TEBaseReg)
// -- ric: 20/Jul/98 - updatedt to be a little more generic ;)
  {
  int i;
  TExoffs=TEBaseReg->x1;
  TEyoffs=TEBaseReg->y1;
  if (!TEBaseReg->data) err("No region data defined.");
  if (((struct DATABASEREGION *)TEBaseReg->data)->infotype!=DITBASEREGION)
    err("RenderTileEdit called with invalid base region.");
  switch (((struct DATABASEREGION *)TEBaseReg->data)->windowtype)
    {
    case DBRSTDWINDOW:
      Window(TExoffs,TEyoffs,TEBaseReg->x2,TEBaseReg->y2, ((TEBaseReg->caption) ? TEBaseReg->caption : ""));
      break;
    case DBRPANEL:
      stdwindow(TExoffs,TEyoffs,TEBaseReg->x2,TEBaseReg->y2);
      break;
    }

  i=((struct DATABASEREGION *)TEBaseReg->data)->numofregs;
  do
    {
    i--;
    if ((TEBaseReg+i)->bitflags&REGVISIBLE) (TEBaseReg+i)->drawproc(TEBaseReg+i);
    } while (i>1);
  }

void TileEdit (void)
  {
  int i,j;
  int TEoldeditmode=EDDRAW;
  if (TEbitflags&TEQUITCUR) TEbitflags^=TEQUITCUR;

  InitTEImage();
  SetupRegions();
  TEeditmode=EDDRAW;  // Default to drawing mode
  if (TEbitflags&TESHOWMSK) TEbitflags^=TESHOWMSK;

  while (!(TEbitflags&TEQUITCUR))
    {
    // Mouse button click handler routines
    MBClickHandler(TEregs);

    TEbfa=BFANONE;          // Reset button action flag
    if (TEeditmode==EDMASK && TEoldeditmode!=EDMASK) TEbitflags|=TESHOWMSK;
    if (TEeditmode==EDMASK && TEoldeditmode==EDMASK && !(TEbitflags&TESHOWMSK))
      TEeditmode=EDDRAW;
    TEoldeditmode=TEeditmode;

    // Draw loop
    RenderTileEdit(TEregs);
    DrawMouse();
    ShowPage();

    if (key[SCAN_ESC])
      {
      key[SCAN_ESC]=0;
      TEbitflags|=TEQUITCUR;
      }
    if ((key[SCAN_CTRL] && key[SCAN_Z]) || (TEbfa&BFAUNDO) || (key[SCAN_U] && !(key[SCAN_CTRL])) && ((MBStatus[0]|MBStatus[1])==MOUSENONE))
      {
      key[SCAN_U]=0;
      key[SCAN_Z]=0;
      TEUndo();
      }
    if (key[SCAN_CTRL] && key[SCAN_R])
      {
      key[SCAN_R]=0;
      memcpy(TEundo, TEimage, TExsize*TEysize);   // copy image into undo buffer
      memcpy(TEimage, TEsource, TExsize*TEysize); // copy original image into edit buffer
      }
    if (key[SCAN_CTRL] && key[SCAN_A])
      {
      key[SCAN_A]=0;
      j=TEysize*TExsize;
      memset(TEmask, 0xf, j);
      }
    if (key[SCAN_CTRL] && key[SCAN_U])
      {
      key[SCAN_U]=0;
      j=TEysize*TExsize;
      memset(TEmask, 0x0, j);
      }
    if ((TEbfa&BFACOPY) || (key[SCAN_CTRL] && key[SCAN_C]) && ((MBStatus[0]|MBStatus[1])==MOUSENONE))
      {
      key[SCAN_C]=0;
      TECopyImage();
      }
    if ((TEbfa&BFACUT) || (key[SCAN_CTRL] && key[SCAN_X]) && ((MBStatus[0]|MBStatus[1])==MOUSENONE))
      {
      key[SCAN_X]=0;
      TECopyImage();
      TEClearImage();
      }
    if (key[SCAN_CTRL] && key[SCAN_V] && ((MBStatus[0]|MBStatus[1])==MOUSENONE))
      {
      key[SCAN_V]=0;
      TEeditmode=EDPASTE;
      }
    if ((TEbfa&BFACLEAR) || key[SCAN_DEL] && ((MBStatus[0]|MBStatus[1])==MOUSENONE))
      {
      key[SCAN_DEL]=0;
      TEClearImage();
      }
    if (key[SCAN_Q] && ((MBStatus[0]|MBStatus[1])==MOUSENONE))
      {
      key[SCAN_Q]=0;
      if (TEtype==TETILE && TEanimstrand)
        {
        if (TEtile>vspanim[TEanimstrand-1].start)
          {
          // Change tile index
          TEtile-=1;
          i=TExsize;
          j=TEysize;

          // Restart editor
          RestartTileEditor();
          }
        }
      }
    if (key[SCAN_W] && ((MBStatus[0]|MBStatus[1])==MOUSENONE))
      {
      key[SCAN_W]=0;
      if (TEtype==TETILE && TEanimstrand)
        {
        if (TEtile<vspanim[TEanimstrand-1].finish)
          {
          // Change tile index
          TEtile+=1;
          i=TExsize;
          j=TEysize;

          // Restart editor
          RestartTileEditor();
          }
        }
      }

    if ((TEbfa&BFARIAC) || key[SCAN_L] && ((MBStatus[0]|MBStatus[1])==MOUSENONE))
      {
      key[SCAN_L]=0;
      switch (TEeditmode)
        {
        case EDDRAW:
          memcpy(TEundo, TEimage, TExsize*TEysize);
          TERotateImageAntiClockwise(TEimage, TExsize, TEysize);
          break;
        case EDMASK:
          TERotateImageAntiClockwise(TEmask, TExsize, TEysize);
          break;
        case EDPASTE:
          if (TEcopy)
            {
            TERotateImageAntiClockwise(TEcopy, TEcopyxsize, TEcopyysize);
            TERotateImageAntiClockwise(TEcmsk, TEcopyxsize, TEcopyysize);
            }
          break;
        }
      }
    if ((TEbfa&BFARIC) || key[SCAN_R] && ((MBStatus[0]|MBStatus[1])==MOUSENONE))
      {
      key[SCAN_R]=0;
      switch (TEeditmode)
        {
        case EDDRAW:
          memcpy(TEundo, TEimage, TExsize*TEysize);
          TERotateImageClockwise(TEimage, TExsize, TEysize);
          break;
        case EDMASK:
          TERotateImageClockwise(TEmask, TExsize, TEysize);
          break;
        case EDPASTE:
          if (TEcopy)
            {
            TERotateImageClockwise(TEcopy, TEcopyxsize, TEcopyysize);
            TERotateImageClockwise(TEcmsk, TEcopyxsize, TEcopyysize);
            }
          break;
        }
      }
    if ((TEbfa&BFAMIRROR) || key[SCAN_X] && ((MBStatus[0]|MBStatus[1])==MOUSENONE))
      {
      key[SCAN_X]=0;
      switch (TEeditmode)
        {
        case EDDRAW:
          memcpy(TEundo, TEimage, TExsize*TEysize);
          TEMirrorImage(TEimage, TExsize, TEysize);
          break;
        case EDMASK:
          TEMirrorImage(TEmask, TExsize, TEysize);
          break;
        case EDPASTE:
          if (TEcopy)
            {
            TEMirrorImage(TEcopy, TEcopyxsize, TEcopyysize);
            TEMirrorImage(TEcmsk, TEcopyxsize, TEcopyysize);
            }
          break;
        }
      }
    if ((TEbfa&BFAFLIP) || key[SCAN_Y] && ((MBStatus[0]|MBStatus[1])==MOUSENONE))
      {
      key[SCAN_Y]=0;
      switch (TEeditmode)
        {
        case EDDRAW:
          memcpy(TEundo, TEimage, TExsize*TEysize);
          TEFlipImage(TEimage, TExsize, TEysize);
          break;
        case EDMASK:
          TEFlipImage(TEmask, TExsize, TEysize);
          break;
        case EDPASTE:
          if (TEcopy)
            {
            TEFlipImage(TEcopy, TEcopyxsize, TEcopyysize);
            TEFlipImage(TEcmsk, TEcopyxsize, TEcopyysize);
            }
          break;
        }
      }
    if (key[SCAN_F1])
      {
      key[SCAN_F1]=0;

      // Deacitvate tile editor
      for (i=0; i<MAX_REGIONS; i++)
        if (TEregs[i].bitflags&REGACTIVE) TEregs[i].bitflags^=REGACTIVE;

      AboutTE();

      // Reacitvate tile editor
      for (i=0; i<MAX_REGIONS; i++)
        TEregs[i].bitflags|=REGACTIVE;
      }
    if (key[SCAN_A])
      {
      key[SCAN_A]=0;
      lcolor+=1;
      lcolor&=255;
      }
    if (key[SCAN_D])
      {
      key[SCAN_D]=0;
      rcolor+=1;
      rcolor&=255;
      }
    if (key[SCAN_Z])
      {
      key[SCAN_Z]=0;
      if (!lcolor) lcolor=255;
      else lcolor-=1;
      }
    if (key[SCAN_C])
      {
      key[SCAN_C]=0;
      if (!rcolor) rcolor=255;
      else rcolor-=1;
      }
    if (key[SCAN_G])
      {
      key[SCAN_G]=0;
      TEbitflags^=TEGRID;
      }
    if (key[SCAN_H])
      {
      key[SCAN_H]=0;
      TEbitflags^=TEHILIGHT;
      }
    if (key[SCAN_I])
      {
      key[SCAN_I]=0;
      j=TEysize*TExsize;
      do
        {
        j--;
        TEmask[j]^=0xf;
        } while (j>0);
      }
    if (key[SCAN_M])
      {
      key[SCAN_M]=0;
      if (TEeditmode==EDMASK)
        TEeditmode=EDDRAW;
      TEbitflags^=TESHOWMSK;
      }
    if (key[SCAN_SPACE])
      {
      key[SCAN_SPACE]=0;
      if (TEeditmode==EDDRAW)
        {
        TEeditmode=EDMASK;
        TEbitflags|=TESHOWMSK;
        }
      else
        TEeditmode=EDDRAW;
      }
    if (key[SCAN_PGUP])
      {
      key[SCAN_PGUP]=0;
      switch (TEeditmode)
        {
        case EDDRAW:
        case EDMASK:
          memcpy(TEundo, TEimage, TExsize*TEysize);
          if (TEbitflags&TESHOWMSK)
            TEShiftColours(TEimage, TEmask, TExsize, TEysize, 1);
          else
            TEShiftColours(TEimage, NULL, TExsize, TEysize, 1);
          break;
        case EDPASTE:
          if (TEcopy)
            TEShiftColours(TEcopy, TEcmsk, TEcopyxsize, TEcopyysize, 1);
          break;
        }
      }
    if (key[SCAN_PGDN])
      {
      key[SCAN_PGDN]=0;
      switch (TEeditmode)
        {
        case EDDRAW:
        case EDMASK:
          memcpy(TEundo, TEimage, TExsize*TEysize);
          if (TEbitflags&TESHOWMSK)
            TEShiftColours(TEimage, TEmask, TExsize, TEysize, -1);
          else
            TEShiftColours(TEimage, NULL, TExsize, TEysize, -1);
          break;
        case EDPASTE:
          if (TEcopy)
            TEShiftColours(TEcopy, TEcmsk, TEcopyxsize, TEcopyysize, -1);
          break;
        }
      }

    if (key[SCAN_LANGLE])
      {
      key[SCAN_LANGLE]=0;
      ShutdownVideo();
      TEFreeReg(TEregs);
      vm=0;
      InitVideo(0);
      SetupRegions();
      set_intensity(63);
      InitMouse();
      }
    if (key[SCAN_RANGLE])
      {
      key[SCAN_RANGLE]=0;
      ShutdownVideo();
      TEFreeReg(TEregs);
      vm=1;
      InitVideo(1);
      SetupRegions();
      set_intensity(63);
      InitMouse();
      }
    }
  DeinitTEImage();
  TEFreeReg(TEregs);
  }
