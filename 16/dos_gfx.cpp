//DOS Graphics thingy by sparky4 licence GPL v2
//a special thanks to everyone to release source code for mode X
/*src\lib\*/
#include "dos_gfx.h"

int old_mode;
//color ‚Ä‚·‚Æ
int gq = LGQ;
//‚Ä‚·‚Æ
int q = 0;
int bakax = 0, bakay = 0;
cord xx = rand()&0%320, yy = rand()&0%240, sx = 0, sy = 0;
byte coor;

byte *vga = (byte *) MK_FP(0xA000, 0);

/*
 * Comment out the following #define if you don't want the testing main()
 * to be included.
 */
#define TESTING

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// setvideo() - This function Manages the video modes					  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
void setvideo(/*byte mode, */int vq){
		union REGS in, out;

		if(!vq){ // deinit the video
				// change to the video mode we were in before we switched to mode 13h
				//mxSetMode( MX_TEXT );
				mxTerm();
				in.h.ah = 0x00;
				in.h.al = old_mode;
				int86(0x10, &in, &out);

		}else if(vq == 1){ // init the video
				// get old video mode
				in.h.ah = 0xf;
				int86(0x10, &in, &out);
				old_mode = out.h.al;

				// enter mode
				mxInit();
				mxSetMode( MX_320x240 );
//				mxSetVirtualScreen(SW+(SW/4), SH+(SH/4));
//				mxSetVirtualScreen(SW*2, SH*2);
				mxSetVirtualScreen(VW,VH);
//				mxSetVirtualScreen((640-(TILEWH*4)),(480-(TILEWH*4)));
				mxSetClip(0);
				//mxSetClipRegion(0, 0, SW, SH);
		}
}

/////////////////////////////////////////////////////////////////////////////
//																		 //
// cls() - This clears the screen to the specified color, on the VGA or on //
//		 the Virtual screen.											 //
//																		 //
/////////////////////////////////////////////////////////////////////////////
void cls(byte color, byte *Where){
		_fmemset(Where, color, VW*(VH));
}

//color ‚Ä‚·‚Æ
int colortest(){
		if(gq < NUM_COLORS){
				cls(gq, vga);
				gq++;
		}else gq = 0;
		return gq;
}

//color ‚Ä‚·‚Æ
int colorz(){
		if(gq < HGQ){
//----		  cls(gq, vaddr);
				cls(gq, vga);
				gq++;
		}else gq = LGQ;
		return gq;
}

//slow spectrum down
void ssd(int svq){
		if(sy < SH+1){
				if(sx < SW+1){
						//plotpixel(xx, yy, coor, vga);
						//ppf(sx, sy, coor, vga);
						mxPutPixel(sx, sy, coor);
						//printf("%d %d %d %d\n", sx, sy, svq, coor);
						sx++;
				}else sx = 0;
				if(sx == SW){
						sy++;
						if(svq == 7) coor++;
						if(sy == SH && svq == 8) coor = rand()%NUM_COLORS;
				}
		}else sy = 0;
}

/*-----------ding-------------*/
int ding(int q){

		int d3y;

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

		if(q==8){ colorz(); return gq; }else
		if(q==10){ ssd(q); /*printf("%d\n", coor);*/ }else
		if(q==5){ colortest(); return gq; }else
		if(q==11){ colorz(); delay(100); return gq; }
		if(q==6){
				coor = rand()%NUM_COLORS;
				cls(coor, vga);
				//updatevbuff();
		}

		if(q==7||q==9){
				if(gq < HGQ){
						if(q == 7) ssd(q);
						if(q == 9){ ssd(q); coor++; }
						gq++;
				}else gq = LGQ;
		}
		if((q<5 && gq<BONK) || (q==16 && gq<BONK)){ // the number variable make the colors more noticable
				if(q==1){
						if(xx==SW){bakax=0;}
						if(xx==0){bakax=1;}
						if(yy==SH){bakay=0;}
						if(yy==0){bakay=1;}
				}else if(q==3){
						if(xx!=SW||yy!=SH){
								if(xx==0){bakax=1;bakay=-1;d3y=1;}
								if(yy==0){bakax=1;bakay=0;d3y=1;}
								if(xx==SW){bakax=-1;bakay=-1;d3y=1;}
								if(yy==SH){bakax=1;bakay=0;d3y=1;}
						}else if(xx==SW&&yy==SH) xx=yy=0;
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
									xx-=TILEWH;
//									xx--;
								}else if(bakax>1){
									xx+=TILEWH;
//									xx++;
								}
								if(!bakay){
									yy-=TILEWH;
//									yy--;
								}else if(bakay>1){
									yy+=TILEWH;
//									yy++;
								}
						}
				}
				// fixer
				if(q!=16){
						if(xx<0) xx=(VW/*-TILEWH*/);
						if(yy<0) yy=(VH/*-TILEWH*/);
						if(xx>(VW/*-TILEWH*/)) xx=0;
						if(yy>(VH/*-TILEWH*/)) yy=0;
				}

