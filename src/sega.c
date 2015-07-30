#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

struct list {
    struct list __based(__self) *next;
    int         value;
};

void main(int argc, char *argv[])
  {
    int         i;
    __segment   segu;
    struct list __based(segu) *head;
    struct list __based(segu) *p;

    /* allocate based heap */
    segu = _bheapseg( 1024 );
    if( segu == _NULLSEG ) {
      printf( "Unable to allocate based heap\n" );
      exit( 1 );
    }

    /* create a linked list in the based heap */
    head = 0;
    for( i = 1; i < 10; i++ ) {
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
	printf("Segu = %04X %FP", p, p);
      printf( "	Value = %d\n", p->value );
    }

    /* free all the elements of the linked list */
    for( ; p = head; ) {
      head = p->next;
      _bfree( segu, p );
    }
    /* free the based heap */
    _bfreeseg( segu );
	printf("program=%P\n", *argv[0]);
	printf("seg=%P\n", segu);
}
