/*

Name:
MIKMOD.C

Description:
Modplaying example of mikmod.

MSDOS:  BC(y)   Watcom(y)       DJGPP(y)
Win95:  BC(y*)
Os2:    y
Linux:  n

* console mode only
(y) - yes
(n) - no (not possible or not useful)
(?) - may be possible, but not tested

*/
#ifdef __WIN32__
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

#ifndef __OS2__
#include <dos.h>
#endif

#include "wildfile.h"
#include "mikmod.h"

char helptext[]=

"Available switches (CaSe SeNsItIvE!):\n"
"\n"
"  /d x    use device-driver #x for output (0 is autodetect). Default=0\n"
"  /ld     List all available device-drivers\n"
"  /ll     List all available loaders\n"
"  /x      disables protracker extended speed\n"
"  /p      disables panning effects (9fingers.mod)\n"
"  /v xx   Sets volume from 0 (silence) to 100. Default=100\n"
"  /f xxxx Sets mixing frequency. Default=44100\n"
"  /m      Force mono output (so sb-pro can mix at 44100)\n"
"  /8      Force 8 bit output\n"
"  /i      Use interpolated mixing\n"
"  /r      Restart a module when it's done playing";


/*
        declarations for boring old sys-v style getopt *yawn*:
*/
int     getopt(int argc, char *argv[], char *optionS);
extern char *optarg;
extern int optind;
extern int opterr;


void tickhandler(void)
{
        MP_HandleTick();    /* play 1 tick of the module */
        MD_SetBPM(mp_bpm);
}


