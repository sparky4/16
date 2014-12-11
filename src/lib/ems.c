//------- PRACE S EMS pameti ---------------
#include <stdio.h>
#include <fcntl.h>
#include <io.h>
#include <dos.h>
#include <mem.h>

typedef struct
  {
  unsigned long length;         /* velikost prenasene pameti      */
  unsigned int sourceH;         /* handle pro zdroj (0=konvencni  */
  unsigned long sourceOff;      /* offset zdroje pameti           */
  unsigned int destH;           /* handle pro terc (0=konvencni   */
  unsigned long destOff;        /* offset terce pameti            */
  } XMOVE;

int get_emem(void);             // Fce pro zachazeni s EMS
int alloc_emem(int n);          // Alokuje n KB pameti, vraci handle
int dealloc_emem(int h);        // Dealokuje EMS (handle)
int move_emem(XMOVE *p);        // presune blok z/do EMS
int mem_emem(unsigned *total, unsigned *free);

#define  pagesizeEMS 0x4000       // pamet EMS je ze 16k stranek

//int   pagesAllocated = 0;
//int   totalPages;
char *EmsFrame;


//------ Zda je EMS driver dostupny: ret=  1 - ANO, 0 - NE
int isEMS(void)
{
    int fh;
    union REGS rg;

    if((fh=open("EMMXXXX0",O_RDONLY,&fh)) == -1) return( 0 );

    rg.h.ah = 0x44;
    rg.h.al = 0x00;
    rg.x.bx = fh;
    int86(0x21,&rg,&rg);
    close(fh);
    if(rg.x.cflag) return(0);
    if(rg.x.dx & 0x80)
     return( 1 );
    else
     return( 0 );
}

//----- Zda je EMS HW dostupny ret=  1 - ANO, 0 - NE
int checkEMS(void)
{
    union REGS rg;

    rg.h.ah = 0x40;
    int86(0x67,&rg,&rg);
    if(rg.h.ah == 0)
     return( 1 );
    else
     return( 0 );
}

//----- Vraci totalni pocet stranek EMS nebo -1 ----
int coretotalEMS(void)
{
    union REGS rg;

    rg.h.ah = 0x42;
    int86(0x67,&rg,&rg);
    if(rg.x.cflag) return( -1 );
    //if(!pagesAllocated)
    // { pagesAllocated = 1;
    //   totalPages = rg.x.dx;
    // }
    return(rg.x.bx);
}

//----- Vraci pocet volnych stranek EMS nebo -1 ----
int coreleftEMS(void)
{
    union REGS rg;

    //if(pagesAllocated) return(totalPages);
    rg.h.ah = 0x42;
    int86(0x67,&rg,&rg);
    if(rg.x.cflag) return( -1 );
    //if(!pagesAllocated)
    //pagesAllocated = 1;
    //totalPages = rg.x.dx;
    //return(totalPages);
    return(rg.x.dx);
}

//----- Vraci EMS page frame (pointr na EMS) nebo NULL ----
char *pageframeEMS(void)
{
    union REGS rg;

    rg.h.ah = 0x41;
    int86(0x67,&rg,&rg);
    if(rg.h.ah != 0)
     return( NULL );
    else
     return((char *)MK_FP(rg.x.bx,0));
}

//----- Alokuje n stranek - vraci handle na blok stranek nebo 0 ----
unsigned mallocEMS(int n)
{
    union REGS rg;

    if(n > coreleftEMS() ) return( 0 );
    rg.h.ah = 0x43;
    rg.x.bx = n;
    int86(0x67,&rg,&rg);
    if(rg.h.ah)
     return( 0 );
    else
     return(rg.x.dx);
}

//----- Dealokuje blok stranek ret = 1-O.K. 0-ERR -----
unsigned freeEMS(unsigned h)
{
    union REGS rg;
    int   i;

    for(i=0; i<5; i++)
     { rg.h.ah = 0x45;
       rg.x.dx = h;
       int86(0x67,&rg,&rg);
       if(rg.h.ah == 0) break;
     }
    if(rg.h.ah == 0)
     return( 1 );
    else
     return( 0 );
}

//----- Mapuje logiclou stranku do fyzicke stranky
int mapEMS(unsigned h, int Ppage, int Lpage)
{
    union REGS rg;

    if(Ppage < 0 || Ppage > 3) return( 0 );
    rg.h.ah = 0x44;
    rg.h.al = Ppage;
    rg.x.bx = Lpage;
    rg.x.dx = h;
    int86(0x67,&rg,&rg);
    if(rg.h.ah != 0)
     return( 0 );
    else
     return( 1 );
}

// ##### Fce se stejnymi parametry pro EMS jako pro XMS

