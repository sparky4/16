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

#ifndef CONLIB_H
#define CONLIB_H

#define ETC 0
#define RENDER 1
#define PFLIP 2

extern byte cpu_watch, cpubyte;

void ListMounts();
void PackInfo();
void FileInfo();
void CD_Play();
void vid_mode();
void CPU_Usage();
void Mount();
void ZeroConWarp(char **args);
void ZeroConBrowseTiles(void);
void CameraTracking();
void ZeroGetRString();
void ZeroSetRString(char **args);
void Obstructions();
void MoveGranularity();
void Phantom();
void CurPos();
void PlayerSpeed();
void Player();
void SpawnEntity();
void ShowZones();
#endif
