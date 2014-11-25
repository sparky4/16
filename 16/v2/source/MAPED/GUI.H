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

#ifndef GUI_INC
#define GUI_INC

extern int amxofs, amyofs;
extern int mmxofs, mmyofs;
extern int cnxofs, cnyofs;
extern int lmxofs, lmyofs;
extern int lvxofs, lvyofs;
extern int vaxofs, vayofs;
extern int mpxofs, mpyofs;
extern int zexofs, zeyofs;
extern int exofs, eyofs;
extern int prxofs, pryofs;
extern int rsxofs, rsyofs;

extern void CheckMessageExpirations();
extern void Message(char *text, int duration);
extern void DrawScreenControls();
extern void RenderMessages();
extern void RenderGUI();
extern void DoMainMenu();
extern void MainMenu(int dx, int dy);
#define byte unsigned char
extern byte TileCmp(byte *one, byte *two);
extern void ImageToLayer();
extern void Destroyer();
extern void LayersSubMenu(int dx, int dy);
extern void ExecuteSubMenu(int dx, int dy);
extern void AddLayer();
extern void EditLayerProperties();
extern void NewMAP();
extern void About();
extern int  Confirm(char *message);
extern void LoadMAPDialog();
extern void LoadVSPDialog();
extern void MPDialog();
extern void ZoneEdDialog();
extern void EntityEditor(int i);
extern void CHRList();
extern void MoveScriptDialog();
extern void MapPropertiesDialog();
extern void ResizeMapDialog();
extern void CDDialog();
extern void NoticeDialog();
extern void OptionsDialog();
extern void VCNotify(char *msg);

#endif // GUI_INC
