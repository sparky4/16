Open Watcom Linker Version 2.0 beta Jun 27 2016 12:31:53 (64-bit)
Copyright (c) 2002-2016 The Open Watcom Contributors. All Rights Reserved.
Portions Copyright (c) 1985-2002 Sybase, Inc. All Rights Reserved.
Created on:       16/06/30 12:04:48
Executable Image: exmmtest.exe
creating a DOS executable


                        +------------+
                        |   Groups   |
                        +------------+

Group                           Address              Size
=====                           =======              ====

DGROUP                          1266:0000            00009400



                        +--------------+
                        |   Segments   |
                        +--------------+

Segment                Class          Group          Address         Size
=======                =====          =====          =======         ====

exmmtest_TEXT          CODE           AUTO           0000:0000       00000408
16_in_TEXT             CODE           AUTO           0000:0410       00002555
16_mm_TEXT             CODE           AUTO           0000:2970       00003320
wcpu_TEXT              CODE           AUTO           0000:5c90       00000058
16_head_TEXT           CODE           AUTO           0000:5cf0       0000046e
16_ca_TEXT             CODE           AUTO           0000:6160       0000041f
16_dbg_TEXT            CODE           AUTO           0000:6580       00000243
kitten_TEXT            CODE           AUTO           0000:67d0       0000080a
16_hc_TEXT             CODE           AUTO           0000:6fe0       000015ad
16_timer_TEXT          CODE           AUTO           0000:8590       000003e4
_TEXT                  CODE           AUTO           0898:0000       000091a7
8250_TEXT              CODE           AUTO           0898:91b0       0000009a
8250bios_TEXT          CODE           AUTO           0898:9250       00000086
8250siop_TEXT          CODE           AUTO           0898:92e0       00000000
8250prob_TEXT          CODE           AUTO           0898:92e0       000002d5
8250cint_TEXT          CODE           AUTO           0898:95c0       00000045
8250fifo_TEXT          CODE           AUTO           0898:9610       0000008a
8250bauc_TEXT          CODE           AUTO           0898:96a0       00000041
8250baud_TEXT          CODE           AUTO           0898:96f0       00000040
doswin_TEXT            CODE           AUTO           0898:9730       00000231
dos_TEXT               CODE           AUTO           0898:9970       000001bd
dosntvig_TEXT          CODE           AUTO           0898:9b30       00000000
16_in13_DATA           FAR_DATA       AUTO           124b:0000       000001a4
FAR_DATA               FAR_DATA       AUTO           1265:0004       00000000
_NULL                  BEGDATA        DGROUP         1266:0000       00000020
_AFTERNULL             BEGDATA        DGROUP         1268:0000       00000002
CONST                  DATA           DGROUP         1268:0002       00000102
CONST2                 DATA           DGROUP         1278:0004       0000013a
_DATA                  DATA           DGROUP         128c:0000       00000640
XIB                    DATA           DGROUP         12f0:0000       00000000
XI                     DATA           DGROUP         12f0:0000       0000003c
XIE                    DATA           DGROUP         12f3:000c       00000000
YIB                    DATA           DGROUP         12f3:000c       00000000
YI                     DATA           DGROUP         12f3:000c       00000018
YIE                    DATA           DGROUP         12f5:0004       00000000
STRINGS                DATA           DGROUP         12f5:0004       00000000
DATA                   DATA           DGROUP         12f5:0004       00000000
_emu_init_start        EMU            DGROUP         12f5:0004       00000000
_emu_init_end          EMU            DGROUP         12f5:0004       00000000
_BSS                   BSS            DGROUP         12f6:0000       00000afc
STACK                  STACK          DGROUP         13a6:0000       00008000


                        +----------------+
                        |   Memory Map   |
                        +----------------+

* = unreferenced symbol
+ = symbol only referenced locally

Address        Symbol
=======        ======

