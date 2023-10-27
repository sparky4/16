typedef enum {CGA,EGA1,EGA2,VGA,TEXT} video;
#define ESCOUT 0xdeaf

void Quit(char *string);
void WaitVBL(int times);
int  input(char *string,int max);
void SetupKBD ();
void ShutdownKBD ();
void clearkeys (void);
int  MouseInit(void);
void MouseHide(void);
void MouseShow(void);
int  MouseButton(void);
void MouseCoords(int *x,int *y);
void printhex(unsigned value);
void printbin(unsigned value);
unsigned inputint(int numchars);
int  input(char *string,int max);
void print (const char *str);
void fprint (const char huge *str);
void printhexb(unsigned char value);
void printint(unsigned value);
void bar (int xl, int yl, int xh, int yh, int ch);
int  get(void);
void drawwindow (int xl, int yl, int xh, int yh);
void erasewindow (void);
void centerwindow (int width, int height);
void expwin (int width, int height);
void expwinh (int width, int height);
void expwinv (int width, int height);
void SaveFile(char *filename,char huge *buffer, long size,long offset);
unsigned long LoadFile(char *filename,char huge *buffer,long offset,long size);
void LoadIn(char *filename,void _seg **baseptr);

void MouseOrigin(int x,int y);
void MouseLimits(int xmin,int xmax,int ymin,int ymax);

extern int win_xl,win_yl,win_xh,win_yh,screencenterx,screencentery;
extern unsigned char keydown[256];
extern unsigned sx,sy,xormask,MouseStatus,leftedge;

long filelen(char *filename);