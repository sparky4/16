/* 
 *************************************************************************
 *
 *  PCX_EXT.H - PCX_LIB Library External Definitions Include File
 *
 *  Version:    1.00B
 *
 *  History:    91/02/14 - Created
 *              91/04/01 - Release 1.00A
 *              91/04/07 - Release 1.00B
 *
 *  Compiler:   Microsoft C V6.0
 *
 *  Author:     Ian Ashdown, P.Eng.
 *              byHeart Software
 *              620 Ballantree Road
 *              West Vancouver, B.C.
 *              Canada V7S 1W3
 *              Tel. (604) 922-6148
 *              Fax. (604) 987-7621
 *
 *  Copyright:  Public Domain
 *
 *************************************************************************
 */

/*      DEFINITIONS                                                     */

#define _PCX_EXT_H      1

#define FALSE           0
#define TRUE            1

typedef int BOOL;       /* Boolean flag                                 */

/*      STRUCTURE DECLARATIONS                                          */

typedef struct pcx_vsb  /* BIOS video services data save buffer         */
{
  struct pcx_ppt                /* Primary Pointer Table                */
  {
    void _far *vptp;            /* Video Parameter Table pointer        */
    unsigned char _far *dsap;   /* Dynamic Save Area pointer            */
    void _far *tmacgp;          /* Text Mode Aux Char Generator pointer */
    void _far *gmacgp;          /* Graphics Mode Aux Char Generator ptr */
    void _far *sptp;            /* Secondary Pointer Table pointer      */
    void _far *rsv_1;           /* Reserved                             */
    void _far *rsv_2;           /* Reserved                             */
  }
  pcx_ppt;

  /* Previous Primary Pointer Table pointer                             */

  struct pcx_ppt _far *prev_pptp;
}
PCX_VSB;

/* FUNCTIONS PROTOTYPES                                                 */

extern BOOL pcx_init_dsa(PCX_VSB **);
extern BOOL pcx_isvga(void);
extern BOOL pcx_read(char *, int, int);
extern BOOL pcx_write(char *, int, int, int, int);

extern void pcx_free_dsa(PCX_VSB *);

