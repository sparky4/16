' Using EMS in QuickBASIC: Part 1 of 3
' Source Code
'
' By Jon Petrosky (Plasma)
' www.phatcode.net
'
' [Remember to start QB with the /L switch to enable interrupts]

DEFINT A-Z
'$DYNAMIC
'$INCLUDE: 'QB.BI'

DIM SHARED Regs AS RegTypeX
DIM SHARED EMS.Error            'Holds the error code of the last operation

DECLARE FUNCTION EMS.ErrorMsg$ ()
DECLARE FUNCTION EMS.Init ()
DECLARE FUNCTION EMS.Version$ ()
DECLARE FUNCTION EMS.PageFrame ()
DECLARE FUNCTION EMS.FreeHandles ()
DECLARE FUNCTION EMS.FreePages ()
DECLARE FUNCTION EMS.TotalPages ()
DECLARE FUNCTION EMS.AllocPages (NumPages)
DECLARE SUB EMS.DeallocPages (Handle)
DECLARE SUB EMS.MapPage (Physical, Logical, Handle)
DECLARE SUB EMS.MapXPages (PhysicalStart, LogicalStart, NumPages, Handle)
DECLARE SUB EMS.CopyMem (Length&, SrcHandle, SrcSegment, SrcOffset, DstHandle, DstSegment, DstOffset)
DECLARE SUB EMS.ExchMem (Length&, SrcHandle, SrcSegment, SrcOffset, DstHandle, DstSegment, DstOffset)

CLS

IF NOT EMS.Init THEN
  PRINT "No EMM detected."
  END
END IF

COLOR 14, 1
PRINT SPACE$(22); "Using EMS in QuickBasic: Part 1 of 3"; SPACE$(22)
COLOR 15, 0
PRINT STRING$(31, 196); " EMS Information "; STRING$(32, 196)
COLOR 7
PRINT "EMM Version: "; EMS.Version$

IF EMS.Version$ < "4.0" THEN
  PRINT
  PRINT "EMM 4.0 or later must be present to use some of the EMS functions."
  END
END IF

PRINT "Page frame at: "; HEX$(EMS.PageFrame); "h"
PRINT "Free handles:"; EMS.FreeHandles

IF EMS.FreeHandles = 0 THEN
  PRINT
  PRINT "You need at least one free handle to run this demo."
  END
END IF

PRINT "Total EMS:"; EMS.TotalPages; "pages /"; EMS.TotalPages * 16&; "KB /"; EMS.TotalPages \ 64; "MB"
PRINT "Free EMS:"; EMS.FreePages; "pages /"; EMS.FreePages * 16&; "KB /"; EMS.FreePages \ 64; "MB"

IF EMS.FreePages < 64 THEN
  PRINT
  PRINT "You need at least 64 pages (1 MB) free EMS to run this demo."
  END
END IF

PRINT
COLOR 15, 0
PRINT STRING$(31, 196); " Allocation Test "; STRING$(32, 196)
COLOR 7
PRINT "Allocating 64 pages (1 MB) of EMS...";

Handle = EMS.AllocPages(64)
IF EMS.Error THEN
  PRINT "error!"
  PRINT EMS.ErrorMsg$
  END
ELSE
  PRINT "ok!"
END IF

PRINT "Pages allocated to handle"; Handle
PRINT
COLOR 15, 0
PRINT STRING$(30, 196); " Page Map/Copy Test "; STRING$(30, 196)
COLOR 7
PRINT "Mapping logical page 0 to physical page 0...";

EMS.MapPage 0, 0, Handle
IF EMS.Error THEN
  PRINT "error!"
  PRINT EMS.ErrorMsg$
  END
ELSE
  PRINT "ok!"
END IF

PRINT "Mapping logical pages 0-3 to physical pages 0-3...";

EMS.MapXPages 0, 0, 4, Handle
IF EMS.Error THEN
  PRINT "error!"
  PRINT EMS.ErrorMsg$
  END
ELSE
  PRINT "ok!"
END IF

PRINT "Copying logical pages 0-31 to logical pages 32-63...";

EMS.CopyMem 512288, Handle, 0, 0, Handle, 32, 0
IF EMS.Error THEN
  PRINT "error!"
  PRINT EMS.ErrorMsg$
  END
ELSE
  PRINT "ok!"
END IF

PRINT "Exchanging logical pages 0-31 with logical pages 32-63...";

