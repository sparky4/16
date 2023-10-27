#define IDSTRING	"TED5v1.0"
#define TITLESTR	"TED5 vD.IP"
#define IDSTSTR		"         "TITLESTR

#include <stdio.h>
#include <process.h>
#include <io.h>
#include <dos.h>
#include <alloc.h>
#include <dir.h>
#include <mem.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>
#include <fcntl.h>
#include "lib.h"
#include "memmgr.h"
#include "menu.h"
#include "jhuff.h"
#include "xms.h"

#define	SGN(x)		(x>0?1:x<0?-1:0)
#define	OFF3(m,i)	(*(long huge *)((char huge *)m+(i)*3)&0xffffff)


//
// Defs for TED5
//

typedef enum {TILES,MASKED,ICONS} screentype;
typedef enum {DATA,CODE,FARDATA} segtype;	// FOR MAKEOBJ ONLY

#define SCindex		0x3C4
#define SCmapmask	2
#define GCindex		0x3CE
#define GCreadmap	4
#define GCmode		5
#define crtcaddr	0x3d4

//
// STRUCTURE OF TED5 TEMPFILE WHILE LAUNCHING
//
typedef struct {
		video lastmode;
		char ext[4];
	       } TempStruct;

//
// STRUCTURE OF THE "TEDINFO.EXT" FILE THAT TED5 CREATES
//
typedef struct {
		int level,lastvid,lastx,lasty,tsize;
		int OldCgaXMS,OldEgaXMS,OldVgaXMS;
		long OldCgaXMSsize,OldEgaXMSsize,OldVgaXMSsize;
		int CgaXMSlook,EgaXMSlook,VgaXMSlook;
		char permicon,pflags;
		int oscrx,oscry;
		char parmstring[64],launchname[14];
		char BackgndColor;
		char ImportPath[64];
	       } InfoStruct;

//
// STRUCTURE OF THE "GFXINFO?.EXT" FILE THAT IGRAB CREATES
//
typedef struct {
		 int num8,num8m,num16,num16m,num32,num32m;
		 int off8,off8m,off16,off16m,off32,off32m;
		 int numpics,numpicm,numsprites;
		 int offpic,offpicm,offsprites;
		 int offpicstr,offpicmstr,offsprstr;
		 int numexterns,offexterns;
	       } GfxStruct;

//
// TED5 LOADS & SAVES THIS HEADER FOR INTERNAL MAPFILE USAGE
//
typedef struct { unsigned maptype;	//bit 0=bkgnd/1=frgnd/2=info
		 unsigned tsize;	//1=8/2=16/3=32

		 unsigned numtplanes,oldtilenum;
		 long tileinfooff[10];
		 unsigned tileinfolen[10];
		 char tnames[10][8];

		 unsigned numtmplanes,oldtilemnum;
		 long tileinfomoff[10];
		 unsigned tileinfomlen[10];
		 char tmnames[10][8];

		 unsigned RLEWtag;
		 long dataoffsets[100];
		 long datalengths[100];

		 int NumIconRows;
	       } MapFileHeaderStr;

//
// TED5 SAVES THIS MAPFILE HEADER FOR THE GAME
//
typedef struct {
		 unsigned RLEWtag;
		 long dataoffsets[100];

	       } OutputHeadStr;

//
// EACH AND EVERY MAP HAS THIS HEADER (IF THE MAP EXISTS)
//
typedef struct { long mapbkgndpl;
		 long mapfrgndpl;
		 long mapinfopl;
		 unsigned mapbkgndlen;
		 unsigned mapfrgndlen;
		 unsigned mapinfolen;
		 unsigned width,height;
		 char name[16];
	       } MapHeaderStr;

//
// SPECIFY WHAT, WHEREFROM, AND HOW TO COPY A REGION
//
typedef struct {
		 char PlanesCopied;	// use BPLANE,FPLANE,IPLANE to mask
		 int MapOrTileSelect;	// 0:map,1:tileselect
		 int x,y,w,h;		// from map or tileselect
	       } CopyStr;

