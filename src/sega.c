#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

struct list {
#ifdef __WATCOMC__
	struct list __based(__self) *next;
#endif
#ifdef __BORLANDC__
	struct list _seg *next;
#endif
	int         value;
};

void main(int argc, char *argv[])
{
	int         i;
#ifdef __WATCOMC__
	__segment   segu;
	void __based(__self) *pee;
	struct list __based(segu) *head;
	struct list __based(segu) *p;
#endif
#ifdef __BORLANDC__
	void _seg *pee;
	struct list _seg *head;
	struct list _seg *p;
#endif

    /* allocate based heap */
    segu = _bheapseg( 65536 );
    if( segu == _NULLSEG ) {
      printf( "Unable to allocate based heap\n" );
      exit( 1 );
    }

    /* create a linked list in the based heap */
    head = 0;
    for( i = 1; i < 4096; i++ ) {
      p = _bmalloc( segu, sizeof( struct list ) );
      if( p == _NULLOFF ) {
        printf( "_bmalloc failed\n" );
        break;
      }
      p->next = head;
      p->value = i;
      head = p;
    }

    /* traverse the linked list, printing out values */
    for( p = head; p != 0; p = p->next ) {
		if(p==head || p->next==0 || p==pee){
			printf("Segu = %04X", p); printf( "	Value = %d\n", p->value );
		}
    }

	printf("program=%FP\n", *argv[0]);
	printf("seg=%04X\n", segu);

    /* free all the elements of the linked list */
    for( ; p = head; ) {
      head = p->next;
      _bfree( segu, p );
    }
    /* free the based heap */
    _bfreeseg( segu );
}
