/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// TUTPROG4.CPP - VGA Trainer Program 4 (in Turbo C++ 3.0)                 //
//                                                                         //
// "The VGA Trainer Program" is written by Denthor of Asphyxia. However it //
// was limited to Pascal only in its first run.  All I have done is taken  //
// his original release, translated it to C++ and touched up a few things. //
// I take absolutely no credit for the concepts presented in this code and //
// am NOT the person to ask for help if you are having trouble.            //
//                                                                         //
// Program Notes : This program implements virtual screens, a great way    //
//                 to update your screen.                                  //
//                                                                         //
//                 For this particular program, I have found the compiler  //
//                 option -mc (Compact memory model) to work better than   //
//                 -ml (Large memory model).  However, you must use -mc or //
//                 greater.                                                //
//                 Also, you might want to go under "Options...Debugger"   //
//                 and increase your programs Heap size to >64k.  I don't  //
//                 know if <64k will lock your system, but I had problems. //
//                                                                         //
// Author        : Grant Smith (Denthor) - denthor@beastie.cs.und.ac.za    //
// Translator    : Christopher G. Mann   - r3cgm@dax.cc.uakron.edu         //
//                                                                         //
// Last Modified : December 23, 1994                                       //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
#include "src\lib\dos_gfx.h"

// declare a pointer to the offset of the Virtual Screen
byte *vaddr = NULL;
// declare a pointer to the offset of the VGA memory
byte *vga = (byte *) MK_FP(VMEM, 0);
int old_mode;
//color てすと
int gq = LGQ;
//てすと
int q = 0;
int bakax = 0, bakay = 0;
int xx = 0, yy = 0, sx = 0, sy = 0;
byte coor;

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// setvideo() - This function Manages the video modes                      //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
void setvideo(byte mode, int vq){
	union REGS in, out;

	if(!vq){ // deinit the video
		// change to the video mode we were in before we switched to mode 13h
		in.h.ah = 0x00;
		in.h.al = old_mode;
		int86(0x10, &in, &out);
	}else if(vq == 1){ // init the video
		// get old video mode
		in.h.ah = 0xf;
		int86(0x10, &in, &out);
		old_mode = out.h.al;

		// enter mode
		in.h.ah = 0x00;
		in.h.al = mode;
		// vesa
		//in.x.ax = 0x4f02;
		//in.x.bx = mode;
		int86(0x10, &in, &out);
	}
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// setvbuff() - This manages the memory needed for the virtual screen.     //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
// manages the buffer/virtual screen
void setvbuff(int vq){
	if(!vq){
		free(vaddr);
	}else if(vq == 1){
		vaddr = (byte *) calloc(SW*SH, 1);

		if(vaddr == NULL){
			setvideo(0x03, 1);
			cout << "Not enough video memory available, exiting program....";
			exit(1);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// updatevbuff() - This flips the virtual screen to the VGA screen.        //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
// updates the buffer/virtual screen
void updatevbuff(){
/*	// wait for vertical re-trace
	while(inportb(INPUT_STATUS_0) & 8);
	while(!(inportb(INPUT_STATUS_0) & 8));*/

	// copy everything to video memory
	_fmemcpy(vga, vaddr, SW*SH);
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// cls() - This clears the screen to the specified color, on the VGA or on //
//         the Virtual screen.                                             //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
void cls(byte color, byte *Where){
	_fmemset(Where, color, SW*SH);
}

// clears the entire video output
void clearscr(){
	cls(0, vga);
	cls(0, vaddr);
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// plotpixel() - This puts a pixel at X,Y using color Col, on VGA or the    //
//              Virtual Screen;                                            //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
void plotpixel(int x, int y, byte color, byte *Where){
	memset(Where+(x+(y*SW)),color,1);
}

// king_crimson's code
void plotpixelfast(int x, int y, byte color, byte *Where) {
	byte far *vidptr;
	vidptr = (byte far *)(Where + y*SW + x);
	*vidptr = color;
}

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// BlockMove() - This tests various ways of moving a block around the      //
//               screen.                                                   //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////
//てすと
void BlockMove(){
	int loop1, loop2, loop3;

	// This draws a block directly to the VGA with no flipping
	for(loop1=1; loop1<271; loop1++){
	for(loop2=1; loop2<51; loop2++)
		for(loop3=1; loop3<51; loop3++)
			plotpixel(loop1+loop2,loop3,32, vga);
		cls(0,vga);
	}

	//`
	for(loop1=1; loop1<281; loop1++){
	for(loop2=1; loop2<41; loop2++)
		for(loop3=1; loop3<41; loop3++)
			plotpixel(loop1+loop2,loop3,14, vaddr);
		updatevbuff();
		cls(0,vaddr);
	}

/*	// This draws to the virtual screen, waits for retrace, then flips to VGA
	for(loop1=1; loop1<51; loop1++){
	for(loop2=1; loop2<51; loop2++)
		for(loop3=1; loop3<51; loop3++)
			plotpixel(loop1+loop2,loop3,32, vaddr);
//		WaitRetrace();
		updatevbuff();
		cls(0,vaddr);
	}*/
}

void etesuto(int x = 160, int y = 100){
	memset(vaddr+(x+(y*SW)),gq-1,1);
	updatevbuff();
}

void qtesuto(int x = 160, int y = 100, int color = 0){
	memset(vaddr+(x+(y*SW)),color,1);
	updatevbuff();
}

//color てすと
int colortest(){
	if(gq < NUM_COLORS){
		cls(gq, vaddr);
		updatevbuff();
		gq++;
	}else gq = 0;
	return gq;
}

//color てすと
int colorz(){
	if(gq < HGQ){
		cls(gq, vaddr);
		updatevbuff();
		gq++;
	}else gq = LGQ;
	return gq;
}

//slow spectrum down
void ssd(int svq){
	if(sy < SH+1){
		if(sx < SW+1){
			//plotpixel(xx, yy, coor, vga);
			plotpixelfast(sx, sy, coor, vga);
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

	if(q <= 4 && gq == BONK-1) coor = rand()%NUM_COLORS;

	if(q == 5){ colortest(); return gq; }
	if(q == 10){ colorz(); return gq; }
	if(q == 11){ colorz(); delay(100); return gq; }
	if(q == 8){ ssd(q); /*printf("%d\n", coor);*/ }
	if(q == 6){
		coor = rand()%NUM_COLORS;
		cls(coor, vaddr);
		updatevbuff();
	}

	if(q == 7 || q== 9){
		if(gq < HGQ){
			if(q == 7) ssd(q);
			if(q == 9){ ssd(q); coor++; }
			gq++;
		}else gq = LGQ;
	}
	if(q<5 && gq<BONK){ // the number variable make the colors more noticable
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
			if(!bakax){
				xx--;
			}else{
				xx++;
			}
			if(!bakay){
				yy--;
			}else{
				yy++;
			}
		}
		// plot the pixel
		plotpixelfast(xx, yy, coor, vga);
		if(q==2) plotpixelfast(rand()%SW, rand()%SH, 0, vga);
		if(q==2||q==4){ bakax = rand()&0x1; bakay = rand()&0x1; }
		gq++;
//		printf("%d %d %d %d\n", xx, yy, gq, coor);
	}else gq = LGQ;
	return gq;
}
