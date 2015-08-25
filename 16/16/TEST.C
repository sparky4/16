#include "lib\modex16.h"
#include <stdio.h>

#include <stdlib.h>
//#include <iostream.h>

word far* clock= (word far*) 0x046C; /* 18.2hz clock */

//color てすと
short gq = LGQ;

page_t page, page2;

//てすと
short bakax = 0, bakay = 0;
sword xx = 0, yy = 0, sx = 0, sy = 0;
byte coor;

//byte *vga = (byte *) MK_FP(0xA000, 0);

/*
 * Comment out the following #define if you don't want the testing main()
 * to be included.
 */
//#define TILE


/*void pdump(){
	int mult=(QUADWH)/2;
	int palq=(mult)*16;
	int palcol=0;
	for(int paly=0; paly<palq; paly+=mult){
		for(int palx=0; palx<palq; palx+=mult){
				//mxFillBox((SCREEN_WIDTH-palq)+palx+32, paly+32, TILEWH, TILEWH, palcol, OP_SET);
				modexClearRegion(page_t *page, (SCREEN_WIDTH-palq)+palx+32, paly+32, TILEWH, TILEWH, palcol);
			palcol++;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//																		 //
// cls() - This clears the screen to the specified color, on the VGA or on //
//		 the Virtual screen.											 //
//																		 //
/////////////////////////////////////////////////////////////////////////////
void cls(byte color, byte *Where){
		_fmemset(Where, color, VW*(VH*2));
}

//color てすと
short colortest(){
		if(gq < NUM_COLORS){
				cls(gq, vga);
				gq++;
		}else gq = 0;
		return gq;
}

//color てすと
short colorz(){
		if(gq <= HGQ){
//----		  cls(gq, vaddr);
				cls(gq, vga);
				gq++;
		}else gq = LGQ;
		return gq;
}

//slow spectrum down
void ssd(short svq){
		if(sy < SCREEN_HEIGHT+1){
				if(sx < SCREEN_WIDTH+1){
						//plotpixel(xx, yy, coor, vga);
						//ppf(sx, sy, coor, vga);
						//mxPutPixel(sx, sy, coor);
						modexClearRegion(page_t *page, sx, sy, 1, 1, coor);
						//printf("%d %d %d %d¥n", sx, sy, svq, coor);
						sx++;
				}else sx = 0;
				if(sx == SCREEN_WIDTH){
						sy++;
						if(svq == 7) coor++;
						if(sy == SCREEN_HEIGHT && svq == 8) coor = rand()%NUM_COLORS;
				}
		}else sy = 0;
}*/

