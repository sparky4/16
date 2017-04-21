#include "modex16.h"

word far* clock= (word far*) 0x046C; /* 18.2hz clock */

void main() {
    int i;
    word start;
    page_t page;

    page=modexDefaultPage();

    modexEnter();
    start = *clock;
    for(i=0; i<500; i++) {
	modexShowPage(&page);
    }
    modexLeave();

}
