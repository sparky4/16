cd doslib/hw/cpu
wlink option quiet system win386  library ../../hw/dos/win386/dos.lib library ../../hw/cpu/win386/cpu.lib library ../../windows/ntvdm/win386/ntvdmlib.lib library ../../hw/dos/win386/dos.lib library ../../hw/cpu/win386/cpu.lib file win386/test.obj option map=win386/test.map name win386/test.exe
wlink option quiet system win386  library ../../hw/dos/win386/dos.lib library ../../hw/cpu/win386/cpu.lib library ../../windows/ntvdm/win386/ntvdmlib.lib library ../../hw/dos/win386/dos.lib library ../../hw/cpu/win386/cpu.lib file win386/grind.obj option map=win386/grind.map
