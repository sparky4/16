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

#ifndef __EMS_H_
#define __EMS_H_

#include <dos.h>
#define   EMM  0x67
     
char far *emmbase;

emmtest();
emmok();
long emmavail();
long emmalloc(int n);
emmmap(long handle, int phys, int page);
void emmmove(int page, char *str, int n);
void emmget(int page, char *str, int n);
emmclose(long handle);

#endif __EMS_H_
