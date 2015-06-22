#include <stdio.h>
#include "lib/types.h"
#include "lib/text.h"

main() {
    int i;
    int j;
    char l[16];
    char c;
    word s, o;
    word addr = (word) l;
    textInit();

    //print the addresses of the fonts
    printf("ROM FONT Addresses\n");
    for(i=0; i<4; i++) {
	printf("%d: %x:%x\n", i, romFonts[i].seg, romFonts[i].off);
    }

    printf("Character: ");
    scanf("%c", &c);

    s=romFonts[ROM_FONT_8x16].seg;
    o=romFonts[ROM_FONT_8x16].off;

    //load the letter 'A'
    __asm {
		MOV DI, addr
		MOV SI, o
		MOV ES, s
		SUB BH, BH
		MOV BL, c	; the letter
		SHL BX, 1
		SHL BX, 1
		SHL BX, 1
		SHL BX, 1
		ADD SI, BX	;the address of A
		MOV CX, 16
	L1:	MOV AX, ES:SI
		MOV DS:DI, AX
		INC SI
		INC DI
		DEC CX
		JNZ L1
    }

    //render the letter in ascii art
    for(i=0; i<16; i++) {
	j=1<<8;
	while(j) {
	    printf("%c", l[i] & j ? '*':' ');
	    j>>=1;
	}
	printf("\n");
    }
}
