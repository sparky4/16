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

#ifndef COLUMNS_INC
#define COLUMNS_INC

extern void BlitPlayField();
extern void BlitPlayFieldwDestruct(char flash);
extern void BlitPiece();
extern void GenerateNewPiece();
extern void DestroyBlock(int x, int y);
extern void ClearConsecutiveColumns();
extern void NextTurn();
extern int CanGoLeft();
extern int CanGoRight();
extern void RotatePiece();
extern void ProcessLevels();
extern void LoadBackground();
extern void Columns();

#endif // COLUMNS_INC
