/*
Copyright (C) 1998 BJ Eirich (aka vecna)
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public Lic
See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#ifndef TILESEL_INC
#define TILESEL_INC

extern int selofs;
extern char ttile[256], ttile2[256];
extern char curastd;
extern char curmode;

extern int curtileptr(int row);
extern void NormalButton(int x1, int y1, int x2, int y2, char *str);
extern void PressdButton(int x1, int y1, int x2, int y2, char *str);
extern void RenderSelector();
extern void FlipX(unsigned short edtile);
extern void FlipY(unsigned short edtile);
extern void FlipZ(unsigned short edtile);
extern void DrawHighlight(int x, int y);
extern void TileSelector();
extern void VSPAnimDialog();
extern void AddTilesDialog();
extern void RenderImport();
extern void ImportTiles();
extern void ImportVSP();
extern void OpenPCXas();

extern void ReadPCXLine(unsigned char *dest);
extern void LoadPCXHeader(char *fname);
extern void LoadPCXHeaderNP(char *fname);
extern int  GrabTileAt(int x, int y, int z, int check_dup);
extern void ImportPCX();

#endif // TILESEL_INC
