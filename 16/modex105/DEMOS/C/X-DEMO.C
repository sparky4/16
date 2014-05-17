/* X-DEMO.C - a Mode "X" Demo      */
/* By Matt Pritchard, 14 Apr, 1993 */

#include <stdlib.h>
#include <stdio.h>

#include "modex.h"
#include "c_utils.h"

#define MAX_SHAPES  32
#define MAX_SPRITES 64

	/* routines in this file */

void demo_res (int, int, int);
int  get_key (void);
void error_out (char*);
void load_shapes (void);
int  int_sqrt (int, int);
void page_demo (void);

	/* Structures for Sprites */

struct Shape
{
	unsigned char Image[512];
	int           X_Width;
	int			  Y_Width;
}   Img [MAX_SHAPES];

struct Sprite
{
	int	X_pos;
	int	Y_pos;
	int X_Dir;
	int Y_Dir;
	int	Shape;
	int	Last_X [2];
	int Last_Y [2];
}   Obj [MAX_SPRITES];


	/* MAIN */


int main(int argc, char *argv[])
{

   /*	if (argc > 0)
	{
		while (argc > 0)
		{
			dos_print ("Unknown Argument: ");
			dos_print (makefp argv[argc]);
			argc--;
		}
		return (0);

	}
	 */

	init_random ();

	load_shapes ();

	demo_res ( Mode_320x200, 320, 200 );
	demo_res ( Mode_320x400, 320, 400 );

	demo_res ( Mode_360x200, 360, 200 );
	demo_res ( Mode_360x400, 360, 400 );

	demo_res ( Mode_320x240, 320, 240 );
	demo_res ( Mode_320x480, 320, 480 );

	demo_res ( Mode_360x240, 360, 240 );
	demo_res ( Mode_360x480, 360, 480 );

	page_demo ();

	set_video_mode (3);
	dos_print ("This Mode X Demo is Finished");
	return (0);

}


	/* Demonstrate a given resolution */


void demo_res (int Screen_Mode, int X_max, int Y_max)
{

char 	*Error1 = "Failure while calling SET_MODEX";
char	*Error2 = "Failure during READ_PIXEL test";

char	*Abort_Msg = "Demo aborted by User";

char	*Demo_Msg = " This is a MODE X demo ";
char	*Scrn_Msg = "Screen Resolution is     by    ";
char	*Cont_Msg = "Press <ANY KEY> to Continue";

char    *Line_Msg = "LINE TEST";
char	*Fill_Msg = "FILL TEST";
char	*Pixel_Msg = "PIXEL TEST";

char	Text[10];

int		x1, y1, x2, y2 = 0;
int		x, y, z = 0;
int		X_Center, gap = 0;


	if (set_modex (Screen_Mode) == 0)
	{
		error_out (Error1);
	}

	X_Center = X_max / 2;

	x1 = 10;
	y1 = 10;
	x2 = X_max - 1;
	y2 = Y_max - 1;

	for (z = 0; z <= 3; z++)
	{
		y = 31 - z -z;
		draw_line (x1+z, y1+z, x2-z, y1+z, y);
		draw_line (x1+z, y1+z, x1+z, y2-z, y);
		draw_line (x1+z, y2-z, x2-z, y2-z, y);
		draw_line (x2-z, y1+z, x2-z, y2-z, y);
	}

	for (x = 0; x < (X_max / 10); x++)
	{
		tgprintc (48 + ((x+1) % 10), x*10+1, 1, 9 + ((x/8) % 7) );
		draw_line (x*10+9, 0, x*10+9, 3, c_bWHITE);
	}

	for (y = 0; y < (Y_max / 10); y++)
	{
		tgprintc (48 + ((y+1) % 10), 1, y*10+1, 9 + ((y/10) % 7) );
		draw_line (0, y*10+9, 3, y*10+9, c_bWHITE);
	}

	for (x = 0; x <= 63; x++)
	{
		z = 15 + (x * 3 / 4);
		set_dac_register (64+x, z, z, z);
		set_dac_register (128+x, 0, z, z);

		draw_line (103-x, 60, 40+x, 123, 64+x);
		draw_line (40, 60+x, 103, 123-x, 128+x);

	}

	tprint_str (Line_Msg, 9, 37, 130, c_BLUE);

	y = 60;
	gap = 0;
	for (x = 0; x <= 9; x++)
	{
		fill_block (120, y, 120+x, y+gap, 64+x);
		fill_block (140 - (15-x), y, 150+x, y+gap, 230+x);
		fill_block (170 - (15-x), y, 170, y+gap, 128+x);
		y = y + gap + 2;
		gap++;
	}

	tprint_str (Fill_Msg, 9, 110, 46, c_GREEN);

	for (x = 190; x <= 250; x+=2)
	{
		for (y = 60; y <= 122; y+=2)
		{
			z = (x+x+y+y) & 0xff;
			set_point (x, y, z);
		}
	}

	tprint_str (Pixel_Msg, 10, 182, 130, c_RED);

	for (x = 190; x <= 250; x+=2)
	{
		for (y = 60; y <= 122; y+=2)
		{
			z = (x+x+y+y) & 0xff;
			if (read_point(x, y) != z)
			{
				error_out (Error2);
			}
		}
	}

	print_str (Demo_Msg, 23, X_Center - 92, 20, c_bRED, c_BLUE);

	x = X_Center - 124;
	print_str (Scrn_Msg, 28, x, 30, c_bGREEN, c_BLACK);

	sprintf (Text, "%3d", X_max);
	print_str (Text, 3, x+168, 30, c_bPURPLE, c_BLACK);

	sprintf (Text, "%3d", Y_max);
	print_str (Text, 3, x + 224, 30, c_bWHITE, c_BLACK);

	for (x = 0; x <= 15; x++)
	{
		set_dac_register (230+x, 63-x*4, 0, 15+x*3);
		draw_line (30+x, Y_max-6-x, X_max-20-x, Y_max-6-x, 230+x);
	}

	tprint_str (Cont_Msg, 27, X_Center - 103, Y_max-18, c_YELLOW);

	if (get_key () == Ky_ESC)
	{
		error_out (Abort_Msg);
	}

	return ;

}


	/* Wait for a Keystroke */


