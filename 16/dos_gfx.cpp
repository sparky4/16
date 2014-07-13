//DOS Graphics thingy by sparky4 licence GPL v2
//a special thanks to everyone to release source code for mode X
/*src\lib\*/
#include "dos_gfx.h"

int old_mode;
//color ‚Ä‚·‚Æ
short gq = LGQ;

//‚Ä‚·‚Æ
short bakax = 0, bakay = 0;
syte xx = rand()&0%SW, yy = rand()&0%SH, sx = 0, sy = 0;
byte coor;

byte *vga = (byte *) MK_FP(0xA000, 0);

/*
 * Comment out the following #define if you don't want the testing main()
 * to be included.
 */
#define TESTING
//#define TILE

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// setvideo() - This function Manages the video modes					  //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
void setvideo(/*byte mode, */short vq){
		union REGS in, out;

		if(!vq){ // deinit the video
				// change to the video mode we were in before we switched to mode 13h
				//mxSetMode( MX_TEXT );
				mxTerm();
				in.h.ah = 0x00;
				in.h.al = old_mode;
				int86(0x10, &in, &out);

		}else if(vq==1){ // init the video
				// get old video mode
				in.h.ah = 0xf;
				int86(0x10, &in, &out);
				old_mode = out.h.al;

				// enter mode
				mxInit();
				mxSetMode( MX_320x240 );
//				mxSetVirtualScreen(SW+(SW/4), SH+(SH/4));
//				mxSetVirtualScreen(SW*2, SH*2);
				mxSetVirtualScreen(VW,BH);
//				mxSetVirtualScreen((640-(TILEWH*4)),(480-(TILEWH*4)));
				mxSetClip(true);
				mxSetClipRegion(0, 0, VW, BH);
				mxPan(TILEWH*2,TILEWH*2);
				//mxSetClipRegion(0, VH+1, VW, (TILEWH*BUFFMX));
		}
}

