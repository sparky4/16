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

void main(int argc, char *argv[])
{
    int i;
    int j;
	char l[16];
    char c;
    word s, o, t, w;
    word addr = (word) l;
    textInit();

    //print the addresses of the fonts
    printf("ROM FONT Addresses\n");
    for(i=0; i<4; i++) {
	printf("%d: %x:%x\n", i, romFonts[i].seg, romFonts[i].off);
    }

	printf("*argv[1]=%d\n", *argv[1]);

    printf("Character: ");
    scanf("%c", &c);

	if(*argv[1]!=1)
	switch(*argv[1])
	{
		case 48:
			t=0;
			w=14;
		break;
		case 49:
			t=1;
			w=8;
		break;
		case 50:
			t=2;
			w=8;
		break;
		case 51:
			t=3;
			w=16;
		break;
		default:
			t=3;
			w=16;
		break;
	}
	else
	{
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

    //render the letter in ascii art
    for(i=0; i<w; i++) {
	j=1<<8;
	while(j) {
	    printf("%c", l[i] & j ? '*':' ');
	    j>>=1;
	}
	printf("\n");
    }
}