int get_key(void)
{

int	c = 0;

	while (c == 0)
	{
		c = scan_keyboard ();
	}

	return (c);

}


	/* Error Handling Routine */


void error_out (char * text)
{

	set_video_mode (3);
	dos_print (text);
	exit (EXIT_SUCCESS);

}


	/* Routine to generate random sprites */


void load_shapes ()
{

unsigned char 	Grid[33][33];

char	*Error1 = "Bad Shape Selected Error";

int		Shape;
int		x, y, z;
int		Style, Color;
int		X_Width, Y_Width, Center, S_Width;
int		Hollow_X, Hollow_Y;

	for (Shape = 0; Shape < MAX_SHAPES; Shape++)
	{
		for (y = 0; y <= 32; y++)
		{
			for (x = 0; x <= 32; x++)
			{
				Grid[x][y] = c_BLACK;
			}
		}

		Style = random_int (6);
		Color = 1 + random_int (15);

		switch (Style)

		{
				/* SOLID BOXES */

			case 0:

			{
				do
				{
					X_Width = 3 + random_int(30);
					Y_Width = 3 + random_int(30);

				} while ( (X_Width * Y_Width) >= 512);

				for (x = 1; x <= X_Width; x++)
				{
					for (y = 1; y <= Y_Width; y++)
					{
					   Grid[x][y] = Color;
					}
				}

				break;

			}
				/* HOLLOW BOXES */

			case 1:

			{
				do {
					X_Width = 6 + random_int(27);
					Y_Width = 6 + random_int(27);
				} while ( (X_Width * Y_Width) >= 512);

				for (y = 1; y <= Y_Width; y++)
				{
					for (x = 1; x <= X_Width; x++)
					{
						Grid[x][y] = Color;
					}
				}

				Hollow_X = 1 + random_int ((X_Width / 2) -1);
				Hollow_Y = 1 + random_int ((Y_Width / 2) -1);

				for (y = Hollow_Y+1; y <= Y_Width-Hollow_Y; y++)
				{
					for (x = Hollow_X+1; x <= X_Width-Hollow_X; x++)
					{
						Grid[x][y] = c_BLACK;
					}
				}

				break;

			}

				/* SOLID DIAMOND */

			case 2:

			{

				X_Width = 3 + 2 * random_int(10);
				Y_Width = X_Width;
				Center = X_Width / 2;

				for (y = 0; y <= Center; y++)
				{
					for (x = 0; x <= y; x++)
					{
						Grid [Center-x+1][y+1] = Color;
						Grid [Center+x+1][y+1] = Color;
						Grid [Center-x+1][Y_Width-y] = Color;
						Grid [Center+x+1][Y_Width-y] = Color;
					}
				}

				break;

			}

				/* HOLLOW DIAMOND */

			case 3:

			{

				X_Width = 3 + 2 * random_int(10);
				Y_Width = X_Width;
				Center = X_Width / 2;
				S_Width = random_int (Center);

				for (y = 0; y <= Center; y++)
				{
					for (x = 0; x <= y; x++)
					{
						if ( x+(Center-y) >= S_Width )
						{
							Grid [Center-x+1][y+1] = Color;
							Grid [Center+x+1][y+1] = Color;
							Grid [Center-x+1][Y_Width-y] = Color;
							Grid [Center+x+1][Y_Width-y] = Color;
						}
					}
				}

				break;

			}

				/* BALL */

			case 4:

			{

				X_Width = 7 + 2 * random_int (8);
				Y_Width = X_Width;
				Center  = 1 + X_Width / 2;

				for (y = 1; y <= Y_Width; y++)
				{
					for (x = 1; x <= X_Width; x++)
					{
						z = int_sqrt(Center-x, Center-y);
						if (z < Center)
						{
							Grid[x][y] = 150 + Color * 2 + z * 3;
						}
					}
				}

				break;
			}

				/* HOLLOW BALLS */

			case 5:

			{
				X_Width = 7 + 2 * random_int (8);
				Y_Width = X_Width;
				Center  = 1 + X_Width / 2;
				S_Width = random_int (X_Width);

				for (y = 1; y <= Y_Width; y++)
				{
					for (x = 1; x <= X_Width; x++)
					{
						z = int_sqrt(Center-x, Center-y);
						if ( (z < Center) && (z >= S_Width) )
						{
							Grid[x][y] = 150 + Color * 2 + z * 3;
						}
					}
				}


				break;
			}

			default:

			{
				error_out (Error1);
				break;

			}

		}

		z = 0;
		for (y = 1; y <= Y_Width; y++)
		{
			for (x = 1; x <= X_Width; x++)
			{
				Img[Shape].Image[z] = Grid[x][y];
				z++;
			}
		}

		Img[Shape].X_Width = X_Width;
		Img[Shape].Y_Width = Y_Width;

	}

	return;
}


	/* Quickie Psuedo Integer Square Root Routine */