void pdump(){
	int mult=(QUADWH)/2;
	int palq=(mult)*16;
	int palcol=0;
	for(int paly=0; paly<palq; paly+=mult){
		for(int palx=0; palx<palq; palx+=mult){
				mxFillBox((SW-palq)+palx+32, paly+32, TILEWH, TILEWH, palcol, OP_SET);
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

//color ‚Ä‚·‚Æ
short colortest(){
		if(gq < NUM_COLORS){
				cls(gq, vga);
				gq++;
		}else gq = 0;
		return gq;
}

//color ‚Ä‚·‚Æ
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
short ding(int q){

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

		if(q==8){ colorz(); return gq; mxOutText(SW/2, SH/2, "bakapi"); }else
		if(q==10){ ssd(q); /*printf("%d\n", coor);*/ }else
		if(q==5){ colortest(); return gq; }else
		if(q==11){ colorz(); delay(100); return gq; }
		if(q==6){
				coor = rand()%NUM_COLORS;
				cls(coor, vga);
				//updatevbuff();
		}

		if(q==7||q==9){
				if(gq <= HGQ){
						if(q == 7) ssd(q);
						if(q == 9){ ssd(q); coor++; }
						gq++;
				}else gq = LGQ;
		}
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
				mxPutPixel(tx, ty, coor);
				//printf("%d %d %d %d %d %d\n", xx, yy, tx, ty, TILEWH);

				// plot the pixel
				}else{
					#ifdef TILE
					mxFillBox(xx, yy, TILEWH, TILEWH, coor, OP_SET);
					#else
					mxPutPixel(xx, yy, coor);
					#endif
				} 

				if(q==2)
				#ifdef TILE
					mxFillBox((rand()*TILEWH)%VW, (rand()*TILEWH)%(VH), TILEWH, TILEWH, 0, OP_SET);
				#else
					mxPutPixel(rand()%VW, rand()%(VH), 0);
				#endif
				if(q==16) mxPutPixel(rand()%VW, rand()%(VH), 0);
				if(q==2||q==4||q==16){ bakax = rand()%3; bakay = rand()%3; }  //random 3 switch
				gq++;
//if(xx<0||xx>320||yy<0||yy>(SH*3))
//	  printf("%d %d %d %d %d\n", xx, yy, coor, bakax, bakay);
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
		short key,d,xpos,ypos,xdir,ydir;
		int ch=0x0;
		// main variables
		d=4; // switch variable
		key=2; // default screensaver number
		xpos=TILEWH*2;
		ypos=TILEWH*2;
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
		//mxFillBox(0, 0, VW, BH, 10, OP_SET);
		while(1){ // conditions of screen saver
		while(!kbhit()){
			ding(key);
		}
		//end of screen savers
		//doTest();
/*		for (int x = 0; x < VW; ++x)
			{
				mxPutPixel(x, 0, 15);
				mxPutPixel(x, SH-1, 15);
			}
		for (int y = 0; y < VH; ++y)
			{
				mxPutPixel(0, y, 15);
				mxPutPixel(SW-1, y, 15);
			}
		for (int x = 0; x < VW; ++x)
			{
				mxPutPixel(x, 0, 15);
				mxPutPixel(x, VH-1, 15);
			}
		for (int y = 240; y < VH; ++y)
			{
				mxPutPixel(0, y, 15);
				mxPutPixel(VW-1, y, 15);
			}
			*/
			pdump();
			getch();
			//text box
			mxSetTextColor(10, OP_TRANS); //set font
			mxBitBlt(xpos, ypos+(TILEWH*12), SW, TILEWH*BUFFMX, 0, BS); //copy background
			mxFillBox(xpos, ypos+(TILEWH*12), SW, TILEWH*BUFFMX, 0, OP_SET); // background for text box
			//+(QUADWH*6)
			mxOutText(xpos+1, ypos+SH-48, "========================================");
			mxOutText(xpos+1, ypos+SH-40, "|    |Chikyuu:$line1");
			mxOutText(xpos+1, ypos+SH-32, "|    |$line2");
			mxOutText(xpos+1, ypos+SH-24, "|    |$line3");
			mxOutText(xpos+1, ypos+SH-16, "|    |$line4");
			mxOutText(xpos+1, ypos+SH-8,  "========================================");
			mxFillBox(xpos+QUADWH, ypos+QUADWH+(TILEWH*12), TILEWH*2, TILEWH*2, 9, OP_SET); //portriat~
			getch();
			mxBitBlt(0, BS, SW, TILEWH*BUFFMX, xpos, ypos+(TILEWH*12)); //copy background
			getch();
		while(!kbhit()){
			//for(int i=0;i<TILEWH;i++){
				
				ding(key);
				mxPan(xpos,ypos);
mxFillBox(384, 304, 384, 304, 10, OP_SET);
//mxBitBlt(xpos, ypos, SW, SH, 32, (SH+64+32));
mxBitBlt(TILEWH*2, TILEWH*2, SW, SH, 32, (SH+64+32));
				//for(short o = 0; o<TILEWH; o++){
					xpos+=xdir;
					ypos+=ydir;
					//if(ypos==1 || (ypos==(BH-SH-1)))delay(500);
					//if((xpos>(VW-SW-1)) || (xpos<1))delay(500);
					//mxWaitRetrace();
//mxBitBlt(32, (SH+32), SW, SH, xpos, ypos);
mxBitBlt(TILEWH*2, (SH+64+32), SW, SH, TILEWH*2, TILEWH*2);
//xpos=ypos=TILEWH*2;
mxPan(32,32);
				//}
				if( (xpos>(VW-SW-1))  || (xpos<1)){xdir=-xdir;}
				if( (ypos>(BH-SH-1)) || (ypos<1)){ydir=-ydir;} // { Hit a boundry, change
			//    direction! }
//mxBitBlt(32, (SH+64+32), SW, SH, xpos, ypos);
//mxBitBlt(TILEWH*2, (SH+64+32), SW, SH, TILEWH*2, TILEWH*2);
			}
			ch=getch();
			if(ch==0x71)break; // 'q'
			if(ch==0x1b)break; // 'ESC'
		}
		setvideo(0);
		printf("wwww\nFull Buffer Virtual Resolution: %dx%d\n", VW,BH);
		printf("Virtual Resolution: %dx%d\n", VW,VH);
		printf("Resolution: %dx%d\n", SW,SH);
		printf("Mode X Library Version: %d\n", mxGetVersion());
		printf("bakapi ver. 1.04.09.04\nis made by sparky4i†ƒÖ…j feel free to use it ^^\nLicence: GPL v2\n");
		return 0;
		}

#endif