//interesting effects
				if(q==16)
				{
				int tx=0,ty=0;
				tx+=xx+16;
				ty+=yy+16;
				mxPutPixel(tx, ty, coor);
				//printf("%d %d %d %d %d %d\n", xx, yy, tx, ty, TILEWH);

				// plot the pixel
				}else{
					mxFillBox(xx, yy, TILEWH, TILEWH, coor, 0);
//					mxPutPixel(xx, yy, coor);
				} 

				if(q==2) mxPutPixel(rand()%SW, rand()%(SH*3), 0);
				if(q==16) mxPutPixel(rand()%SW, rand()%(SH*3), 0);
				if(q==2||q==4||q==16){ bakax = rand()%3; bakay = rand()%3; }
				gq++;
//if(xx<0||xx>320||yy<0||yy>(SH*3))
//	  printf("%d %d %d %d %d %d\n", xx, yy, coor, bakax, bakay, getPixel_X(xx,yy));
//	  printf("%d\n", getPixel_X(xx,yy));
		}else gq = LGQ;
		return gq;
}


/*
 * The library testing routines follows below.
 */

#ifdef TESTING

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
		setvideo(1);
// screen savers

/*while(d!=0){ // on!
				if(!kbhit()){ // conditions of screen saver
						ding(key);
				}else{
						setvideo(0);
						// user imput switch
						printf("Enter 1, 2, 3, 4, or 6 to run a screensaver, or enter 5 to quit.\n", getch());  // prompt the user
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
		for (int x = 0; x < VW; ++x)
			{
				mxPutPixel(x, 0, 15);
				mxPutPixel(x, SH-1, 15);
			}
		for (int y = 0; y < VH; ++y)
			{
				mxPutPixel(0, y, 15);
				mxPutPixel(SW-1, y, 15);
			}

		getch();
		for (int x = 320; x < VW; ++x)
			{
				mxPutPixel(x, 0, 15);
				mxPutPixel(x, VH-1, 15);
			}
		for (int y = 240; y < VH; ++y)
			{
				mxPutPixel(0, y, 15);
				mxPutPixel(VW-1, y, 15);
			}
		mxSetTextColor(16, OP_TRANS);
				mxOutText(64, SH-40, "CRAZY!!!!");
				mxOutText(64, SH-32, "CRAZY!!!!");
				mxOutText(64, SH-24, "  _  CRAZY!!!!");
				mxOutText(64, SH-16, "  _    _  CRAZY!!!!");
		getch();
		while(!kbhit()){
//			hScroll(1);
//			scrolly(1);
//			vScroll(1);
//			delay(100);
			//for(int i=0;i<TILEWH;i++){
				ding(key);
				mxOutText(64, SH-40, "CRAZY!!!!");
				mxOutText(64, SH-32, "CRAZY!!!!");
				mxOutText(64, SH-24, "  _  CRAZY!!!!");
				mxOutText(64, SH-16, "  _    _  CRAZY!!!!");
				mxPan(xpos,ypos);
				//mxWaitRetrace();
				xpos+=xdir;
				ypos+=ydir;
				if( (xpos>(VW-SW-1))  || (xpos<1)){xdir=-xdir;}
				if( (ypos>(VH-SH-1)) || (ypos<1)){ydir=-ydir;} // { Hit a boundry, change
			//    direction! }
			//}
		}
		setvideo(0);
		printf("wwww\nResolution: %dx%d\n", SW,SH);
		printf("Mode X Library Version: %d\n", mxGetVersion());
		//puts("where to next?  It's your move! wwww");
		printf("bakapi ver. 1.04.09.04\nis made by sparky4i†ƒÖ…j feel free to use it ^^\nLicence: GPL v2\n");
		return 0;
		}

#endif