EMS.ExchMem 512288, Handle, 0, 0, Handle, 32, 0
IF EMS.Error THEN
  PRINT "error!"
  PRINT EMS.ErrorMsg$
  END
ELSE
  PRINT "ok!"
END IF

PRINT
COLOR 15, 0
PRINT STRING$(22, 196); " 10-Second Speed Test (Please Wait) "; STRING$(22, 196)
COLOR 7

Mapped& = 0
StartTime! = TIMER
DO UNTIL StartTime! + 5 <= TIMER
  EMS.MapXPages 0, 0, 4, Handle
  Mapped& = Mapped& + 4
LOOP

Copied& = 0
StartTime! = TIMER
DO UNTIL StartTime! + 5 <= TIMER
  EMS.CopyMem 512288, Handle, 0, 0, Handle, 32, 0
  Copied& = Copied& + 1
LOOP

PRINT "Pages Mapped/Sec:"; Mapped& \ 5
PRINT "Bytes copied/Sec:"; (Copied& * 512288) \ 5
PRINT
COLOR 15, 0
PRINT STRING$(30, 196); " Deallocation Test "; STRING$(31, 196)
COLOR 7
PRINT "Deallocating 64 pages...";

EMS.DeallocPages (Handle)
IF EMS.Error THEN
  PRINT "error!"
  PRINT EMS.ErrorMsg$
  END
ELSE
  PRINT "ok!";
END IF

KeyPress$ = INPUT$(1)
CLS
END

FUNCTION EMS.AllocPages (NumPages)

  'Allocates the number of pages in [NumPages] and
  'returns the EMS handle the memory is allocated to.

  Regs.ax = &H4300                           'Allocate [NumPages] pages of EMS
  Regs.bx = NumPages
  InterruptX &H67, Regs, Regs
  EMS.Error = (Regs.ax AND &HFF00&) \ &H100  'Store the status code

  EMS.AllocPages = Regs.dx                   'Return the handle

END FUNCTION

SUB EMS.CopyMem (Length&, SrcHandle, SrcSegment, SrcOffset, DstHandle, DstSegment, DstOffset)

  'Copies memory from EMS or base memory to EMS or base memory, where:
  '
  'Length&    = Length of memory to copy in bytes
  'SrcHandle  = EMS handle of source memory (use 0 if source is base memory)
  'SrcSegment = Segment of source memory (or page number if source is EMS)
  'SrcOffset  = Offset of source memory
  'DstHandle  = EMS handle of destination memory (use 0 if destination is base memory)
  'DstSegment = Segment of destination memory (or page number if destination is EMS)
  'DstOffset  = Offset of destination memory

  'Determine the source and destination memory types by checking the handles
  IF SrcHandle = 0 THEN SrcType$ = CHR$(0) ELSE SrcType$ = CHR$(1)
  IF DstHandle = 0 THEN DstType$ = CHR$(0) ELSE DstType$ = CHR$(1)

  'Create a buffer containing the copy information
  CopyInfo$ = MKL$(Length&) + SrcType$ + MKI$(SrcHandle) + MKI$(SrcOffset) + MKI$(SrcSegment) + DstType$ + MKI$(DstHandle) + MKI$(DstOffset) + MKI$(DstSegment)

  Regs.ax = &H5700                           'Copy the memory region
  Regs.ds = VARSEG(CopyInfo$)                'described in the buffer
  Regs.si = SADD(CopyInfo$)
  InterruptX &H67, Regs, Regs
  EMS.Error = (Regs.ax AND &HFF00&) \ &H100  'Store the status code

END SUB

SUB EMS.DeallocPages (Handle)

  'Deallocates the EMS pages allocated the EMS handle [Handle].
  'You MUST remember to call the sub before your program ends
  'if you allocate any memory!

  Regs.ax = &H4500                           'Release the pages allocated to [Handle]
  Regs.dx = Handle
  InterruptX &H67, Regs, Regs
  EMS.Error = (Regs.ax AND &HFF00&) \ &H100  'Store the status code

END SUB

