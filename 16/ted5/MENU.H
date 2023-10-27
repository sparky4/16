/////////////////////////////////////////////////////////////////
//
// Pull-Down menu interface .H file
//
/////////////////////////////////////////////////////////////////
typedef struct { char *item_name;
		 void (*routine)(void);
		 char shiftflag;
		 char hotkey;
	       } MenuDef;

typedef struct { int num_items;
		 MenuDef *menu_def;
		 char *menu_name;
	       } MBarDef;

typedef struct { int menux;
		 int menunamelen;
		 int menuwidth;
	       } MInfoType;

typedef struct { char *text;
		 int xoff,yoff,border;
	       } btype;

typedef struct { char *text;
		 int width,height,numbuttons;
		 btype *buttons;
		 void (*hook)(int x,int y);
	       } DialogDef;

typedef struct { int x,y,w,h;
		 void _seg *buffer;
	       } BackSaveStr;

#define SCindex 0x3C4
#define SCmapmask 2
#define GCindex 0x3CE
#define GCreadmap 4
#define GCmode 5
#define crtcaddr 0x3d4

#define NUMFLASHES	10
#define ALT		0x38
#define CTRL		0x1d

#define MAXFDNAMES 	40
extern struct ffblk f;		// more info from GetPath


void extern InitDesktop(MBarDef *menubar,int initmouse);
void extern DeskEventLoop(void (*UserRoutine)(void),void (*ConstantRoutine)(void));
void extern ClearScreen(void);
void extern RedrawDesktop(void);
void extern SaveBackground(int x,int y,int w,int h);
void extern RestoreBackground(void);
int extern DoDialog(DialogDef *TheDialog);
void extern ErrDialog(char *string,char *bstring);
void extern DrawBorder(int x,int y,int w,int h,int b);
int extern CheckButtons(DialogDef *TheDialog);
int extern CheckButtonsRet(DialogDef *TheDialog);
void extern DrawDialog(DialogDef *TheDialog,int saveback);
void extern GetButtonXY(DialogDef *TheDialog,int button,unsigned *x,unsigned *y);
void extern GetDialogXY(DialogDef *TheDialog,unsigned *x,unsigned *y);
int extern CheckList(int x,int y,int w,int h,void (*oncode)(),void (*offcode)(),int blink);
int extern Message(char *mstring);

//
// RETURNS 0=OK,-1=NO FILES MATCH FILTER,-2=CANCELED
//
extern char NameList[MAXFDNAMES][13];
int GetPath(char *string,char *filter,char *path);
int GetList(char *string,int numnames);

static void FDon(int x,int y,int w);
static void FDoff(int x,int y,int w);
static void CancelOn(int x,int y);
static void CancelOff(int x,int y);