//
// UNDO REGION
//
typedef struct {
		int x,y,w,h;
	       } UndoStr;

//
// LAST-BUILT "VIEWMAP"
//
typedef struct {
		unsigned step,built_flag,EGAseg,maxpack;
	       } VMapStr;


//
// HEADER FOR APPLE-PREFERRED FILES
//
typedef struct { long length;
		 char Kind[5];
		 int MasterMode;
		 int PixelsPerLine;
		 int NumColorTables;
		 int ColorTable[16];
		 int NumScanLines;
	       } ApPrefStr;

#define CREATED		1
#define NOTCREATED 	2
#define ANYLIST		3

#define BPLANE		1
#define FPLANE		2
#define IPLANE		4

#define O_FGNDBACK	4
#define ICONBACK	3

#define TINFOWIDTH	3
#define TINFOHEIGHT	7
#define TINFOHEIGHTEGA2	25

extern MBarDef TED5MenuBar[];
extern DialogDef DoCreated;

extern UndoStr UndoRegion;
extern CopyStr TileCopy;
extern MapFileHeaderStr _seg *MapFileHeader;
extern char _seg *Tinfo[10],_seg *TMinfo[10],_seg *GraphHeader;
extern long _seg *XMSlookup,_seg *CgaXMSlookup,_seg *EgaXMSlookup,_seg *VgaXMSlookup;
extern int _seg *MapBkgnd,_seg *MapFrgnd,_seg *MapInfoPl,
    _seg *CutBkgnd,_seg *CutFrgnd,_seg *CutInfoPl;
extern MapHeaderStr MapHeader;

#if 0
extern char far TOM;	// JOKE SHIT!
#endif

extern TempStruct LaunchInfo;
extern InfoStruct _seg *TEDInfo;
extern GfxStruct _seg *GfxInfo;
extern video lastvideo,videomode;
extern screentype whichscreen;
extern VMapStr VMapData;

extern char launchname[64],ext[4],format[2],projname[64],mapname[64],planes,
	infoname[64],mapheadname[64],tdata,MapNames[100][16],parmstring[64];
extern char SM_name[64],SM_loadname[64],BkgndColor;

extern unsigned temp,whichmap,numtplanes,tilenum,tilemnum,numtmplanes,left,
	DirtyFlag,tilelen,tilemlen,whicht,whichtm,whichi,
	tsize,infoy,infomaxw,mapwidth,mapheight,screenw,usingbat,
	screenh,planeton,planemon,planeion,maxiconrows,lasticon,firsticon,
	viewton,viewmon,viewion,XMSundoB,XMSundoF,XMSundoI,XMSmaps,
	EgaXMS,CgaXMS,VgaXMS,xmshandle,GridMode,SnapMode,snapx,snapy,
	snapxsize,snapysize,writeH,F3_flag,NoAbout;
extern int tilebase,tilembase,infobaron,xbase,ybase,scrnbot,scrnrgt,
	FillMode,PasteMode,SelectMode,SelX1,SelY1,PasteOK,SelX2,SelY2,pixelx,pixely,
	selectcols,px,py,lastmap,TIybase,TIymbase,TIxbase,TIxmbase,BfillMode,
	Plotting,TsearchMode;
extern long CgaXMSsize,EgaXMSsize,VgaXMSsize;

extern void far *XMSdriver;

//
// FUNCTION PROTOTYPES
//

//
// LIB_A.ASM
//
extern void CGAcharout(int x,int y,char ch);
extern void EGAcharout(int x,int y,char ch,video vid);
extern void VGAcharout(int x,int y,char ch);

//
// TED5_A.ASM
//
extern unsigned EGA1lookup[200];
extern unsigned EGA2lookup[200];
extern void DrawTile(int x,int y,int tile);
extern void CopyCGA(int srcx,int srcy,int width,int height,int destx,int desty);
extern void CopyEGA(int srcx,int srcy,int width,int height,int destx,int desty);
extern void CopyVGA(int srcx,int srcy,int width,int height,int destx,int desty);
extern void Overlay(int tsize);

