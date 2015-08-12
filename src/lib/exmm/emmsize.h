// Cut this out as emmsize.h

/*      File:       EmmSize.h
 *      Module:     All Modules
 *      Author(s):  Chris Somers
 *      Date:       May 8, 1992
 *      Version:    V.1.0
 */

#ifndef _EMMSIZES_DEF
#define _EMMSIZES_DEF 1


/* Module size constants */

#define     MAXACTIONS          29
#define     MAXCHANNELS          4
#define     MAXFOCUS             3
#define     MAXBUFTYPES         64
#define     MAXEMHANDLES        64      /* up to a max of 255 */
#define     MAXTIMERS           32
#define     MAXTITEMS          128
#define     MAXQUEUES           24
#ifdef PRINTER
#define     MAXPRNS              4
#else
#define     MAXPRNS              0
#endif
#ifdef UTSCOM
#define     MAXUTSSCRNS          2
#else
#define     MAXUTSSCRNS          0
#endif
#ifdef CAPTURE
#define     MAXCAPTURE           1
#else
#define     MAXCAPTURE           0
#endif
#define     MAXIRQS             10
#define     MAXTCPDSCPTITEMS    16     /*max # of TCP connections*/

#define FREEBUFF        1   /* CallIODevice flag for BuffSize arrays */
#define DSCONLY         2   /* allocate a BufDscpt, only - no buffer */

#define     EMMPAGESIZE     0x4000     /* 16K EMM page size */
#define     SETUPLEN            32
#define     NAMELEN             40
#define     UNIXLEN             32
#define     ADDRLEN             16
#define     TITLELEN           128
#define     TSLEN               80
#define     COMMENTLEN          65
#define     PALSIZE            768
#define     QSTRLEN            254
#define     PRNBUFSIZ         2048
#define     TXTSCRNSIZE       4096
#define     UTSBUFSIZ         4096
#define     UTSSCRNSIZ        1920
#define     QMEMSIZE            15
#define     KBUFLEN             16
#define     GXBUFFSIZE      0x4000
#define     TCPNDBUFFSIZE   0x1000      /*router auto-allocation buff size*/
                                        /* graphics printing scale values */
#define     SCALE_HALF           1      /* value must not change */
#define     SCALE_ONE            2
#define     SCALE_QUART          3      /* value must not change */

#define     SIXTH_SECOND         3L     /* shade over 1/6 of a second*/
#define     HALF_SECOND          9L
#define     ONE_SECOND          18L
#define     TWO_SECONDS         36L
#define     FIVE_SECONDS        91L
#define     TEN_SECONDS        182L
#define     HALF_MINUTE      182*3L     /* same as 18.2*30, right? */
#define     ONE_MINUTE       182*6L     /* same as 18.2*60, right? */
#define     TWO_MINUTES     182*12L     /* same as 18.2*120, right? */
#define     FIVE_MINUTES    182*30L     /* same as 18.2*300, right? */
#define     TEN_MINUTES     182*60L     /* same as 18.2*600, right? */
#define     HALF_HOUR      182*180L     /* same as 18.2*1800, right? */
#define     ONE_HOUR       182*360L     /* same as 18.2*3600, right? */

#define MAXROUTMOVES            24      /*max # of routing moves at one time*/
                                        /*also max # of Move Complete Events*/
/* Event Channel Defines */

#define MAXRECALLEVENTS    1   /*max nm of Recall Event Channels*/
#define MAXKBDEVENTS       1   /*max nm of Kbd Event Channels*/
#define MAXPRNEVENTS       4   /*max nm of Prt Prog & Prt Cmplte Event Chan*/
#define MAXUTSRCVEVENTS    MAXUTSSCRNS       /*max nm of Uts Rx Event Chans*/
#define MAXUTSXMTEVENTS    MAXUTSSCRNS       /*max nm of Uts Tx Event Chans*/
#define MAXCAPEVENTS       2   /* max number of capture event channels */
#define MAXOP1CMPLTEVENTS  1   /*max nm of Operation 1 Cmplt Event Channels*/
#define MAXOP2CMPLTEVENTS  1   /*max nm of Operation 2 Cmplt Event Channels*/
#define MAXOP3CMPLTEVENTS  MAXTCPDSCPTITEMS  /*max nm of Op 3 Event Chans*/
#define MAXTCPEVENTS       MAXTCPDSCPTITEMS  /* max nm of TCP Event types */

#endif
