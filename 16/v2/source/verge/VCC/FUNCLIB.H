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

#if !defined(__FUNCLIB_INC)
#define __FUNCLIB_INC

extern void vcfunc_Exit();
extern void Message();
extern void vc_loadimage();
extern void vc_AllocateEntity();
extern void vc_Map();
extern void vc_LoadFont();
extern void vc_PlayFLI();
extern void vc_PrintString();
extern void vc_LoadRaw();
extern void vc_EntityMove();
extern void vc_strlen();
extern void vc_strcmp();
extern void vc_SetRString();
extern void vc_PartyMove();
extern void vc_PlayMusic();
extern void vc_OpenFile();
extern void vc_HookRetrace();
extern void vc_HookTimer();
extern void vc_HookKey();
extern void vc_QuickRead();
extern void vc_CacheSound();
extern void vc_val();
extern void vc_Log();
extern void vc_fgetline();
extern void vc_fgettoken();
extern void vc_fwritestring();
extern void vc_frename();
extern void vc_fdelete();
extern void vc_fwopen();
extern void vc_asc();
extern void vc_NumForScript();
extern void vc_FileSize();
extern void GenericFunc(unsigned char funcid, int numargs);

#endif // __FUNCLIB_INC