//----- Zda je EMS dostupna
int get_emem(void)
{
  int ist;

  ist = checkEMS();
  if(ist == 1)
  { ist = isEMS();
    if(ist == 1) return( 0x0300 );
  }
  return( -1 );
}

//----- Allokuje Kb pameti -------
int alloc_emem(int kb)
{
   int Pages,hhh;

   Pages = kb / 16;
   if((Pages * 16) < kb) Pages++;

   hhh = mallocEMS(Pages);
   if(hhh == 0)
    return( -1);
   else
    return(hhh);
}

//----- dealokuje EMS pres handle
int dealloc_emem(int h)
{
   return( freeEMS( h ) );
}

//----- presune blok pameti
//  unsigned long length;         /* velikost prenasene pameti      */
//  unsigned int  sourceH;        /* handle pro zdroj (0=konvencni  */
//  unsigned long sourceOff;      /* offset zdroje pameti           */
//  unsigned int  destH;          /* handle pro terc (0=konvencni   */
//  unsigned long destOff;        /* offset terce pameti            */
int move_emem(XMOVE *pxm)
{
   unsigned char *SrcBuf,*DstBuf;
   int      ist;
   unsigned int  SrcOff, DstOff, BegPage, BegOff, FreeByte, CopyLen;

   if(pxm->sourceH == 0 && pxm->destH != 0)    // Buffer->EMS
   {
     SrcBuf  = (unsigned char *)pxm->sourceOff;// buffer
     SrcOff  = 0;
     BegPage = pxm->destOff / pagesizeEMS;     // pocatecni page
     BegOff  = pxm->destOff % pagesizeEMS;     // offset in page
     FreeByte= pagesizeEMS - BegOff;           // volnych B na page
     CopyLen = pxm->length;                    // celkova delka

     Next_page:
     if(CopyLen > FreeByte)
     {
       ist = mapEMS(pxm->destH, 0, BegPage);
       if(ist==0) return( 0 );
       memcpy(EmsFrame+BegOff, SrcBuf+SrcOff, FreeByte);

       CopyLen  = CopyLen - FreeByte;
       SrcOff  += FreeByte;
       BegPage++;
       BegOff   = 0;
       FreeByte = pagesizeEMS;
       goto Next_page;
     }
     else
     {
       ist = mapEMS(pxm->destH, 0, BegPage);
       if(ist==0) return( 0 );
       memcpy(EmsFrame+BegOff, SrcBuf+SrcOff, CopyLen);
     }
   }
   else if(pxm->sourceH != 0 && pxm->destH == 0) // EMS->Buffer
   {
     DstBuf  = (unsigned char *)pxm->destOff;// buffer
     DstOff  = 0;
     BegPage = pxm->sourceOff / pagesizeEMS;     // pocatecni page
     BegOff  = pxm->sourceOff % pagesizeEMS;     // offset in page
     FreeByte= pagesizeEMS - BegOff;             // volnych B na page
     CopyLen = pxm->length;                    // celkova delka

     Next_page2:
     if(CopyLen > FreeByte)
     {
       ist = mapEMS(pxm->sourceH, 0, BegPage);
       if(ist==0) return( 0 );
       memcpy(DstBuf+DstOff, EmsFrame+BegOff, FreeByte);

       CopyLen  = CopyLen - FreeByte;
       DstOff  += FreeByte;
       BegPage++;
       BegOff   = 0;
       FreeByte = pagesizeEMS;
       goto Next_page2;
     }
     else
     {
       ist = mapEMS(pxm->sourceH, 0, BegPage);
       if(ist==0) return( 0 );
       memcpy(DstBuf+DstOff, EmsFrame+BegOff, CopyLen);
     }
   }
   else   // Error
   { return( 0 );
   }

   return 1;
}

// ----- Vrati pocet volnych a max. KB EMS
int mem_emem(unsigned int *total, unsigned int *freeall)
{
 int pom;

 pom = coretotalEMS();
 if(pom != -1 )
  *total = pom * 16;
 else
  return( 0 );
 pom = coreleftEMS();
 if(pom != -1)
  *freeall = pom * 16;
 else
  return( 0 );

 return( 1 );
}

void emmmove(int page, short *str, int n)
     {
          /*
          Move 'n' bytes from conventional memory to the specified
     expanded memory
          page
          */
     
          char far *ptr;
     
          ptr = pageframeEMS() + page * 16384;
          while(n-- > 0)
               *ptr++ = *str++;
     }
     
     void emmget(int page, short *str, int n)
     {
          /*
          Move 'n' bytes from the specified expanded memory page into
     conventional
          memory
          */
     
          char far *ptr;
     
          ptr = pageframeEMS() + page * 16384;
          while(n-- > 0)
               *str++ = *ptr++;
     }