Module: exmmtest.o(/dos/z/16/src/exmmtest.c)
1266:0900+     _gvar
0000:0186      main_
Module: 16_in.o(/dos/z/16/src/lib/16_in.c)
1266:0274+     _testkeyin
1266:0276+     _testcontrolnoisy
1266:0278+     _testctrltype
0000:05bc+     INL_KeyService_
0000:0858+     Mouse_
0000:08b0+     IN_GetJoyAbs_
0000:0c5a+     IN_GetJoyButtonsDB_
0000:0ee0+     IN_SetupJoy_
0000:10a2*     IN_Startup_
0000:11a8*     IN_Default_
0000:12c4*     IN_Shutdown_
0000:1344*     IN_SetKeyHook_
0000:1366+     IN_ClearKeysDown_
0000:1446*     IN_ReadCursor_
0000:155e*     IN_ReadControl_
0000:2336*     IN_SetControlType_
0000:2384*     IN_GetScanName_
0000:240c*     IN_WaitForKey_
0000:2442*     IN_WaitForASCII_
0000:2478+     IN_AckBack_
0000:2560*     IN_Ack_
0000:2602+     IN_IsUserInput_
0000:26a8*     IN_UserInput_
0000:26ec*     IN_KeyDown_
0000:2734+     IN_ClearKey_
0000:27ac*     IN_qb_
0000:2806*     IN_initplayer_
1266:0b0e+     _inpu
Module: 16_mm.o(/dos/z/16/src/lib/16_mm.c)
0000:3376+     MML_CheckForEMS_
0000:33d2+     MML_SetupEMS_
0000:358a+     MML_ShutdownEMS_
0000:35f6+     MM_MapEMS_
0000:36f2*     MM_MapXEMS_
0000:3812+     MML_CheckForXMS_
0000:386e*     MML_SetupXMS_
0000:3956+     MML_ShutdownXMS_
0000:39e4+     MML_UseSpace_
0000:3bc2+     MML_ClearBlock_
0000:3c46      MM_Startup_
0000:4078      MM_Shutdown_
0000:410e      MM_GetPtr_
0000:44fe      MM_FreePtr_
0000:45f6*     MM_SetPurge_
0000:46d0*     MM_SetLock_
0000:47a8+     MM_SortMem_
0000:4996      MM_ShowMemory_
0000:5036      MM_DumpData_
0000:5190+     MM_UnusedMemory_
0000:5214+     MM_TotalFree_
0000:52aa*     MM_Report_
0000:5c62*     MM_BombOnError_
1266:0ce0+     _beforesort
1266:0ce4+     _aftersort
1266:0ce8+     _XMSaddr
Module: wcpu.o(/dos/z/16/src/lib/wcpu/wcpu.c)
0000:5c90      detectcpu_
Module: 16_head.o(/dos/z/16/src/lib/16_head.c)
0000:5d20*     wait_
0000:5d6c*     filesize_
0000:5dd2      printmeminfoline_
0000:5fbe      US_CheckParm_
0000:6122      dirchar_
Module: 16_ca.o(/dos/z/16/src/lib/16_ca.c)
0000:61b0      CA_OpenDebug_
0000:6200      CA_CloseDebug_
0000:622e+     CA_FarRead_
0000:62ae*     CA_FarWrite_
0000:632e*     CA_ReadFile_
0000:63aa      CA_LoadFile_
0000:6452*     CAL_OptimizeNodes_
0000:6502      CA_Startup_
0000:6552      CA_Shutdown_
1266:0df0*     _finishcachebox
1266:0df4*     _drawcachebox
1266:0df8*     _updatecachebox
Module: 16_dbg.o(/dos/z/16/src/lib/16_dbg.c)
0000:65d2*     _DEBUG_INIT_
0000:66fe+     _DEBUG_
0000:6776*     _DEBUGF_
1266:0574+     __DEBUG_INITed
1266:0578+     __DEBUG_uart
Module: kitten.o(/dos/z/16/src/lib/nyan/kitten.c)
0000:680c      KITTENGETS
0000:686c*     kittenopen_
0000:6aba+     catread_
0000:6b6e*     kittenclose_
0000:6b8a+     mystrtoul_
0000:6c3e+     processEscChars_
0000:6dc4+     get_line_
0000:6e36+     db_fetch_
0000:6ecc+     db_insert_
1266:057c+     __kitten_catalog
Module: 16_hc.o(/dos/z/16/src/lib/16_hc.c)
0000:71ae+     LargestFreeBlock_
0000:723c+     _coreleft_
0000:72b8+     LargestFarFreeBlock_
0000:736e+     _farcoreleft_
0000:741c+     LargestHugeFreeBlock_
0000:750c*     _hugecoreleft_
0000:75dc      GetFreeSize_
0000:7636      GetFarFreeSize_
0000:7680      GetNearFreeSize_
0000:76ce      heapdump_
0000:8370+     heapstat_
0000:84ae+     heapstat0_
0000:8510+     HC_OpenDebug_
0000:8560*     HC_CloseDebug_
Module: 16_timer.o(/dos/z/16/src/lib/16_timer.c)
0000:85c0*     start_timer_
0000:86fa*     elapsed_timer_
0000:874c+     ticktock_
0000:87b8*     time_in_seconds_
0000:8804*     shinkutxt_
0000:894c*     WaitPee_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(stk086.asm)
0898:0005      __STK
0898:0025      __STKOVERFLOW_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(fmalloc.c)
0898:0042      _fmalloc_
0898:0042      malloc_
1266:0596      ___fheap
1266:0598      ___fheapRover
1266:059a      ___LargestSizeB4Rover
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(pia.asm)
0898:01bf      __PIA
0898:01b8*     __PIS
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(printf.c)
0898:01d6      printf_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(bmsize.c)
0898:0205      _bmsize_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(getch.c)
0898:023c      getch_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(ffree.c)
0898:0256      _ffree_
0898:0256      free_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(cstart)
1266:0000*     __nullarea
1266:059c*     __ovlflag
1266:059d*     __intno
1266:059e*     __ovlvec
0898:02c4      _cstart_
0898:0397*     _Not_Enough_Memory_
0898:04c9      __exit_
0898:04e6      __do_exit_with_msg__
0898:053f      __GETDS
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(cmodel.asm)
0898:054a      _big_code_
0898:054a*     CodeModelMismatch
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(argcv.c)
1266:1084      __argv
1266:1088      ___argv
1266:108c      __argc
1266:108e      ___argc
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(intx086.c)
0898:054a+     int86x_
0898:06cd      int86_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(i4m.asm)
0898:06fa      __I4M
0898:06fa      __U4M
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(d_getvec.c)
0898:0712      _dos_getvect_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(d_setvec.c)
0898:071d      _dos_setvect_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(i4d.asm)
0898:072a      __I4D
0898:0779      __U4D
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(heapgrow.c)
0898:07fd*     _heapgrow_
0898:07fd      _fheapgrow_
0898:07fe      _nheapgrow_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(memavl.c)
0898:0876      _memavl_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(nmalloc.c)
0898:08bb      _nmalloc_
1266:05a2      ___nheapbeg
1266:05a4      ___MiniHeapRover
1266:05a6      ___LargestSizeB4MiniHeapRover
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(nfree.c)
0898:0995      _nfree_
1266:1090+     ___MiniHeapFreeRover
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(exit.c)
0898:0a90+     __null_int23_exit_
0898:0a91      exit_
0898:0aa9*     _exit_
1266:05a8+     ___int23_exit
1266:05ac      ___FPE_handler_exit
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(ptc.asm)
0898:0ac4      __PTC
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(ltoa.c)
0898:0af7      ultoa_
0898:0bb5*     ltoa_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(write.c)
0898:0d3b      write_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(fopen.c)
0898:0f67+     __open_flags_
0898:1228+     _fsopen_
0898:128e      fopen_
0898:13b1*     freopen_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(fprintf.c)
0898:1439      fprintf_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(fclose.c)
0898:1468      __doclose_
0898:15b2      __shutdown_stream_
0898:15cc      fclose_
1266:1092+     ___RmTmpFileFn
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(clockdos.c)
0898:16ad      clock_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(kbhit.c)
0898:16d4      kbhit_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(ftell.c)
0898:16eb      ftell_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(fseek.c)
0898:18d1      fseek_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(istable.c)
1266:0124      __IsTable
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(tolower.c)
0898:1ac9      tolower_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(unlnk.c)
0898:1ad7      unlink_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(open.c)
0898:1d07      open_
0898:1d3e      sopen_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(close.c)
0898:1d6b      close_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(filelen.c)
0898:1d70      filelength_
Module: src/lib/doslib/hw/8250/dos86h/8250.lib(/dos/z/16/src/lib/doslib/hw/8250/8250.c)
0898:91b0      already_got_8250_port_
0898:9204      init_8250_
1266:05b8+     _inited_8250
1266:109a      _base_8250_port
1266:10aa      _info_8250_port
1266:10ca      _base_8250_ports
Module: src/lib/doslib/hw/8250/dos86h/8250.lib(/dos/z/16/src/lib/doslib/hw/8250/8250bios.c)
0898:9250      probe_8250_bios_ports_
0898:928a      get_8250_bios_port_
1266:05ba      _bios_8250_ports
Module: src/lib/doslib/hw/8250/dos86h/8250.lib(/dos/z/16/src/lib/doslib/hw/8250/8250siop.c)
1266:0226      _standard_8250_ports
Module: src/lib/doslib/hw/8250/dos86h/8250.lib(/dos/z/16/src/lib/doslib/hw/8250/8250prob.c)
0898:92e0      probe_8250_
Module: src/lib/doslib/hw/8250/dos86h/8250.lib(/dos/z/16/src/lib/doslib/hw/8250/8250cint.c)
0898:95c0      uart_8250_enable_interrupt_
Module: src/lib/doslib/hw/8250/dos86h/8250.lib(/dos/z/16/src/lib/doslib/hw/8250/8250fifo.c)
0898:9610*     uart_8250_disable_FIFO_
0898:9650      uart_8250_set_FIFO_
Module: src/lib/doslib/hw/8250/dos86h/8250.lib(/dos/z/16/src/lib/doslib/hw/8250/8250bauc.c)
0898:96a0      uart_8250_baud_to_divisor_
0898:96bc*     uart_8250_divisor_to_baud_
Module: src/lib/doslib/hw/8250/dos86h/8250.lib(/dos/z/16/src/lib/doslib/hw/8250/8250baud.c)
0898:96f0      uart_8250_set_baudrate_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(snprintf.c)
0898:1e3c      vsnprintf_
0898:1ed1*     snprintf_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(sprintf.c)
0898:1f00      sprintf_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(getenv.c)
0898:1f2f      getenv_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(pts.asm)
0898:201a      __PTS
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(read.c)
0898:204b      read_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(strdup.c)
0898:2184*     __clib_strdup_
0898:2184      strdup_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(haloc.c)
0898:21eb      halloc_
0898:22d0      hfree_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(fheapwal.c)
0898:22d7      _heapwalk_
0898:22d7      _fheapwalk_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(nheapwal.c)
0898:22ec+     __NHeapWalk_
0898:23e5      _nheapwalk_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(stack086.c)
0898:23f5      stackavail_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(iob.c)
1266:05bc      ___iob
1266:10cc      ___OpenStreams
1266:10d0      ___ClosedStreams
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(fltused.c)
1266:0724      _fltused_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(_8087086.asm)
1266:0726      __8087
1266:0727      __real87
1266:0728      __dos87emucall
1266:072a      __dos87real
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(xmsg.c)
0898:2404*     __exit_with_msg_
0898:2409      __fatal_runtime_error_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(crwdata)
1266:072c      __curbrk
1266:0734      __STACKLOW
1266:0736      __STACKTOP
1266:0738      __cbyte
1266:073a      __child
1266:073c      __no87
1266:0749      ___FPE_handler
1266:072e      __psp
1266:073d      __get_ovl_stack
1266:0741      __restore_ovl_stack
1266:0745      __close_ovl_file
1266:074d      __LpCmdLine
1266:0751      __LpPgmName
1266:0730      __osmajor
1266:0731      __osminor
1266:0732      __osmode
1266:0733      __HShift
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(mem.c)
0898:2430      __MemAllocator
0898:24fa      __MemFree
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(growseg.c)
0898:2657      __GrowSeg_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(allocseg.c)
0898:27cd      __AllocSeg_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(fmemneed.c)
0898:28bc      __fmemneed_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(fprtf.c)
0898:28db      __fprtf_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(cmain086.c)
0898:2a2e      __CMain
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(initrtns.c)
0898:2a8f      __InitRtns
0898:2a8f*     __FInitRtns
0898:2aea      __FiniRtns
0898:2aea*     __FFiniRtns
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(uselfn.c)
1266:0756      ___uselfn
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(dosseg)
0898:2b4e      __DOSseg__
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(initargv.c)
0898:2e7d      __Init_Argv_
0898:2edf      __Fini_Argv_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(intr.c)
0898:2f09      intr_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(segread.c)
0898:2f1d      segread_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(grownear.c)
0898:3008      __LastFree_
0898:3082      __ExpandDGROUP_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(nmemneed.c)
0898:3193      __nmemneed_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(alphabet.c)
1266:0022      ___Alphabet
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(__lseek.c)
0898:3196      __lseek_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(dosret.c)
0898:31d0*     __dosretax
0898:31d5*     __dosret0
0898:31dc      __set_errno_dos_
0898:3244*     __set_errno_dos_reterr_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(errno.c)
0898:324f*     __get_errno_ptr_
1266:10d8      _errno
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(iomode.c)
0898:3256      __GetIOMode_
0898:328b      __SetIOMode_nogrow_
1266:076c      ___NFiles
1266:076e      ___init_mode
1266:0796      ___io_mode
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(comtflag.c)
0898:32c0*     __set_commode_
1266:079a      __commode
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(textmode.c)
1266:079c      __fmode
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(chktty.c)
0898:32cc      __chktty_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(freefp.c)
0898:332b      __freefp_
0898:33cc      __purgefp_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(allocfp.c)
0898:340d      __allocfp_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(_clse.c)
0898:35bd      __close_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(flush.c)
0898:35f3      __flush_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(fflush.c)
0898:383c      fflush_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(tell.c)
0898:3851      tell_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(lseek.c)
0898:3862      lseek_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(error086.asm)
0898:3894      __doserror_
0898:38a2      __doserror1_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(opendos.c)
0898:38ae      _dos_open_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(isatt.c)
0898:38d4      isatty_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(doserrno.c)
0898:38f0*     __get_doserrno_ptr_
1266:10da      __doserrno
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(umaskval.c)
1266:079e      ___umaskval
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(creatdos.c)
0898:38f7      _dos_creat_
0898:391b*     _dos_creatnew_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(stiomode.c)
0898:393c+     __grow_iomode_
0898:3a23+     __shrink_iomode_
0898:3a64      __SetIOMode_
Module: src/lib/doslib/hw/dos/dos86h/dos.lib(/dos/z/16/src/lib/doslib/hw/dos/doswin.c)
0898:9730*     detect_windows_
1266:07a0+     _windows_version_method
1266:07a4+     _windows_version
1266:07a6      _windows_mode
1266:07a7+     _windows_init
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(prtf.c)
0898:3b0b      __prtf_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(vsprintf.c)
0898:4c78      vsprintf_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(environ.c)
1266:10de      ___env_mask
1266:10e2      _environ
1266:10e6*     __wenviron
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(mbterm.c)
0898:4cba      _mbterm_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(mbsnextc.c)
0898:4d0d      _mbsnextc_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(mbcupper.c)
0898:4d68      _mbctoupper_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(mbsinc.c)
0898:4d8b      _mbsinc_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(iomodtty.c)
0898:4ddd      __ChkTTYIOMode_
0898:4e4b*     __IOMode_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(heapwalk.c)
0898:4f1d      __HeapWalk_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(initfile.c)
0898:5143      __InitFiles_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(ioexit.c)
0898:5374*     fcloseall_
0898:537b      __full_io_exit_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(setefg.c)
0898:5385      __setEFGfmt_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(chk8087.c)
0898:53bd*     _fpreset_
0898:53e8      __init_8087_
0898:5467      __chk8087_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(enterdb.c)
0898:54d7      __EnterWVIDEO_
1266:07c6+     ___WD_Present
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(heapen.c)
0898:54fb*     _heapenable_
1266:07c8      ___heap_enabled
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(amblksiz.c)
1266:07ca      __amblksiz
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(fputc.c)
0898:550c      fputc_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(ioalloc.c)
0898:56e1      __ioalloc_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(___argc.c)
1266:10ea      ____Argv
1266:10ee      ____Argc
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(sgdef086)
1266:08a0      __Start_XI
1266:08dc      __End_XI
1266:08dc      __Start_YI
1266:08f4      __End_YI
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(histsplt.c)
1266:10f0      ___historical_splitparms
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(dointr)
0898:5870      _DoINTR_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(sbrk.c)
0898:5be9      __brk_
0898:5c51*     sbrk_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(qwrit.c)
0898:5c61      __qwrite_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(fsync.c)
0898:5cc1      fsync_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(flushall.c)
0898:5cc6+     __flushall_
0898:5d5a      flushall_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(frealloc.c)
0898:5d60*     _frealloc_
0898:5d60      realloc_
Module: src/lib/doslib/hw/dos/dos86h/dos.lib(/dos/z/16/src/lib/doslib/hw/dos/dos.c)
0898:9970      probe_dos_
1266:07d0*     _lib_dos_option
1266:07d1+     _dos_flavor
1266:07d2      _dos_version
1266:07d4+     _freedos_kernel_version
1266:07d8+     _dos_version_method
Module: src/lib/doslib/hw/dos/dos86h/dos.lib(/dos/z/16/src/lib/doslib/hw/dos/dosntvig.c)
1266:07e0      _detect_windows_ntdvm_dosntast_init_CB
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(wctomb.c)
0898:5e3c      wctomb_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(itoa.c)
0898:5e87+     utoa_
0898:5f28      itoa_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(strupr.c)
0898:5f72      strupr_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(noefgfmt.c)
1266:07e4      ___EFG_printf
1266:07e8      ___EFG_scanf
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(mbisdbcs.c)
1266:07ec      ___IsDBCS
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(mbislead.c)
0898:5fbe*     _ismbblead_
1266:10f2      ___MBCSIsTable
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(lltoa.c)
0898:5fda      ulltoa_
0898:6117*     lltoa_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(setenvp.c)
0898:61a6      __setenvp_
0898:6332      __freeenvp_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(mbinit.c)
0898:63ce      __mbinit_
1266:07ee      ___MBCodePage
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(mbdtoupp.c)
0898:64a9      _mbdtoupper_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(toupper.c)
0898:64b7      toupper_
Module: /dos/fdos/watcom2/lib286/math87h.lib(efgfmt.c)
0898:64c5      _EFG_Format_
Module: /dos/fdos/watcom2/lib286/math87h.lib(cnvs2d.c)
0898:6697      __cnvs2d_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(8087cw.c)
1266:07f0      __8087cw
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(save8087.c)
1266:07f2      ___Save8087
1266:07f6      ___Rest8087
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(grabfp87.c)
0898:66b7      __GrabFP87_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(ini87086.asm)
0898:66ea*     __init_8087_emu
0898:66f0      __x87id
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(filedos.c)
0898:674d*     _dos_close_
0898:675b      _dos_commit_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(fmsize.c)
0898:676a*     _msize_
0898:6785      _fmsize_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(nexpand.c)
0898:6796      _nexpand_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(fexpand.c)
0898:67ea      _fexpand_
0898:67ea*     _expand_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(clearenv.c)
0898:681f      clearenv_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(rtcntrl.c)
0898:6950*     _SetLD64bit_
0898:695b*     _SetLD80bit_
0898:6966      _LDisDouble_
Module: /dos/fdos/watcom2/lib286/math87h.lib(ldcvt.c)
0898:6acb      _LDScale10x_
0898:73a3+     __cvtld_
0898:7b7f      __LDcvt_
Module: /dos/fdos/watcom2/lib286/noemu87.lib(emustub.asm)
0000:0000*     FJSRQQ
0000:0000*     FISRQQ
0000:0000*     FIERQQ
0000:0000      FIDRQQ
0000:0000      FIWRQQ
0000:0000*     FJCRQQ
0000:0000*     FJARQQ
0000:0000*     FICRQQ
0000:0000*     FIARQQ
0898:7b90*     __init_87_emulator
Module: /dos/fdos/watcom2/lib286/math87h.lib(strtod.c)
0898:83a4+     __Strtold_
0898:85e3      strtod_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(fpe87.asm)
0898:878a      __Init_FPE_handler_
0898:87c3      __Fini_FPE_handler_
0898:87fa*     __FPEHandler
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(_expand.c)
0898:8a8c      __HeapManager_expand_
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(bexpand.c)
0898:8f95      _bexpand_
Module: /dos/fdos/watcom2/lib286/math87h.lib(ldclass.c)
0898:8fee      __LDClass_
0898:9074*     _FLClass_
Module: /dos/fdos/watcom2/lib286/math87h.lib(bufld086.asm)
0898:9088      __ZBuf2LD
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(seterrno.c)
0898:9150*     __set_EDOM_
0898:915c      __set_ERANGE_
0898:9168*     __set_EINVAL_
Module: /dos/fdos/watcom2/lib286/math87h.lib(inf_nan.c)
1266:022e*     ___f_infinity
1266:0232*     ___f_posqnan
1266:0236      ___d_infinity
1266:023e      ___d_posqnan
1266:0246*     ___ld_infinity
1266:024e*     ___ld_posqnan
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(hugeval.c)
1266:0256      __HugeValue
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(fpeint.c)
1266:089e      ___FPE_int
Module: /dos/fdos/watcom2/lib286/dos/clibh.lib(bfree.c)
0898:9182      _bfree_


                        +--------------------+
                        |   Libraries Used   |
                        +--------------------+

src/lib/doslib/hw/dos/dos86h/dos.lib
src/lib/doslib/hw/8250/dos86h/8250.lib
/dos/fdos/watcom2/lib286/noemu87.lib
/dos/fdos/watcom2/lib286/math87h.lib
/dos/fdos/watcom2/lib286/dos/clibh.lib


                        +-----------------------+
                        |   Linker Statistics   |
                        +-----------------------+

Stack size:  8000 (32768.)
Memory size:  0001ba60 (113248.)
Entry point address: 0898:02c4
Link time: 00:00.36
