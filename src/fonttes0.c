/* Project 16 Source Code~
 * Copyright (C) 2012-2015 sparky4 & pngwen & andrius4669
 *
 * This file is part of Project 16.
 *
 * Project 16 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Project 16 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */
#include <stdio.h>
#include <conio.h>
#include "lib/types.h"
#include "lib/16text.h"
#include "lib/modex16.h"

global_game_variables_t gvar;

void main(int argc, char near *argv[])
{
    int i, xp;
    int j;
	char l[16];
    char c,ct;
    word s, o, t, w;
    word addr = (word) l;
    //byte *pal, *pal2, fond;
	//bitmap_t fontdata;
	page_t page;
    textInit();

    //print the addresses of the fonts
    printf("ROM FONT Addresses\n");
    for(i=0; i<4; i++) {
	printf("%d: %x:%x\n", i, romFonts[i].seg, romFonts[i].off);
    }

//	printf("*argv[1]=%d\n", *argv[1]);

	printf("Font type: ");
	gets(&ct);

	printf("Character: ");
	gets(&c);

	if(ct!=1)
	switch(ct)
	{
		case 48:
			printf("type 0\n");
			t=0;
			w=14;
		break;
		case 49:
			printf("type 1\n");
			t=1;
			w=8;
		break;
		case 50:
			printf("type 2\n");
			t=2;
			w=8;
		break;
		case 51:
			printf("type 3\n");
			t=3;
			w=16;
		break;
		default:
			printf("type 3\n");
			t=3;
			w=16;
		break;
	}
	else
	{
		printf("type 3\n");
		t=3;
		w=16;
	}


    s=romFonts[t].seg;
    o=romFonts[t].off;

    //load the letter 'A'
    __asm {
		MOV DI, addr
		MOV SI, o
		MOV ES, s
		SUB AH, AH
		MOV AL, c	; the letter
		MOV CX, w
		MUL CX
		ADD SI, AX	;the address of charcter
	L1:	MOV AX, ES:SI
		MOV DS:DI, AX
		INC SI
		INC DI
		DEC CX
		JNZ L1
    }

    //load our palette
    //modexLoadPalFile("data/default.pal", &pal2);

	/*fontdata.width=8;
	fontdata.height=w;
	fontdata.palette=pal2;

	fontdata.data=malloc(256);

	for(i=0; i<w; i++)
	{
		j=1<<8;
		while(j)
		{
			fond=(byte)l[i] & j ? '1':'0';
			fontdata.data=&(fond);
			printf("%02x ", *(fontdata.data));
			j>>=1;
		}
		printf("\n");
	}

	printf("\n\nok hit a key~\n");
	while(!getch())
	{
		//DrawPBuf(&page, 0, 0, p, 0);
	}*/

    // save the palette
    /*pal  = modexNewPal();
    modexPalSave(pal);
    modexFadeOff(4, pal);
    modexPalBlack();
*/
    VGAmodeX(1, &gvar);
/*    modexPalBlack();*/

    // set up the page, but with 16 pixels on all borders in offscreen mem
    page=modexDefaultPage();
    //page.width += 32;
    //page.height += 32;
	modexShowPage(&page);
	modexClearRegion(&page, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
	// fade in
	//modexFadeOn(4, pal2);

	//render the letter in ascii art
	for(i=0; i<w; i++)
	{
		j=1<<8;
		xp=0;
		while(j)
		{
			//printf("j=%d\n", j);
			//printf("%c", l[i] & j ? '*':' ');
			//====printf("%02x ", l[i] & j);
			//printf("l[i]=%d	", l[i]);
			//printf("j=%d	", j);
			//printf("%02x\n", l[i] & j);
				//modexPutPixel(xp, i, l[i] & j ? 15:0);
				//modexpixelwr(xp, i, 0, l[i] & j ? 15:0);
				modexputPixel(&page, xp, i, l[i] & j ? 15:0);
				//modexClearRegion(&page, xp*4, i*4, 4, 4, l[i] & j ? 15:0);
//while(!getch());
			xp++;
			j>>=1;
		}
		//====
		//printf("\n");
	}
	//modexDrawSprite(&page, 0, 0, &fontdata);
	while(!getch())
	{
		//DrawPBuf(&page, 0, 0, p, 0);
	}

	// fade back to text mode
	/*modexFadeOff(4, pal2);
	modexPalBlack();*/
	VGAmodeX(0, &gvar);
	/*modexPalBlack();
	modexFadeOn(4, pal);*/
	//render the letter in ascii art
	for(i=0; i<w; i++)
	{
		j=1<<8;
		while(j)
		{
			printf("%c", l[i] & j ? '*':' ');
			//====printf("%02x ", l[i] & j);
//while(!getch());
		j>>=1;
		}
		printf("\n");
	}
	printf("\ni=%d\n", i);
	printf("j=%d\n", j);

}
