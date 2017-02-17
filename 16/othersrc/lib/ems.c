/*

                   THE IBM PC PROGRAMMER'S GUIDE TO C
                                    
                                    
                                    
                               3rd Edition
                                    
                                    
                                    
                             Matthew Probert


                                    
                            COPYRIGHT NOTICE


This publication remains the property of Matthew Probert. License is
hereby given for this work to be freely distibuted in whole under the
proviso that credit is given to the author. Sections of this work may be
used and distributed without payment under the proviso that credit is
given to both this work and the author. Source code occuring in this work
may be used within commercial and non-commercial applications without
charge and without reference to the author.
*/
     /*
     Various functions for using Expanded memory
     */
#include "src\lib\ems.h"
     emmtest()
     {
          /*
          Tests for the presence of expnaded memory by attempting to
          open the file EMMXXXX0.
          */
     
          union REGS regs;
          struct SREGS sregs;
          int error;
          long handle;
     
          /* Attempt to open the file device EMMXXXX0 */
          regs.x.ax = 0x3d00;
          regs.x.dx = (int)"EMMXXXX0";
          sregs.ds = 0; //????
          intdosx(&regs,&regs,&sregs);
          handle = regs.x.ax;
          error = regs.x.cflag;
     
          if (!error)
          {
               regs.h.ah = 0x3e;
               regs.x.bx = handle;
               intdos(&regs,&regs);
          }
          return error;
     }
     
     emmok()
     {
          /*
          Checks whether the expanded memory manager responds correctly
          */
     
          union REGS regs;
     
          regs.h.ah = 0x40;
          int86(EMM,&regs,&regs);
     
          if (regs.h.ah)
               return 0;
     
          regs.h.ah = 0x41;
          int86(EMM,&regs,&regs);
     
          if (regs.h.ah)
               return 0;
     
          emmbase = MK_FP(regs.x.bx,0);
          return 1;
     }
     
     long emmavail()
     {
        /*
        Returns the number of available (free) 16K pages of expanded
     memory
        or -1 if an error occurs.
        */
     
             union REGS regs;
     
          regs.h.ah = 0x42;
          int86(EMM,&regs,&regs);
          if (!regs.h.ah)
               return regs.x.bx;
          return -1;
     }
     
     long emmalloc(int n)
     {
          /*
          Requests 'n' pages of expanded memory and returns the file
     handle
          assigned to the pages or -1 if there is an error
          */
     
          union REGS regs;
     
          regs.h.ah = 0x43;
          regs.x.bx = n;
          int86(EMM,&regs,&regs);
          if (regs.h.ah)
               return -1;
          return regs.x.dx;
     }
     
     emmmap(long handle, int phys, int page)
     {
          /*
          Maps a physical page from expanded memory into the page frame
     in the
          conventional memory 16K window so that data can be transfered
     between
          the expanded memory and conventional memory.
          */
     
          union REGS regs;
     
          regs.h.ah = 0x44;
          regs.h.al = page;
          regs.x.bx = phys;
          regs.x.dx = handle;
          int86(EMM,&regs,&regs);
          return (regs.h.ah == 0);
     }
     
     void emmmove(int page, char *str, int n)
     {
          /*
          Move 'n' bytes from conventional memory to the specified
     expanded memory
          page
          */
     
          char far *ptr;
     
          ptr = emmbase + page * 16384;
          while(n-- > 0)
               *ptr++ = *str++;
     }
     
     void emmget(int page, char *str, int n)
     {
          /*
          Move 'n' bytes from the specified expanded memory page into
     conventional
          memory
          */
     
          char far *ptr;
     
          ptr = emmbase + page * 16384;
          while(n-- > 0)
               *str++ = *ptr++;
     }
     
     emmclose(long handle)
     {
          /*
          Release control of the expanded memory pages allocated to
     'handle'
          */
     
          union REGS regs;
     
          regs.h.ah = 0x45;
          regs.x.dx = handle;
          int86(EMM,&regs,&regs);
          return (regs.h.ah == 0);
     }