int int_sqrt ( int x, int y )
{

int	Sqr_Table[12] = {1, 4, 9, 6, 25, 36, 49, 64, 81, 100, 121, 144};

int	r, d;

	d = (x * x) + (y * y);
	r = 0;

	while ( d >= Sqr_Table[r] )
	{
		r++;
	}

	return (r);

}


	/* The Bit Sprite Demo */


void page_demo ()
{

char	*Error1 = "Failure during SET_VGA_MODEX (0, 360, 240, 3) call";

int		Last_Objects[2], Visible_Objects;

int		Screen_X = 360;
int		Screen_Y = 240;

int		x, y, z;
int		c, dc;
int		x1, y1, x2, y2;

int		Sprite_X, Sprite_Y;
int		Current_Page;
int		New_X, New_Y;

int		View_X,	View_Y,	View_Max, View_Cnt, View_XD, View_YD;
int		Set_Color, Prev_Color, S_Dir, P_Dir;

int		Demo_Running = True;
int		redo, code;

	if (set_vga_modex(Mode_320x200, Screen_X, Screen_Y, 3) == 0)
	{
		error_out (Error1);
	}

	set_active_page (0);
	clear_vga_screen (c_BLACK);

   	print_str ("This is a Test of the Following Functions:", 99, 10, 9, c_bWHITE, c_BLACK);

	draw_line (10, 18, 350, 18, c_YELLOW);
	print_str ("SET_ACTIVE_PAGE", 99, 10, 20, c_bBLUE, c_BLACK);
	print_str ("SET_DISPLAY_PAGE", 99, 10, 30, c_GREEN, c_BLACK);
	print_str ("SET_DAC_REGISTER", 99, 10, 40, c_RED, c_BLACK);
	print_str ("CLEAR_VGA_SCREEN", 99, 10, 50, c_CYAN, c_BLACK);

	print_str ("TDRAW_BITMAP", 99, 10, 60, c_PURPLE, c_BLACK);
	print_str ("COPY_PAGE", 99, 10, 70, c_GREEN, c_BLACK);
	print_str ("COPY_BITMAP", 99, 10, 80, c_CYAN, c_BLACK);

	print_str ("GPRINTC", 99, 10, 90, c_BLUE, c_BLACK);
	print_str ("TGPRINTC", 99, 10, 100, c_GREEN, c_BLACK);
	print_str ("SET_WINDOW", 99, 10, 110, c_RED, c_BLACK);

	print_str ("VIRTUAL SCREEN SIZES", 20, 190, 20, c_bBLUE, c_BLACK);
	print_str ("    SMOOTH SCROLLING", 20, 190, 30, c_GREEN, c_BLACK);
	print_str ("    SPRITE ANIMATION", 20, 190, 40, c_CYAN, c_BLACK);
	print_str ("       PAGE FLIPPING", 20, 190, 50, c_RED, c_BLACK);
	print_str ("       COLOR CYCLING", 20, 190, 60, c_PURPLE, c_BLACK);

	for (x = 0; x <=60; x++)
	{
		set_dac_register (50 + x, 3 + x, 0, 60 - x);
		set_dac_register (150 + x, 3 + x, 0, 60 - x);
	}

	c = 0;
	dc = 1;
	for (x = 0; x <= (Screen_X / 2); x++)
	{
		draw_line (Screen_X / 2 - 1, Screen_Y / 4, x, Screen_Y - 1, c + 50);
		draw_line (Screen_X / 2, Screen_Y / 4, Screen_X - x - 1, Screen_Y - 1, c + 50);
		c+= dc;
		if ((c == 0) || (c == 60) ) { dc = -dc;}
	}

	tprint_str ("Press <ANY KEY> to Continue", 99, 72, 190, c_bWHITE);
	tprint_str ("< > = Faster   < > = Slower", 99, 72, 204, c_bGREEN);
	tprint_str ("< > = Fewer Shapes  < > = More Shapes", 99, 32, 218, c_bCYAN);

	tgprintc (43, 80, 204, c_YELLOW);
	tgprintc (45, 200, 204, c_YELLOW);

	tgprintc (25, 40, 218, c_YELLOW);
	tgprintc (24, 200, 218, c_YELLOW);

	copy_page (0, 1);
	copy_page (0, 2);

	for (x = 0; x < MAX_SPRITES; x++)
	{
		do {
			Obj[x].X_Dir = random_int(7) - 3;
			Obj[x].Y_Dir = random_int(7) - 3;
		} while ( (Obj[x].X_Dir == 0) && (Obj[x].Y_Dir == 0) );

		Obj[x].Shape = x % MAX_SHAPES;

		Sprite_X = Img[Obj[x].Shape].X_Width;
		Sprite_Y = Img[Obj[x].Shape].Y_Width;

		Obj[x].X_pos = 1 + random_int(Screen_X - Sprite_X - 2);
		Obj[x].Y_pos = 1 + random_int(Screen_Y - Sprite_Y - 2);

		Obj[x].Last_X[0] = Obj[x].X_pos;
		Obj[x].Last_X[1] = Obj[x].X_pos;
		Obj[x].Last_Y[0] = Obj[x].Y_pos;
		Obj[x].Last_Y[1] = Obj[x].Y_pos;

	}

	Current_Page = 0;

	View_X = 0;
	View_Y = 0;
	View_Max = 3;
	View_Cnt = 0;
	View_XD = 1;
	View_YD = 1;

	Set_Color = 3;
	S_Dir = 1;
	Prev_Color = 0;
	P_Dir = 1;

	Visible_Objects = MAX_SPRITES / 2;
	Last_Objects[0] = 0;
	Last_Objects[1] = 0;

	while (Demo_Running)
	{

		set_active_page (Current_Page);

			/* Erase Old Images */

		for (x = 0; x <= Last_Objects[Current_Page]; x++)
		{
			z = 2;
			y = Obj[x].Shape;
			x1 = Obj[x].Last_X[Current_Page];
			y1 = Obj[x].Last_Y[Current_Page];
			x2 = x1 + Img[y].X_Width -1;
			y2 = y1 + Img[y].Y_Width -1;

			x1 = x1 & 0xfffc;
			x2 = x2 | 0x0003;

			copy_bitmap (z, x1, y1, x2, y2, Current_Page, x1, y1);
		}

			/* Draw new images */

		for (x = 0; x <= Visible_Objects; x++)
		{
			Sprite_X = Img[Obj[x].Shape].X_Width;
			Sprite_Y = Img[Obj[x].Shape].Y_Width;

			/*  Move Sprite */

			do
			{
				redo = False;
				New_X = Obj[x].X_pos + Obj[x].X_Dir;

				if (( New_X < 0 ) || (New_X + Sprite_X > Screen_X) )
				{
					Obj[x].X_Dir = -Obj[x].X_Dir;
					if (random_int(20) == 1)
					{
						do
						{
							Obj[x].X_Dir = random_int(7) - 3;
							Obj[x].Y_Dir = random_int(7) - 3;
						} while ( (Obj[x].X_Dir == 0) && (Obj[x].Y_Dir == 0) );
						redo = True;
					}
				}
			} while (redo);
			Obj[x].X_pos = Obj[x].X_pos + Obj[x].X_Dir;


			do
			{
				redo = False;
				New_Y = Obj[x].Y_pos + Obj[x].Y_Dir;

				if ( (New_Y < 0) || (New_Y + Sprite_Y > Screen_Y) )
				{
					Obj[x].Y_Dir = -Obj[x].Y_Dir;
					if (random_int(20) == 1)
					{
						do
						{
							Obj[x].X_Dir = random_int(7) - 3;
							Obj[x].Y_Dir = random_int(7) - 3;
						} while ( (Obj[x].X_Dir == 0) && (Obj[x].Y_Dir == 0) );
						redo = True;
					}
				}
			} while (redo);

			Obj[x].Y_pos = Obj[x].Y_pos + Obj[x].Y_Dir;

			/* Draw Sprite */

			tdraw_bitmap ((char far*) &Img[Obj[x].Shape], Obj[x].X_pos, Obj[x].Y_pos, Sprite_X, Sprite_Y);

			Obj[x].Last_X[Current_Page] = Obj[x].X_pos;
			Obj[x].Last_Y[Current_Page] = Obj[x].Y_pos;

		}

		Last_Objects[Current_Page] = Visible_Objects;


		/* Pan Screen Back & Forth */

		View_Cnt++;
		if (View_Cnt >= View_Max)
		{
			View_X+= View_XD;
			if ( (View_X == 0) || (View_X == 39) ) {View_XD = -View_XD;}
			if (View_XD < 0)
			{
				View_Y+= View_YD;
				if ( (View_Y == 0) || (View_Y == 39) ) {View_YD = -View_YD;}
			}

			set_window (Current_Page, View_X, View_Y);

			View_Cnt = 0;
		}
		else
		{
			set_display_page (Current_Page);
		}

		/* Cycle Colors */

		set_dac_register (50 + Prev_Color, 3 + Prev_Color, 0, 60 - Prev_Color);
		set_dac_register (50 + Set_Color, Set_Color, 10, 63 - Set_Color);

		set_dac_register (150 + Prev_Color, 3 + Prev_Color, 0, 60 - Prev_Color);
		set_dac_register (150 + Set_Color, 63, 63, Set_Color);

		Set_Color+= S_Dir;
		if ( (Set_Color == 60) || (Set_Color == 0) ) {S_Dir = -S_Dir;}

		Prev_Color+= P_Dir;
		if ( (Prev_Color == 60) || (Prev_Color == 0) ) {P_Dir = -P_Dir;}

		/* Check for Keystroke */

		Current_Page = Current_Page ^ 0x01;

		code = scan_keyboard ();

		if (code == Ky_ESC) {Demo_Running = False;}

		if (code == Ky_Plus)
		{
			if (View_Max < 12) {View_Max++;}
		}

		if (code == Ky_Minus)
		{
			if (View_Max > 1) {View_Max--;}
			if (View_Cnt >= View_Max) {View_Cnt = 0;}
		}

		if (code == Ky_Up)
		{
			if (Visible_Objects < MAX_SPRITES-1) {Visible_Objects++;}
		}

		if (code == Ky_Down)
		{
			if (Visible_Objects > 0) {Visible_Objects--;}
		}

	}

}
