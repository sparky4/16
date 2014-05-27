/*
 * The library testing routines follows below.
 */


#ifdef TESTING

#include <stdio.h>
#include <conio.h>

void doTest(void)
		{
		int p, x, y, pages;

		/* This is the way to calculate the number of pages available. */
		pages = 65536L/(widthBytes*height); // apparently this takes the A000 address
//		if(height==240) pages++;

//		printf("%d¥n", pages);

		for (p = 0; p <= pages; ++p)
				{
				setActivePage(p);

				/* On each page draw a single colored border, and dump the palette
				   onto a small square about the middle of the page. */

				   //{
						for (x = 0; x <= width; ++x)
								{
//								putPixel_X(x, 0, p+1);
								mxPutPixel(x, 0, p+1);
								if(p!=pages) mxPutPixel(x, height-1, p+1);
										else if(height==240) mxPutPixel(x, 99-1, p+1);
								}

						for (y = 0; y <= height; ++y)
								{
								mxPutPixel(0, y, p+1);
								if(p!=pages) mxPutPixel(width-1, y, p+1);
										else if(height==240) mxPutPixel(width-1, y, p+1);
								}

						for (x = 0; x < TILEWH; ++x)
								for (y = 0; y < TILEWH; ++y)
										mxPutPixel(x+(p+2)*16, y+(p+2)*TILEWH, x + y*TILEWH);
						//}

				}

		/* Each pages will now contain a different image.  Let the user cycle
		   through all the pages by pressing a key. */
		for (p = 0; p < pages; ++p)
				{
				setVisiblePage(p);
				getch();
				}

		}

/*
 * Library test (program) entry point.
 */

int main(void)
		{
		int key,d,xpos,ypos,xdir,ydir;
		//short int temp;
		// main variables
		d=1; // switch variable
		key=4; // default screensaver number
		xpos=0;
		ypos=0;
		xdir=1;
		ydir=1;
//	  puts("First, have a look at the 320x200 mode.  I will draw some rubbish");
//	  puts("on all of the four pages, then let you cycle through them by");
//	  puts("hitting a key on each page.");
//	  puts("Press a key when ready...");
//	  getch();

//	  doTest();

//	  puts("Then, check out Mode X, 320x240 with 3 (and a half) pages.");
//	  puts("Press a key when ready...");
//	  getch();

//++++0000
		setvideo(1);
// screen savers

/*while(d!=0){ // on!
				if(!kbhit()){ // conditions of screen saver
						ding(key);
				}else{
						setvideo(0);
						// user imput switch
						printf("Enter 1, 2, 3, 4, or 6 to run a screensaver, or enter 5 to quit.¥n", getch());  // prompt the user
						scanf("%d", &key);
						//if(key==3){xx=yy=0;} // crazy screen saver wwww
						if(key==5) d=0;
						setvideo(1);
				}
		}*/ // else off
		while(!kbhit()){ // conditions of screen saver
			ding(key);
		}
		//end of screen savers
		//doTest();
		for (int x = 0; x < width; ++x)
			{
				mxPutPixel(x, 0, 15);
				mxPutPixel(x, height-1, 15);
			}
		for (int y = 0; y < height; ++y)
			{
				mxPutPixel(0, y, 15);
				mxPutPixel(width-1, y, 15);
			}
		getch();
		while(!kbhit()){
//			hScroll(1);
//			scrolly(1);
//			vScroll(1);
//			delay(100);
			//for(int i=0;i<TILEWH;i++){
				ding(key);
				mxPan(xpos,ypos);
				mxWaitRetrace();
				xpos=xpos+xdir;
				ypos=ypos+ydir;
				if( (xpos>239)  || (xpos<1))xdir=-xdir;
				if( (ypos>179) || (ypos<1))ydir=-ydir; // { Hit a boundry, change
			//    direction! }
			//}
		}
		setvideo(0);
		printf("wwww¥n%dx%d¥n", width,height);
		printf("[%d]¥n", mxGetVersion());
		puts("where to next?  It's your move! wwww");
		printf("bakapi ver. 1.04.09.04¥nis made by sparky4（≧ω≦） feel free to use it ^^¥nLicence: GPL v2¥n");
		return 0;
		}

#endif
#endif