//
// TED5
//
void FindGraphFile(void);
void HandleEvent(void);
void Continuous(void);
void LoadInfoFile(void);
int  LoadGraphStuff(int rtn,video newvid);
void LoadMapHeader(void);
void ParseCmdline(void);
void InitTed5(void);
void CallDesktop(void);
void SelectTiles(int screen);
void STnot(int x,int y);
void DrawProjBord(int x,int y);
void DrawInfoBar(void);
void PrintCoords(void);
void FigureScreenEdges(void);
void errsound(void);
void DrawMap(void);
void CheckSelectEdges(int x,int y,int i,int j);
int  InputIconAmount(void);

//
// TED5-1
//
void Item_InputInfoplane(void);
void Item_SelectTile(void);
void Item_EditTinfoNames(void);
void Item_BlockFill(void);
void Item_TileSearch(void);
void Item_Launch(void);
void Item_Undo(void);
void Item_TINFOCopy(void);

void CreateMap(int exitok);
int  SelectMap(int exitok,int createflg,char *title);
int  PickMorePlanes(void);
void DoBlockFill(void);
void RemoveUndoBuffers(void);
void AllocateUndoBuffers(void);
void RestoreUndo(void);
void SaveUndo(int x,int y,int w,int h);
void CopyUndoRegion(void);
void SaveOutputHeader(void);
void SaveTEDInfo(void);
void BackupFile(char *filename);

void TInfoNon(int x,int y,int b);
void TInfoNoff(int x,int y,int b);
void TInfoMNon(int x,int y,int b);
void TInfoMNoff(int x,int y,int b);
void TIDoneOn(int x,int y);
void TIDoneOff(int x,int y);


//
// TED5-2
//
void Item_About(void);
void Item_ModeSwitch(void);
void Item_DeleteMap(void);
void Item_EditMap(void);
void Item_SaveMap(void);
void Item_CreateMap(void);
void Item_Quit(void);
void Item_MapStats(void);
void Item_ToggleInfo(void);
void Item_Amputate(void);
void Item_SwitchMap(void);
void Item_EditMapNames(void);
void Item_Copy(void);
void Item_Paste(void);
void Item_LastVideo(void);
void Item_FloodFill(void);
void Item_LastMap(void);
void Item_CountTiles(void);
int  CheckForMapSave(void);
void DoFloodFill(int x,int y,int whichb);

void DrawTileSelect(int deltarows,int *numrows,int *numcols);
void DrawUnused(int deltarow);
void DrawCurrentTiles(void);
void ZeroModes(void);

//
// TED5-3
//
void Item_EditMapEdges(void);
void Item_PrintMem(void);
void Item_Huffman(void);
void CheckInfoValues(int i,int j,int tilei);
void PrintMem(int x,int y);
void EraseFloatPaste(void);
void DrawFloatPaste(void);
void CopyScreen(int srcx,int srcy,int width,int height,int destx,int desty);
void Item_LAUNCHname(void);
void Item_PARMstring(void);
void Item_ChangeIconRows(void);
void Item_ChangeLaunchIcon(void);
void Item_ChangeBkgndColor(void);

//
// TED5-4
//
void SignalSound(void);
void Item_GraphicDump(void);
void Item_EditTinfoValues(void);
void Item_ProjectReSelect(void);
void Item_AddDelTinfo(void);
void Item_GridMode(void);
void Item_SnapTog(void);
void Item_ViewMap(void);
void Item_ReviewMap(void);
void Item_ImportMaps(void);
void Item_VisitDOS(void);
void Item_POtog(void);
void Do_ViewMap(int how);
int  MakeOBJ(char *filename,char *destfilename,char *public,segtype whichseg,char *farname);
void DrawTinfoScreen(int thescreen,int deltax,int deltay);
void EnterTinfoValue(int whichtinfo,int mx,int my,int H_or_V);
void UseTinfoValue(int whichtinfo,int mx,int my,int PickupOrDrop);
