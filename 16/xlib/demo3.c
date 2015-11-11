#include <stdio.h>
#include "xlib.h"
#include "xdetect.h"
int processor;
void main(void){
  printf("Hardware detection demo...\n\n");
    printf("Graphics Card    : ");
      switch (x_graphics_card()){
        case NoGraphics:  printf("None\n"); break;
        case MDA:         printf("MDA\n"); break;
        case CGA:         printf("CGA\n"); break;
        case EGAMono:     printf("EGA Mono\n"); break;
        case EGAColor:    printf("EGA Color\n"); break;
        case VGAMono:     printf("VGA Mono\n");
        case VGAColor:    printf("VGA Color\n"); break;
        case MCGAMono:    printf("MCGA mono\n"); break;
        case MCGAColor:   printf("MCGA Color\n"); break;
      };
      printf("Processor        : ");
      switch (processor=x_processor()){
        case I8086:  printf("8088 or 8086\n"); break;
        case I80186: printf("80186\n"); break;
        case I80286: printf("80286\n"); break;
        case I80386SX: printf("80386-SX\n"); break;
        case I80386DX: printf("80386-DX\n"); break;
      };

      if (processor==I80486){
        if (x_coprocessor()) printf("80486-DX or 80486-SX+FPU\n");
        else printf("80486-SX\n");
      } else {
        printf("Math Co-Processor: ");
        switch (x_coprocessor()){
          case 0: printf("8088 Assumed\n"); break;
          case 1: switch (processor=x_processor()){
            case I8086:  printf("80287 Assumed\n"); break;
            case I80186: printf("80187 Assumed\n"); break;
            case I80286: printf("0287 Assumed\n"); break;
            case I80386SX:
	    case I80386DX: printf("80387 Assumed\n"); break;
          };
        };
      };
      printf("Mouse Driver     : ");
      switch (x_mousedriver()){
        case 0: printf("not installed\n"); break;
        case 1: {
          printf("installed (Mouse Driver vers %d.%d)\n",
            (MouseVersion>>8),(MouseVersion&0xff));
          printf("                   ");
          switch (MouseType) {
            case BUS_MOUSE: printf("Bus Mouse on IRQ");break;
            case SERIAL_MOUSE: printf("Serial Mouse on IRQ ");break;
            case INPORT_MOUSE: printf("Inport Mouse on IRQ ");break;
            case PS2_MOUSE: printf("PS/2 Mouse on IRQ ");break;
            case HP_MOUSE:  printf("HP Mouse on IRQ ");break;
            default      :  printf("Unknown Mouse on IRQ ");break;
          }
          printf("%d with %d buttons.",MouseIRQ, MouseButtonCount);
        }
      }
    }