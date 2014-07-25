#include "modex16.h"

word far* clock= (word far*) 0x046C; /* 18.2hz clock */

void main() {
    int i;
    word start;
    float t1, t2;
    page_t page;

    page=modexDefaultPage();

    setvideo(1);
    start = *clock;
    for(i=0; i<500; i++) {
	modexShowPage(&page);
    }
    t1 = (*clock - start)/18.2;
    setvideo(0);

    printf("Time: %f\n", t1);
}
