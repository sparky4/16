#include "modex16.h"
#include <stdio.h>

word far* clock= (word far*) 0x046C; /* 18.2hz clock */

void main() {
    int plane;
    int x, y, i;
    word start, end;
    page_t page, page2;
    word far* ptr;
    float elapsed;
    byte *pal, *pal2=NULL;

    /* load our palette */
    modexLoadPalFile("gfx.pal", &pal2);

    /* save the palette */
    pal  = modexNewPal();
    modexPalSave(pal);
    modexFadeOff(1, pal);
    modexPalBlack();;

    modexEnter();
    modexPalBlack();

    page= VGA;
    page2=VGA+PAGE_SIZE;

    /* fill the page with one color */
    modexShowPage(page2);
    modexClearRegion(page, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 42);
    modexClearRegion(page, 17, 12, 30, 30, 19);
    modexShowPage(page);

    /* fade in */
    modexFadeOn(1, pal2);

    /* fill page2 up */
    modexClearRegion(page2, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 128);


    start = *clock;
    for(i=0; i<500; i++) {
	modexShowPage(page);
	page+=SCREEN_WIDTH/4;
    }
    end = *clock;

    /* fade back to text mode */
    modexFadeOff(1, pal2);
    modexPalBlack();
    modexLeave();
    modexPalBlack();
    modexFadeOn(1, pal);
    elapsed = (end-start)/18.2;
    printf("500 frames in %f seconds for %f fps\n", elapsed, 500.0/elapsed);
}