int main(int argc,char *argv[])
{
        UNIMOD *mf;
        int cmderr=0;                   /* error in commandline flag */
        int morehelp=0;                 /* set if user wants more help */
        int quit;
        int t;
        static int nargc;
        static char **nargv;

        puts(mikbanner);

        /* Expand wildcards on commandline */

        nargc=argc; nargv=argv;
#ifndef __DJGPP__
        MyGlob(&nargc,&nargv,0);
#else
        setvbuf(stdout, NULL, _IONBF, 0);
        __djgpp_set_ctrl_c(0);
#endif

        /*
                Initialize soundcard parameters.. you _have_ to do this
                before calling MD_Init(), and it's illegal to change them
                after you've called MD_Init()
        */

        md_mixfreq      =44100;                     /* standard mixing freq */
        md_dmabufsize   =20000;                     /* standard dma buf size */
        md_mode         =DMODE_16BITS|DMODE_STEREO; /* standard mixing mode */
        md_device       =0;                                                     /* standard device: autodetect */

        /*
                Register the loaders we want to use..
        */

        ML_RegisterLoader(&load_m15);    /* if you use m15load, register it as first! */
        ML_RegisterLoader(&load_mod);
        ML_RegisterLoader(&load_mtm);
        ML_RegisterLoader(&load_s3m);
        ML_RegisterLoader(&load_stm);
        ML_RegisterLoader(&load_ult);
        ML_RegisterLoader(&load_uni);
        ML_RegisterLoader(&load_xm);

        /*
                Register the drivers we want to use:
        */

        MD_RegisterDriver(&drv_nos);
#ifdef __OS2__
        MD_RegisterDriver(&drv_os2);
#elif defined(__WIN32__)
        MD_RegisterDriver(&drv_w95);
#else
        MD_RegisterDriver(&drv_ss);
        MD_RegisterDriver(&drv_sb);
        MD_RegisterDriver(&drv_gus);
#endif

        MD_RegisterPlayer(tickhandler);

        /* Parse option switches using standard getopt function: */

        opterr=0;

        while( !cmderr &&
                  (t=getopt(nargc,nargv,"ohxpm8irv:f:l:d:")) != EOF ){

                switch(t){

                        case 'd':
                                md_device=atoi(optarg);
                                break;

                        case 'l':
                                if(optarg[0]=='d') MD_InfoDriver();
                                else if(optarg[0]=='l') ML_InfoLoader();
                                else{
                                        cmderr=1;
                                        break;
                                }
                                exit(0);

                        case 'r':
                                mp_loop=1;
                                break;

                        case 'm':
                                md_mode&=~DMODE_STEREO;
                                break;

                        case '8':
                                md_mode&=~DMODE_16BITS;
                                break;

                        case 'i':
                                md_mode|=DMODE_INTERP;
                                break;

                        case 'x':
                                mp_extspd=0;
                                break;

                        case 'p':
                                mp_panning=0;
                                break;

                        case 'v':
                                if((mp_volume=atoi(optarg))>100) mp_volume=100;
                                break;

                        case 'f':
                                md_mixfreq=atol(optarg);
                                break;

                        case 'h':
                                morehelp=1;
                                cmderr=1;
                                break;

                        case '?':
                                puts("\07Invalid switch or option needs an argument\n");
                                cmderr=1;
                                break;
                }
        }

        if(cmderr || optind>=nargc){

                /*
                        there was an error in the commandline, or there were no true
                        arguments, so display a usage message
                */

                puts("Usage: MIKMOD [switches] <fletch.mod> ... \n");

                if(morehelp)
                        puts(helptext);
                else
                        puts("Type MIKMOD /h for more help.");

                exit(-1);
        }

        /*  initialize soundcard */

        if(!MD_Init()){
                printf("Driver error: %s.\n",myerr);
                return 0;
        }

        printf("Using %s for %d bit %s %s sound at %u Hz\n\n",
                        md_driver->Name,
                        (md_mode&DMODE_16BITS) ? 16:8,
                        (md_mode&DMODE_INTERP) ? "interpolated":"normal",
                        (md_mode&DMODE_STEREO) ? "stereo":"mono",
                        md_mixfreq);

#ifdef __OS2__
        DosSetPriority( PRTYS_THREAD, PRTYC_TIMECRITICAL, 5L, 0UL );
#endif

        for(quit=0; !quit && optind<nargc; optind++){

                printf("File    : %s\n",nargv[optind]);

                /* load the module */

                mf=ML_LoadFN(nargv[optind]);

                /* didn't work -> exit with errormsg. */

                if(mf==NULL){
                        printf("MikMod Error: %s\n",myerr);
                        break;
                }

                /*      initialize modplayer to play this module */

                MP_Init(mf);

                printf( "Songname: %s\n"
                                "Modtype : %s\n"
                                "Periods : %s,%s\n",
                                mf->songname,
                                mf->modtype,
                                (mf->flags&UF_XMPERIODS) ? "XM type" : "mod type",
                                (mf->flags&UF_LINEAR) ? "Linear" : "Log");

                /*
                        set the number of voices to use.. you
                        could add extra channels here (e.g. md_numchn=mf->numchn+4; )
                        to use for your own soundeffects:
                */

                md_numchn=mf->numchn;

                /*  start playing the module: */

                MD_PlayStart();

                while(!MP_Ready()){

                        char c;

                        c=kbhit() ? getch() : 0;

                        if(c=='+')
                                MP_NextPosition();
                        else if(c=='-')
                                MP_PrevPosition();
                        else if(c==0x1b){
                                quit=1;
                                break;
                        }
                        else if(c==' ') break;

                        MD_Update();

                        /* wait a bit */

#if defined(__OS2__)
                        DosSleep(40);   /* hmmmm */
#elif defined(__WIN32__)
                        Sleep(10);
#elif defined(__DJGPP__)
                        /* no wait for djgpp ? */
#else
                        delay(10);
#endif
                        printf("\rsngpos:%d patpos:%d sngspd %d bpm %d   ",mp_sngpos,mp_patpos,mp_sngspd,mp_bpm);
                }

                MD_PlayStop();          /* stop playing */
                ML_Free(mf);            /* and free the module */
                puts("\n");
        }
        MD_Exit();
        return 0;
}