FUNCTION EMS.ErrorMsg$

  'Returns a text string describing the error code in EMS.Error.

  SELECT CASE EMS.Error
    CASE &H0: Msg$ = "successful"
    CASE &H80: Msg$ = "internal error"
    CASE &H81: Msg$ = "hardware malfunction"
    CASE &H82: Msg$ = "busy -- retry later"
    CASE &H83: Msg$ = "invalid handle"
    CASE &H84: Msg$ = "undefined function requested by application"
    CASE &H85: Msg$ = "no more handles available"
    CASE &H86: Msg$ = "error in save or restore of mapping context"
    CASE &H87: Msg$ = "insufficient memory pages in system"
    CASE &H88: Msg$ = "insufficient memory pages available"
    CASE &H89: Msg$ = "zero pages requested"
    CASE &H8A: Msg$ = "invalid logical page number encountered"
    CASE &H8B: Msg$ = "invalid physical page number encountered"
    CASE &H8C: Msg$ = "page-mapping hardware state save area is full"
    CASE &H8D: Msg$ = "save of mapping context failed"
    CASE &H8E: Msg$ = "restore of mapping context failed"
    CASE &H8F: Msg$ = "undefined subfunction"
    CASE &H90: Msg$ = "undefined attribute type"
    CASE &H91: Msg$ = "feature not supported"
    CASE &H92: Msg$ = "successful, but a portion of the source region has been overwritten"
    CASE &H93: Msg$ = "length of source or destination region exceeds length of region allocated to either source or destination handle"
    CASE &H94: Msg$ = "conventional and expanded memory regions overlap"
    CASE &H95: Msg$ = "offset within logical page exceeds size of logical page"
    CASE &H96: Msg$ = "region length exceeds 1 MB"
    CASE &H97: Msg$ = "source and destination EMS regions have same handle and overlap"
    CASE &H98: Msg$ = "memory source or destination type undefined"
    CASE &H9A: Msg$ = "specified alternate map register or DMA register set not supported"
    CASE &H9B: Msg$ = "all alternate map register or DMA register sets currently allocated"
    CASE &H9C: Msg$ = "alternate map register or DMA register sets not supported"
    CASE &H9D: Msg$ = "undefined or unallocated alternate map register or DMA register set"
    CASE &H9E: Msg$ = "dedicated DMA channels not supported"
    CASE &H9F: Msg$ = "specified dedicated DMA channel not supported"
    CASE &HA0: Msg$ = "no such handle name"
    CASE &HA1: Msg$ = "a handle found had no name, or duplicate handle name"
    CASE &HA2: Msg$ = "attempted to wrap around 1M conventional address space"
    CASE &HA3: Msg$ = "source array corrupted"
    CASE &HA4: Msg$ = "operating system denied access"
    CASE ELSE: Msg$ = HEX$(EMS.Error) '"undefined error"
  END SELECT

  EMS.ErrorMsg$ = Msg$

END FUNCTION

SUB EMS.ExchMem (Length&, SrcHandle, SrcSegment, SrcOffset, DstHandle, DstSegment, DstOffset)

  'Exhanges memory from EMS or base memory to EMS or base memory, where:
  '
  'Length&    = Length of memory to exchange in bytes
  'SrcHandle  = EMS handle of source memory (use 0 if source is base memory)
  'SrcSegment = Segment of source memory (or page number if source is EMS)
  'SrcOffset  = Offset of source memory
  'DstHandle  = EMS handle of destination memory (use 0 if destination is base memory)
  'DstSegment = Segment of destination memory (or page number if destination is EMS)
  'DstOffset  = Offset of destination memory

  'Determine the source and destination memory types by checking the handles
  IF SrcHandle = 0 THEN SrcType$ = CHR$(0) ELSE SrcType$ = CHR$(1)
  IF DstHandle = 0 THEN DstType$ = CHR$(0) ELSE DstType$ = CHR$(1)

  'Create a buffer containing the copy information
  ExchInfo$ = MKL$(Length&) + SrcType$ + MKI$(SrcHandle) + MKI$(SrcOffset) + MKI$(SrcSegment) + DstType$ + MKI$(DstHandle) + MKI$(DstOffset) + MKI$(DstSegment)

  Regs.ax = &H5701                           'Exchange the memory region
  Regs.ds = VARSEG(ExchInfo$)                'described in the buffer
  Regs.si = SADD(ExchInfo$)
  InterruptX &H67, Regs, Regs
  EMS.Error = (Regs.ax AND &HFF00&) \ &H100  'Store the status code

END SUB

