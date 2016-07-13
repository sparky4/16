// AJR notes: some of these are not used by the emm code

// Cut this out as emmret.h

/*      File:       EmmRet.h
 *      Module:     All Modules
 *      Author(s):  Chris Somers
 *      Date:       May 8, 1992
 *      Version:    V.1.0
 */

#ifndef _EMM_RETURNS_DEF
#define _EMM_RETURNS_DEF 1

/* Module return values */

/* Return Values */

#define     START                8
#define     END                  7
#define     MOVABORT             6
#define     PRN_COMPLETE         5
#define     PRN_PROGRESS         4
#define     INCOMPLETE           3
#define     HAVEROOM             2
#define     SUCCESS              1
#define     TRUE                 1
#define     YES                  1
#define     FALSE                0
#define     NO                   0
#define     NOTREADY             0
#define     NO_DATA              0
#define     NONE                -1

/* Start of Recoverable error codes */

#define     NO_SPACE            -1001
#define     NOTPOLLING          -1002
#define     ALREADYDONE         -1003
#define     NO_PRN_DEV          -1004
#define     OUTF_PAPER          -1005
#define     NO_VIDEO            -1006
#define     TIMEOUT             -1007
#define     FILENOPEN           -1008
#define     ABORT_REQ           -1009
#define     DEV_IOERR           -1010

#define     MAXRERR                10
#define     RECOVERABLE         -1999   /* all above errors are recoverable */

/* Start of Threadfatal error codes */

#define     NOT_OPEN            -2001
#define     NOT_ATTACHED        -2002
#define     NO_CONNECTION       -2003
#define     INSUFF_MEM          -2004
#define     NR_TIMEOUT          -2005

#define     MAXTERR                 5
#define     THREADFATAL         -2999   /* above errors force task to cancel */

/* Start of Systemfatal error codes */

#define     BAD_TASKNUM         -3001
#define     BAD_HANDLE          -3002
#define     BAD_HARDWARE        -3003
#define     INVALIDACTION       -3004
#define     NOFREEITEMS         -3005
#define     NO_MEMORY           -3006
#define     NO_EMS              -3007
#define     VALUE_OUTF_RANGE    -3008
#define     BAD_MODE            -3009
#define     NO_PALETTE          -3010
#define     BAD_DISPPAGE        -3011
#define     NO_TSR              -3012
#define     BUFTOOSMALL         -3013
#define     BAD_NAME            -3014
#define     BAD_DISPHW          -3015
#define     NO_FLOPPY           -3016

#define     MAXSERR                16
#define     SYSTEMFATAL         -3999   /* above errors are fatal to system */

#endif
