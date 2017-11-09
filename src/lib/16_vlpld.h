#ifndef __16_VLPALD_H__
#define __16_VLPALD_H__

#define BEVERBOSEPALCHECK
//#define BEVERBOSECOLORCHECK
//#define DONOTPALCHECK

#ifdef BEVERBOSECOLORCHECK
#define CHKCOLDBGOUT1 \
		printf("q: %02d\n", (*q));\
		printf("chkcolor start~\n");\
		printf("1				   (*z): %d\n", (*z)/3);\
		printf("1				   (*i): %d\n", (*i)/3);\
		printf("1 offset of color in palette	(*q): %d\n", (*q)/3);\
		printf("wwwwwwwwwwwwwwww\n");
#define CHKCOLDBGOUT2 \
			printf("\n		z: %d\n", (*z));\
			printf("		  q: %d\n", (*q));\
			printf("		  z+q: %d\n\n", ((*z)+(*q)));
#define CHKCOLDBGOUT3 \
			printf("\n%d	[%02d][%02d][%02d]\n", (*z), pal[(*z)], pal[(*z)+1], pal[(*z)+2]);\
			printf("%d	  [%02d][%02d][%02d]\n\n", (*z)+3, pal[(*z)+3], pal[(*z)+4], pal[(*z)+5]);
#define CHKCOLDBGOUT4 \
			printf("zz: %02d\n", zz/3);
#define CHKCOLDBGOUT5 \
			printf("\nzq1:%d[%02d][%02d][%02d]\n", (zz+*q), pal[(zz+*q)], pal[(zz+*q)+1], pal[(zz+*q)+2]); \
			printf("zq2:%d[%02d][%02d][%02d]\n\n", (zz+*q)+3, pal[(zz+*q)+3], pal[(zz+*q)+4], pal[(zz+*q)+5]);
//printf("	  zv: %d  [%02d][%02d][%02d] wwww\n", (zz-z+q)/3, pal[(zz-z+q)], pal[(zz-z+q)+1], pal[(zz-z+q)+2]);//
#define CHKCOLDBGOUT6 \
			printf("\n\nwwwwwwwwwwwwwwww\n"); \
			printf("	zq: %d  [%02d][%02d][%02d] value that is needing to be changed~\n", ((*z)+(*q))/3, pal[((*z)+(*q))], pal[((*z)+(*q))+1], pal[((*z)+(*q))+2]); \
			printf("	zz: %d  [%02d][%02d][%02d] value that the previous value is going to change to~\n", (zz)/3, pal[zz], pal[zz+1], pal[zz+2]); \
			printf("	z : %d  [%02d][%02d][%02d] offset value~\n", (*z)/3, pal[(*z)], pal[(*z)+1], pal[(*z)+2]);
#define CHKCOLDBGOUT7 \
			printf("z+q: %d\n\n", ((*z)+(*q)));
#define CHKCOLDBGOUT8 \
			printf("!!					  a[%02d]: %d\n", (((*z)+(*q))/3), zz/3); \
			printf("\n		  aa: %d\n\n", (*aa)); \
			printf("	a[%02d]=(%02d) offset array i think the palette should be updated again~\n", ((*z)+(*q))/3, a[((*z)+(*q))/3]); \
			printf("wwwwwwwwwwwwwwww\n\n");
#define CHKCOLDBGOUT9 \
			printf("[%d]", (zz+*q));
#define CHKCOLDBGOUT10 \
		printf("wwwwwwwwwwwwwwww\n"); \
		printf("2				   (*z): %d\n", (*z)/3); \
		printf("2				   (*i): %d\n", (*i)/3); \
		printf("2 offset of color in palette	(*q): %d\n", (*q)/3); \
		printf("chkcolor end~\n");
#else
#define	CHKCOLDBGOUT1
#define	CHKCOLDBGOUT2
#define	CHKCOLDBGOUT3
#define	CHKCOLDBGOUT4
#define	CHKCOLDBGOUT5
#define	CHKCOLDBGOUT6
#define	CHKCOLDBGOUT7
#define	CHKCOLDBGOUT8
#define	CHKCOLDBGOUT9
#define	CHKCOLDBGOUT10
#endif
//======//

#endif