/*-----------ding-------------*/
short ding(int q){

		page=modexDefaultPage();
		page2 = modexNextPage(&page);
		page.width += 32;
		page.height += 32;
		short d3y;

//++++  if(q <= 4 && q!=2 && gq == BONK-1) coor = rand()%HGQ;
		if((q == 2
		||q==4
		||q==16
		) && gq == BONK){
						if(coor < HGQ && coor < LGQ) coor = LGQ;
						if(coor < HGQ){
								coor++;
				}else{ coor = LGQ;
						bakax = rand()%3; bakay = rand()%3;
				}
		}

//		if(q==8){ colorz(); return gq; /*mxOutText(SCREEN_WIDTH/2, SCREEN_HEIGHT/2, "bakapi");*/ }else
//		if(q==10){ ssd(q); /*printf("%d¥n", coor);*/ }else
//		if(q==5){ colortest(); return gq; }else
//		if(q==11){ colorz(); delay(100); return gq; }
/*		if(q==6){
				coor = rand()%NUM_COLORS;
				cls(coor, vga);
				//updatevbuff();
		}

		/f(q==7||q==9){
				if(gq <= HGQ){
						if(q == 7) ssd(q);
						if(q == 9){ ssd(q); coor++; }
						gq++;
				}else gq = LGQ;
		}*/
		if((q<5 && gq<BONK) || (q==16 && gq<BONK)){ // the number variable make the colors more noticable
				if(q==1){
						if(xx==VW){bakax=0;}
						if(xx==0){bakax=1;}
						if(yy==VH){bakay=0;}
						if(yy==0){bakay=1;}
				}else if(q==3){
						if(xx!=VW||yy!=VH){
								if(xx==0){bakax=1;bakay=-1;d3y=1;}
								if(yy==0){bakax=1;bakay=0;d3y=1;}
								if(xx==VW){bakax=-1;bakay=-1;d3y=1;}
								if(yy==VH){bakax=1;bakay=0;d3y=1;}
						}else if(xx==VW&&yy==VH) xx=yy=0;
				}
				if(q==3){
						if(d3y){
								if(bakay<0){
										yy--;
										d3y--;
								}else
								if(bakay>0){
										yy++;
										d3y--;
								}
						}
						if(bakax<0){
								xx--;
						}else
						if(bakax>0){
								xx++;
						}
				}else{
						if(q==16)
						{
								if(!bakax){
									xx--;
								}else if(bakax>0){
									xx++;
								}
								if(!bakay){
									yy--;
								}else if(bakay>0){
									yy++;
								}
						}else{
								if(!bakax){
									#ifdef TILE
									xx-=TILEWH;
									#else
									xx--;
									#endif
								}else if(bakax>1){
									#ifdef TILE
									xx+=TILEWH;
									#else
									xx++;
									#endif
								}
								if(!bakay){
									#ifdef TILE
									yy-=TILEWH;
									#else
									yy--;
									#endif
								}else if(bakay>1){
									#ifdef TILE
									yy+=TILEWH;
									#else
									yy++;
									#endif
								}
						}
				}
				// fixer
				if(q!=16){
					#ifdef TILE
						if(xx<0) xx=(VW-TILEWH);
						if(yy<0) yy=(VH-TILEWH);
						if(xx>(VW-TILEWH)) xx=0;
						if(yy>(VH-TILEWH)/*+(TILEWH*BUFFMX)*/) yy=0;
					#else
						if(xx<0) xx=VW;
						if(yy<0) yy=VH;
						if(xx>VW) xx=0;
						if(yy>VH) yy=0;
					#endif
				}

//interesting effects
				if(q==16)
				{
				short tx=0,ty=0;
				tx+=xx+16;
				ty+=yy+16;
				//mxPutPixel(tx, ty, coor);
				modexClearRegion(page_t *page, tx, ty, 1, 1, coor);
				//printf("%d %d %d %d %d %d¥n", xx, yy, tx, ty, TILEWH);

				// plot the pixel
				}else{
					#ifdef TILE
					//mxFillBox(xx, yy, TILEWH, TILEWH, coor, OP_SET);
					modexClearRegion(page_t *page, xx, yy, TILEWH, TILEWH, coor);
					#else
					//mxPutPixel(xx, yy, coor);
					modexClearRegion(page_t *page, xx, yy, 1, 1, coor);
					#endif
				} 

				if(q==2)
				#ifdef TILE
					//mxFillBox((rand()*TILEWH)%VW, (rand()*TILEWH)%(VH), TILEWH, TILEWH, 0, OP_SET);
					modexClearRegion(page_t *page, (rand()*TILEWH)%VW, (rand()*TILEWH)%(VH), TILEWH, TILEWH, 0);
				#else
					//mxPutPixel(rand()%VW, rand()%(VH), 0);
					modexClearRegion(page_t *page, rand()%VW, rand()%(VH), 1, 1, 0);
				#endif
				if(q==16) //mxPutPixel(rand()%VW, rand()%(VH), 0);
					modexClearRegion(page_t *page, rand()%VW, rand()%(VH), 1, 1, 0);
				if(q==2||q==4||q==16){ bakax = rand()%3; bakay = rand()%3; }  //random 3 switch
				gq++;
//if(xx<0||xx>320||yy<0||yy>(SCREEN_HEIGHT*3))
//	  printf("%d %d %d %d %d¥n", xx, yy, coor, bakax, bakay);
		}else gq = LGQ;
		return gq;
}

void main() {
    int i, j;
    bitmap_t bmp;
    word start, end;
	//page_t page, page2;
    float elapsed;
    byte *pal, *pal2=NULL;

	/* load pcx file */
	bmp = modexLoadPcx("ed.pcx");

    /* load our palette */
    modexLoadPalFile("gfx.pal", &pal2);

    /* save the palette */
    pal  = modexNewPal();
    modexPalSave(pal);
    modexFadeOff(1, pal);
    modexPalBlack();

    setvideo(1);
    modexPalBlack();

    /* set up the page, but with 16 pixels on all borders in offscreen mem */
    page=modexDefaultPage();
    page2 = modexNextPage(&page);
    page.width += 32;
    page.height += 32;


    /* fill the page with one color, but with a black border */
    modexShowPage(&page2);
    modexClearRegion(&page, 16, 16, SCREEN_WIDTH, SCREEN_HEIGHT, 128);
    modexClearRegion(&page, 32, 32, SCREEN_WIDTH-32, SCREEN_HEIGHT-32, 42);
    modexClearRegion(&page, 48, 48, SCREEN_WIDTH-64, SCREEN_HEIGHT-64, 128);
    modexShowPage(&page);

    modexDrawSprite(&page, 20, 20, &bmp);
    //modexDrawBmp(&page, xb, yb, &bmp);

    /* fade in */
    modexFadeOn(1, pal2);


    start = *clock;
    //for(i=0; i<5; i++) {
    while (!kbhit()){  /* Wait for a keystroke                         */
	/* go right */
	for(j=0; j<32; j++) {
	    page.dx++;
	    modexShowPage(&page);
	}
	/* go left */
	for(j=0; j<32; j++) {
	    page.dx--;
	    modexShowPage(&page);
	}
	/* go up */
	for(j=0; j<32; j++) {
	    page.dy++;
	    modexShowPage(&page);
	}

	/* go down */
	for(j=0; j<32; j++) {
	    page.dy--;
	    modexShowPage(&page);
	}
    }

    (void) getch();   /* Clear the keyboard buffer                    */
    end = *clock;

    /* fade back to text mode */
    modexFadeOff(1, pal2);
    modexPalBlack();
    setvideo(0);
    modexPalBlack();
    modexFadeOn(1, pal);
}
