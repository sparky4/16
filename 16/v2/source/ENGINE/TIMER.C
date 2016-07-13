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

#define TIMER_H
#include <go32.h>
#include <dpmi.h>
#include <crt0.h>
#include "verge.h"

#define PIT0 0x40
#define PIT1 0x41
#define PIT2 0x42
#define PITMODE 0x43
#define PITCONST 1193180L

#define OCR1    0x20
#define IMR1    0x21

#define OCR2    0xA0
#define IMR2    0xA1

// ================================= Data ====================================

int _crt0_startup_flags = _CRT0_FLAG_NEARPTR;
typedef __dpmi_paddr *PVI;
static PVI oldhandler;

unsigned int systemtime=0, timer_count=0;
int (*callback) (void);

// ================================= Code ====================================

PVI DJSetHandlerFunc(unsigned char irqno, void (*handler)(), int len)
{
  PVI oldvect = (PVI) malloc(sizeof(__dpmi_paddr));
  int vecno=(irqno>7) ? irqno+0x68 : irqno+0x8;
  _go32_dpmi_seginfo wrapper;
  __dpmi_paddr new;

  wrapper.pm_offset = (long int) handler;
  wrapper.pm_selector = _my_cs();
  _go32_dpmi_allocate_iret_wrapper(&wrapper);
  new.offset32 = wrapper.pm_offset;
  new.selector = wrapper.pm_selector;
 __dpmi_get_and_disable_virtual_interrupt_state();
 if (len) _go32_dpmi_lock_code(handler,len);
 _go32_dpmi_lock_data(&wrapper,sizeof(_go32_dpmi_seginfo));
 __dpmi_get_protected_mode_interrupt_vector(vecno,oldvect);
 __dpmi_set_protected_mode_interrupt_vector(vecno,&new);
 __dpmi_get_and_enable_virtual_interrupt_state();
 return oldvect;
}

void DJSetHandlerAddr(unsigned char irqno, PVI handler)
{
  int vecno=(irqno>7) ? irqno+0x68 : irqno+0x8;
  _go32_dpmi_seginfo wrapper;
  __dpmi_paddr oldhandler;

  __dpmi_get_and_disable_virtual_interrupt_state();
  __dpmi_get_protected_mode_interrupt_vector(vecno, &oldhandler);
  wrapper.pm_offset = oldhandler.offset32;
  wrapper.pm_selector = oldhandler.selector;
  _go32_dpmi_free_iret_wrapper(&wrapper);
  __dpmi_set_protected_mode_interrupt_vector(vecno,handler);
  __dpmi_get_and_enable_virtual_interrupt_state();
  free(handler);
}

static void SendEOI (unsigned char irqno)
{
  unsigned char ocr=(irqno>7) ? OCR2 : OCR1;
  unsigned char eoi=0x60|(irqno&7);

  outportb(ocr,eoi);
  if (irqno>7) outportb(OCR1,0x20);
}

static void newhandler(void)
{
  systemtime++;
  timer_count++;
  if (cpu_watch) CPUTick();
  if (callback) callback();
  CheckTileAnimation();
  HookTimer();
  MD_Update();
  SendEOI(0);
}

static void EndNewHandler() { }

void sethz(unsigned int hz)
{ unsigned int pit0_set, pit0_value;

  disable();

  outportb(PITMODE, 0x34);
  pit0_value=PITCONST / hz;
  pit0_set=(pit0_value & 0x00ff);
  outportb(PIT0, pit0_set);
  pit0_set=(pit0_value >> 8);
  outportb(PIT0, pit0_set);

  enable();
}

void restorehz()
{
  disable();
  outportb(PITMODE, 0x34);
  outportb(PIT0, 0x00);
  outportb(PIT0, 0x00);
  enable();
}

void InitTimer()
{
   oldhandler = DJSetHandlerFunc(0, (void *) newhandler,
                ((int) EndNewHandler) - ((int) newhandler));
   sethz(100);
}

void ShutdownTimer()
{
  DJSetHandlerAddr(0, oldhandler);
  restorehz();
}