FUNCTION EMS.FreeHandles

  'Returns the number of free (available) EMS handles.

  Regs.ax = &H4B00                             'Get the # of handles in use
  InterruptX &H67, Regs, Regs
  UsedHandles = Regs.bx

  Regs.ax = &H5402                             'Get the total # of handles
  InterruptX &H67, Regs, Regs
  EMS.Error = (Regs.ax AND &HFF00&) \ &H100    'Store the status code
  TotalHandles = Regs.bx

  EMS.FreeHandles = TotalHandles - UsedHandles 'Subtract to get the # of free handles

END FUNCTION

FUNCTION EMS.FreePages

  'Returns the number of free (available) EMS pages
  '(Multiply by 16 to get the amount free EMS in KB.)

  Regs.ax = &H4200                           'Get the # of free pages
  InterruptX &H67, Regs, Regs
  EMS.Error = (Regs.ax AND &HFF00&) \ &H100  'Store the status code
  EMS.FreePages = Regs.bx

END FUNCTION

FUNCTION EMS.Init

  'Returns true (-1) if an EMM is installed
  'or false (0) if an EMM is not installed.

  Regs.ax = &H3567                        'Get the interrupt vector for int 67h
  InterruptX &H21, Regs, Regs
  DEF SEG = Regs.es                       'Point to the interrupt segment
  FOR x = 10 TO 17                        'Store the 8 bytes at ES:0A in EMM$
    EMM$ = EMM$ + CHR$(PEEK(x))
  NEXT
  IF EMM$ <> "EMMXXXX0" THEN
    EMS.Init = 0              'EMM not installed
  ELSE
    EMS.Init = -1             'EMM installed
  END IF

END FUNCTION

SUB EMS.MapPage (Physical, Logical, Handle)

  'Maps the logical EMS page [Logical] (allocated to the handle [Handle])
  'to the physical page [Physical] in the EMS page frame.

  Regs.ax = &H4400 + Physical                'Map the logical page [Logical]
  Regs.bx = Logical                          'to the physical page [Physical]
  Regs.dx = Handle
  InterruptX &H67, Regs, Regs
  EMS.Error = (Regs.ax AND &HFF00&) \ &H100  'Store the status code

END SUB

SUB EMS.MapXPages (PhysicalStart, LogicalStart, NumPages, Handle)

  'Maps up to 4 logical EMS pages to physical pages in the page frame, where:
  '
  'PhysicalStart = Physical page first logical page is mapped to
  'LogicalStart  = First logical page to map
  'NumPages      = Number of pages to map (1 to 4)
  'Handle        = EMS handle logical pages are allocated to

  'Create a buffer containing the page information
  FOR x = 0 TO NumPages - 1
    MapInfo$ = MapInfo$ + MKI$(LogicalStart + x) + MKI$(PhysicalStart + x)
  NEXT

  Regs.ax = &H5000                           'Map the pages in the buffer
  Regs.cx = NumPages                         'to the pageframe
  Regs.dx = Handle
  Regs.ds = VARSEG(MapInfo$)
  Regs.si = SADD(MapInfo$)
  InterruptX &H67, Regs, Regs
  EMS.Error = (Regs.ax AND &HFF00&) \ &H100  'Store the status code

END SUB

FUNCTION EMS.PageFrame

  'Returns the segment of the EMS page frame

  Regs.ax = &H4100                           'Get the segment of the page frame
  InterruptX &H67, Regs, Regs
  EMS.Error = (Regs.ax AND &HFF00&) \ &H100  'Save the status code
  EMS.PageFrame = Regs.bx

END FUNCTION

FUNCTION EMS.TotalPages

  'Returns the total number of EMS pages
  '(Multiply by 16 to get the total amount of EMS in KB.)

  Regs.ax = &H4200                           'Get the # of total pages
  InterruptX &H67, Regs, Regs
  EMS.Error = (Regs.ax AND &HFF00&) \ &H100  'Store the status code
  EMS.TotalPages = Regs.dx

END FUNCTION

FUNCTION EMS.Version$

  'Returns a string containing the EMM version.
  '(Must be "4.0" or greater to use our routines.)

  Regs.ax = &H4600                           'Get the EMM version
  InterruptX &H67, Regs, Regs
  EMS.Error = (Regs.ax AND &HFF00&) \ &H100  'Save the status code

  Version = Regs.ax AND &HFF                 'Split the version number into
  Major = (Version AND &HF0) \ &H10          'its major and minor counterparts
  Minor = Version AND &HF
  EMS.Version$ = LTRIM$(STR$(Major)) + "." + LTRIM$(STR$(Minor))

END FUNCTION

