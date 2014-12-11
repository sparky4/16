#define FX_C

#include <fastgraf.h> /* FOR PALETTE AND KEYHIT FUNCTIONS */
#include "fx.h"
#include "roads.h"    /* FOR ANIMATE FUNCTION  */

/* FX.C-specific DEFINITIONS */
#define FADESTEPS 32     /* Number of gradations in the fade -- KEEP BELOW 256 */
#define SETDACS_DELAY 0  /* Number of clock ticks to wait between gradations */

/****************************************************************************\
*                                                                            *
*  fade_in                                                                   *
*                                                                            *
*  Fade one or more DACs from black to their target colors.                  *
*                                                                            *
\****************************************************************************/

void fade_in (int DACstart, int DACend)
{
   register int j, i;
   int k, n, temp;
   char new_palette [VCOLORS*3]; /* Temporarily stores palette */
   char key1, key2; /* USED FOR FOR KEYCHECK */

   if (DACend<DACstart) /* PREVENT start/end REVERSALS which could crash us */
   {
      temp=DACstart;
      DACstart=DACend;
      DACend=temp;
   }

   for (i = 0; i <= FADESTEPS; i++)
   {
      if (abortfadeonkeyhit)
      {
        fg_intkey (&key1, &key2);
        if (key1+key2>0) break;
      }

      for (k=0, n=DACstart*3, j = DACstart; j <= DACend; j++)
      {
         new_palette[k++] = (long) (default_palette[n++] * i) / FADESTEPS;
         new_palette[k++] = (long) (default_palette[n++] * i) / FADESTEPS;
         new_palette[k++] = (long) (default_palette[n++] * i) / FADESTEPS;
      }

      fg_setdacs (DACstart, DACend-DACstart+1, new_palette);
      fg_waitfor (SETDACS_DELAY);
      if (animatewhilefading) animate();
   }
}

/****************************************************************************\
*                                                                            *
*  fade_out                                                                  *
*                                                                            *
*  Fade one or more DACs from their current colors to black.                 *
*                                                                            *
\****************************************************************************/

void fade_out (int DACstart, int DACend)
{
   register int j, i;
   int k, n, temp;
   char new_palette [VCOLORS*3]; /* Temporarily stores palette */
   char key1, key2; /* USED FOR FOR KEYCHECK */

   if (DACend<DACstart) /* PREVENT start/end REVERSALS */
   {
      temp=DACstart;
      DACstart=DACend;
      DACend=temp;
   }

   for (i = FADESTEPS; i >= 0; i--)
   {
      if (abortfadeonkeyhit)
      {
        fg_intkey (&key1, &key2);
        if (key1+key2>0) break;
      }

      for (k=0, n=DACstart*3, j = DACstart; j <= DACend; j++)
      {
         new_palette[k++] = (long) (default_palette[n++] * i) / FADESTEPS;
         new_palette[k++] = (long) (default_palette[n++] * i) / FADESTEPS;
         new_palette[k++] = (long) (default_palette[n++] * i) / FADESTEPS;
      }
      fg_setdacs (DACstart, DACend-DACstart+1, new_palette);
      fg_waitfor (SETDACS_DELAY);
      if (animatewhilefading) animate();
   }
}

