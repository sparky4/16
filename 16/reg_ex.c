#include <stdio.h>



void main()
{
	//GENERAL PURPOSE
	short _ax,_bx,_cx,_dx;
	short _si,_di;
	short _bp,_sp;

	short _cs,_ds,_es,_ss;	//SEGMENT
	short _ip;	//SPECIAL PURPOSE
	char _cf,_pf,_af,_zf,_sf,_tf,_if,_df,_of;	//FLAGS

	__asm {
		mov _ax,ax
		mov _bx,bx
		mov _cx,cx
		mov _dx,dx

		mov _si,si
		mov _di,di
		mov _bp,bp
		mov _sp,sp

		mov _cs,cs
		mov _ds,ds
		mov _es,es
		mov _ss,ss

		/*mov _ip,ip

		mov _cf,cf
		mov _pf,pf
		mov _af,af
		mov _zf,zf
		mov _sf,sf
		mov _tf,tf
		mov _if,if
		mov _df,df
		mov _of,of*/
	}
//	printf("integer values:	ax=%d	bx=%d	cx=%d	dx=%d\n", a, b, c, d);
//	printf("unsigned values:ax=%u	bx=%u	cx=%u	dx=%u\n", a, b, c, d);
	printf("16 bit 8088 register values\n");
	printf("general purpose:\n");
	printf("	ax=%d\n	bx=%d\n	cx=%d\n	dx=%d\n\n", _ax, _bx, _cx, _dx);
	printf("	si=%d\n	di=%d\n	bp=%d\n	sp=%d\n", _si, _di, _bp, _sp);
	printf("segment:\n");
	printf("	cs=%d\n	ds=%d\n	es=%d\n	ss=%d\n", _cs, _ds, _es, _ss);
//	printf("	ip=%d\n\n", _ip);
//	printf("	cf=%d\npf=%d\naf=%d\nzf=%d\nsf=%d\ntf=%d\nif=%d\ndf=%d\nof=%d\n", _cf, _pf, _af, _zf, _sf, _tf, _if, _df, _of);
	printf("for more info see\n	http://stackoverflow.com/questions/9130349/how-many-registers-are-there-in-8086-8088\n");
}
