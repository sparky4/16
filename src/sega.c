#include <stdio.h>
#include <dos.h>
#include <malloc.h>

typedef struct {
	struct a __based( __self ) *next;
	int                         number;
}a_t;

void PrintLastTwo( a_t far *list );

void
main(int argc, char *argv[])
{
	a_t far *list;
	PrintLastTwo(list);
}

     void PrintLastTwo( a_t far *list )
     {
       __segment                seg;
       a_t __based( seg ) *aptr;

       seg  = FP_SEG( list );
       aptr = FP_OFF( list );
       for( ; aptr != _NULLOFF; aptr = aptr->next ) {
         if( aptr->next == _NULLOFF ) {
           printf( "Last item is %d\n",
                   aptr->number );
         } else if( aptr->next->next == _NULLOFF ) {
           printf( "Second last item is %d\n",
                   aptr->number );
         }
       }
     }
     