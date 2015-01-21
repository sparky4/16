#if !defined(__EMS_H)
#define __EMS_H

#define EMSPAGESIZE (4096)
#define EMS_ERROR (-1)
struct {
  unsigned long length;
  unsigned char source_type;
  unsigned int source_handle;
  unsigned int source_ofs; 
  unsigned int source_seg;
  unsigned char dest_type;
  unsigned int dest_handle;
  unsigned int dest_ofs; 
  unsigned int dest_seg;
} myEMSmove;


void conv2ems(int PAGE, int HANDLE, char *output) {
      char *x = (char *) &myEMSmove;
      myEMSmove.length = EMSPAGESIZE;
      myEMSmove.source_type = 0;
      myEMSmove.source_handle = 0;
      myEMSmove.source_ofs = FP_OFF(output);
      myEMSmove.source_seg = FP_SEG(output);
      myEMSmove.dest_type = 1;
      myEMSmove.dest_handle = HANDLE;
      myEMSmove.dest_ofs = (PAGE & 3) << 12;
      myEMSmove.dest_seg = PAGE >> 2;
      __asm push ds
      __asm mov       ax, 5700h;        
      __asm lds       si, x
      __asm int       67h 
      __asm pop ds
}

void ems2conv(int PAGE, int HANDLE, char *output) {
      char *x = (char *) &myEMSmove;
      myEMSmove.length = EMSPAGESIZE;
      myEMSmove.source_type = 1;
      myEMSmove.source_handle = HANDLE;
      myEMSmove.source_ofs = (PAGE & 3) << 12;
      myEMSmove.source_seg = PAGE>>2;
      myEMSmove.dest_type = 0;
      myEMSmove.dest_handle = 0;
      myEMSmove.dest_ofs = FP_OFF(output);
      myEMSmove.dest_seg = FP_SEG(output);
      __asm push ds
      __asm mov       ax, 5700h
      __asm lds       si, x
      __asm int       67h 
      __asm pop ds
}


void freeEMS(int handle) {
  __asm mov       dx, handle;
  __asm mov       ah, 45h;    
  __asm int       67h  
}

int checkEMS() {
  char *x = "EMMXXXX0";
  __asm {
    mov ax, 3567h
    int 21h
    mov di, 10
    mov si, offset x
    mov cx, 4
    rep cmpsw
    je good 
  }
  return -1;
  good:
  __asm {
    sub     ax, ax
    mov     ah, 40h
    int     67h
  }
  return _AX;
}

int getavailEMS() {
  __asm {
    mov     ah, 42h
    int     67h
    cmp     ah, 00h
    je      getavailEMS_Okay
    mov     bx, -1
  }
  getavailEMS_Okay:
  __asm shl bx, 2
  return _BX;
}

int allocEMS(int pages) {
  __asm {
    mov     bx, pages
    shr     bx, 2
    mov     ah, 43h
    int     67h
    cmp     ah, 0
    je      allocEMS_Okay
    mov     dx, -1
  }
  allocEMS_Okay:
  return _DX;
}

#endif
