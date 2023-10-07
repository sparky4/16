Reconstructed Commander Keen 4-6 Source Code
Copyright (C) 2021 K1n9_Duk3
===============================================================================

This is an UNOFFICIAL reconstruction of the original Keen 4-6 source code. More
specifically, it is a reconstruction of the version 1.4 (and 1.5) EGA and CGA
releases.

The code is primarily based on the Catacomb 3-D source code (ID Engine files).
The text view code (CK_TEXT.C) is based on Wolfenstein 3-D, and the main game
and actor code is loosely based on Keen Dreams.

Catacomb 3-D Source Code
Copyright (C) 1993-2014 Flat Rock Software

Wolfenstein 3-D Source Code
Copyright (C) 1992 id Software

Keen Dreams Source Code
Copyright (C) 2014 Javier M. Chavez

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


Getting this code to compile
============================

You will need Borland C++ 2.0, 3.0 or 3.1 to compile this code. Newer versions
may work, but have not been tested.

In addition to the compiler and this source code, you will also need to extract
some data files from the original executables. I have provided patch scripts to
extract that data from the original executables. To use these patch scripts,
you will need one of the following tools:

CKPatch v0.11.3 (unofficial) - 16 bit DOS application
http://ny.duke4.net/files.html

K1n9_Duk3's Patching Utility v2.3 - Win32 application
http://k1n9duk3.shikadi.net/patcher.html

Copy the patching programs into the "static" subdirectory, along with copies of
the original Keen 4-6 executables. The Keen 4-6 executables should be named
KEEN4*.EXE, KEEN5*.EXE and KEEN6*.EXE respectively, otherwise the patching
programs might have trouble recognizing the files.

If you are going to use CKPatch, you should only copy one KEEN4*.EXE file, one
KEEN5*.EXE and one KEEN6*.EXE file into the "static" directory. CKPatch will
always use the first file matching this pattern, so if you have both KEEN4C.EXE
and KEEN4E.EXE in that directory, for example, CK4PATCH will always operate on
KEEN4C.EXE and ignore KEEN4E.EXE.

These patches will only work on very specific versions of the original game's
executables (v1.4/v1.5 CGA/EGA). The GT versions are not supported by CKPatch,
so you will need K1n9_Duk3's Patching Utility to extract their data with the
patch scripts included in the "static" directory. CKPatch may also be unable to
open the FormGen release of Keen 4 (and Keen 5, if it exists). Decompressing
the game's executable with UNLZEXE may fix that problem.

If you are using K1n9_Duk3's Patching Utility, simply run it and open the patch
file (*.PAT) you want to use. If K1n9_Duk3's Patching Utility found more than
one supported version of the executables (or none at all), it will ask you to
open the executable manually. Save the extracted files into the "static"
directory using the suggested file names.

If you are going to use CKPatch, just copy the files as described above, then
use the provided batch files as described in the following section.


Setting up a working environment in DOSBox
------------------------------------------

The Borland C++ compilers, as well as CKPATCH, MAKEOBJ and LZEXE, are all DOS
programs, so you will need a system that is capable of running DOS programs or
you will have to use an emulator like DOSBox to run these programs.

If you are going to use DOSBox, start out by preparing a base directory on your
system that you are going to mount as drive C: in DOSBox (mounting your real C:
drive as C: in DOSBox is NOT recommended). Let's assume your base directory is
"C:\BASE". Extract the contents of this package into that directory. Also copy
the Borland C++ compiler(s) you are going to use into that directory,
preferably into subdirectories named "BC20", "BC30" and "BC31" to make things
easier for you. You can use different names, but then you will have to edit a
couple of files and settings later on.

You could just start DOSBox and manually mount your base directory as the C:
drive in DOSBox, but this project comes with a couple of batch files that make
the process much easier, as long as things are set up correctly.

In case you didn't know, dragging and dropping a file onto DOSBox.exe will
start DOSBox and mount the directory in which that file is located in as the C:
drive in DOSBox, then it will try to execute that file in DOSBox.

At this point, your base directory should have the following contents:

	BC20         - Borland C++ 2.0
	BC30         - Borland C++ 3.0
	BC31         - Borland C++ 3.1
	KEEN4-6      - main source directory

	lzexe.exe    - for compressing executables
	rck4.bat     - opens RCK4.PRJ with the correct compiler version
	rck4c.bat    - opens RCK4C.PRJ with the correct compiler version
	rck5.bat     - opens RCK5.PRJ with the correct compiler version
	rck5c.bat    - opens RCK5C.PRJ with the correct compiler version
	rck6.bat     - opens RCK6.PRJ with the correct compiler version
	rck6c.bat    - opens RCK6C.PRJ with the correct compiler version
	rck4gt.bat   - opens RCK4GT.PRJ with the correct compiler version
	rck5gt.bat   - opens RCK5GT.PRJ with the correct compiler version
	rck6e15.bat  - opens RCK6E15.PRJ with the correct compiler version
	rck6c15.bat  - opens RCK6C15.PRJ with the correct compiler version
	ripnmake.bat - extracts data files and converts them into .OBJ files
	readme.txt   - this file

The first order of business is to drag and drop RIPNMAKE.BAT onto DOSBox.exe or
onto a shortcut to DOSBox.exe. This will try to extract the required data files
from the original executables via CKPatch and then convert the data files into
.OBJ files that the compiler can include when generating the new executables.
The .OBJ files will be created in the KEEN4, KEEN5 and KEEN6 subdirectories.

Note that you should do this step even if you already extracted the data files
using K1n9_Duk3's Patching Utility. The RIPNMAKE.BAT file may not be able to
run the CKPatch programs in that case, but as long as the extracted data files
are present in the "KEEN4-6\static" directory, it will still convert them into
.OBJ files and place the .OBJ files into the correct directories.

If you are using CKPatch and you want to extract the data for both the EGA and
the CGA versions, you need to delete the KEEN*.EXE files from the "static"
directory after running RIPNMAKE.BAT and then copy the other executables into
that directory and run RIPNMAKE.BAT again.


Check the KEEN4, KEEN5 and KEEN6 directories and make sure the following files
are in them:

	CK?ADICT.OBJ
	CK?AHEAD.OBJ
	CK?CDICT.OBJ (for the CGA version)
	CK?CHEAD.OBJ (for the CGA version)
	CK?EDICT.OBJ (for the EGA version)
	CK?EHEAD.OBJ (for the EGA version)
	CK?INTRO.OBJ
	CK?MHEAD.OBJ
	CK6ORDER.OBJ (only for Keen 6)

You can exit from DOSBox after this is done. Simply type "exit" at the prompt
and hit Enter.


Compiling the code:
-------------------

The other batch files in your base directory (RCK*.BAT) are provided to make
compiling the code easy. Simply drag and drop the batch file onto DOSBox.exe
(or onto a shortcut to DOSBox.exe) and it will open the respective project in
the correct version of the compiler.

	RCK4       - EGA version 1.4 - Apogee and FormGen release
	RCK5       - EGA version 1.4 - Apogee (and FormGen?) release)
	RCK6       - EGA version 1.4 - FormGen release
	RCK4C      - CGA version 1.4 - Apogee and FormGen release
	RCK5C      - CGA version 1.4 - Apogee (and FormGen?) release)
	RCK6C      - CGA version 1.4 - FormGen release

	RCK4GT     - EGA version 1.4 - GT release
	RCK5GT     - EGA version 1.4 - GT release
	RCK6E15    - EGA version 1.5 - FormGen release
	RCK6C15    - CGA version 1.5 - FormGen release

The first six are set up for use with Borland C++ 3.0 by default, the later
four are set up for use with Borland C++ 3.1. If you want to compile them with
a different version of the compiler, edit the batch file and change the
compiler directory (in the "SET PATH=" line) to the one you wish to use. Then
open the project (drag and drop the batch file onto DOSBox.exe) and then select
"Options" -> "Directories" from the main menu. Make sure that the Include and
Library directory settings point to a version that you actually have installed.

Note that RCK4, RCK4C, RCK5, RCK5C, RCK6 and RCK6C are set up to compile with
BC30, but using the Library directory from BC20. This is required for
recreating the original executables, but if you don't have both of these
versions and you don't care about creating 100% identical copies, you can just
change the directory settings to point to the compiler you have.

To actually compile the code, press F9 or select "Compile" -> "Make" from the
menu.

Compiling all of the files may take a while, depending on your CPU cycles
settings in DOSBox. By default, DOSBox should automatically switch to maximum
cycles mode when Borland C++ 3.0 or 3.1 are started, but not when Borland C++
2.0 is started. You can simply enter the command "cycles max" at the DOSBox
prompt (or add it to the batch files) to switch DOSBox into maximum cycles mode
if the automatic switch doesn't work for you.

With the current code base, it is completely normal to get 3 or 4 warnings as
the code is compiled. One may come from CK_KEEN2.C ("Condition is always true")
when compiling Keen 6 v1.5. The other three of them should come from ID_US_1.C
(2x "Condition is always true" and 1x "Unreachable code"). You can ignore these
warnings.

Once the code has been compiled, simply press ALT+X or select "File" -> "Quit"
from the menu. Don't just close DOSBox while Borland C++ is still running, you
would just end up with lots of useless swap files in your project directory.

Type "exit" at the DOS prompt to quit DOSBox.

Please note that you should always quit DOSBox after compiling a project.
Trying to compile a second project after the first one may cause issues with
the provided batch files and the way they adjust the PATH environment variable.
For example, DOSBox may end up starting the wrong version of the compiler.


Recreating the original executables
===================================

Here's the TL;DR for advanced users:

RCK4.PRJ, RCK5.PRJ, RCK6.PRJ, RCK4C.PRJ, RCK5C.PRJ, RCK6C.PRJ:
- Use same compiler settings as in the provided RCK?.PRJ files
- Use LIB directory from Borland C++ 2.0
- Use INCLUDE directory from Borland C++ 3.0
- Compile with Borland C++ 3.0
- Compress compiled EXE file with LZEXE version 0.91

RCK4GT.PJR, RCK5GT.PRJ, RCK6E15.PRJ, RCK6C15.PRJ:
- Use same compiler settings as in the provided RCK?GT.PRJ/RCK6?15.PRJ files
- Use LIB and INCLUDE directories from Borland C++ 3.1
- Compile with Borland C++ 3.1
- Compress compiled EXE file with LZEXE version 0.91

To create 100% identical copies of the original v1.4 EGA executables, you will
need a copy of Borland C++ 3.0 as well as a copy of Borland C++ 2.0 (or at
least the LIB directory from Borland C++ 2.0).

Make sure to start with the original project files included in this package.
Those have all of the compiler options set to the correct values. Different
settings may produce slightly different code and the whole point of this is
to get code that's 100% identical to the original executables.

Unlike Borland C++ 3.1, version 3.0 will not recompile every file when you
select "Build all" from the "Compile" menu if neither that file nor the header
files used by that file have changed since the last time that file was
compiled. Therefore I recommend that you delete all *.OBJ files from the
"KEEN*\OBJ" directory to make sure everything gets recompiled. The CLEANUP.BAT
file will take care of that (can be run in Windows as well as in DOSBox).

Open the project in BC30 and select "Options" -> "Directories" from the menu.
Change the "Include Directories" path to the INCLUDE directory from BC30 and
change the "Library Directories" path to the LIB directory from BC20.

Compile the code (select either "Make" or "Build all" from the "Compile" menu)
and once the compiler is done, quit to DOS(Box) and compress the new executable
with LZEXE. To compress RCK4.EXE, you must type "LZEXE RCK4.EXE" and hit Enter.
The program will display a message in French about additional information at
the end of the executable that will be lost after compression and ask if you
want to abort. Type "N" and hit Enter to compress the file.


For reference, these are the results you should be getting after compression:

Keen 4 EGA version 1.4 (Apogee)  : size = 105108 bytes, CRC = 6646B983
Keen 4 EGA version 1.4 (FormGen) : size = 105140 bytes, CRC = F91E558B
Keen 4 EGA version 1.4 (GT)      : size = 106178 bytes, CRC = 0A05442E
Keen 4 CGA version 1.4 (Apogee)  : size =  98007 bytes, CRC = F544DD41
Keen 4 CGA version 1.4 (FormGen) : size =  98007 bytes, CRC = 018FA365

Keen 5 EGA version 1.4 (Apogee)  : size = 106417 bytes, CRC = 2A45589A
(No FormGen release of Keen 5 EGA version 1.4 is known at this time.)
Keen 5 EGA version 1.4 (GT)      : size = 107611 bytes, CRC = 5E450B12
Keen 5 CGA version 1.4 (Apogee)  : size =  98880 bytes, CRC = FB9EB429
(No FormGen release of Keen 5 CGA version 1.4 is known at this time.)

Keen 6 EGA version 1.4 (FormGen) : size = 107719 bytes, CRC = 9CDACDAE
Keen 6 EGA version 1.5 (FormGen) : size = 109058 bytes, CRC = 5B828EE2
Keen 6 CGA version 1.4 (FormGen) : size = 100964 bytes, CRC = F36A4C51
Keen 6 CGA version 1.5 (FormGen) : size = 102166 bytes, CRC = D2F379B8

The GT versions appear to have been compiled with Borland C++ 3.1 and its LIB
directory, but otherwise using the same compiler and optimization settings as
in the earlier Apogee/FormGen versions.

The only difference between the Apogee/FormGen and the GT versions of the Keen
4 and 5 executables (obvious differences in the included OBJ files aside) is
that the GT version has only four entries in the help menu instead of five
(the Order Info section has been removed) and that the GT version has a
different set of default high scores. You must have GOODTIMES defined to
compile these versions (already set in the RCK?GT.PRJ files).

Keen 6 EGA/CGA version 1.5 was also compiled with Borland C++ 3.1 and its LIB
directory, but it uses completely different compiler and optimization settings
and also has one new variable in CK_PLAY.C that is never used but still has to
be present to recreate the original code. It appears that somebody just pressed
the "Fastest code" button in the compiler optimizations window before version
1.5 was compiled, which was a bad idea for this code. None of the variables in
the ID Engine are marked as "volatile", not even the ones that may be changed
by an interrupt. That means the optimizations may end up generating code that
leads to endless loops, as is the case with the "while (!LastScan);" loop in
the PicturePause() routine.

To recreate the exact same files as the original Keen 6 v1.5 executables, you
need to compress the generated executables with LZEXE and then hex-edit the
compressed files to replace the "LZ91" signature at offset 0x1C in the EXE file
with four 0 bytes. If you don't have a hex editor, you can use K1n9_Duk3's
Patching Utility for that step. Simply open the "fix_RCK6_v15.pat" or the
"fix_RCK6C_v15.pat" patch file (located in the KEEN4-6\KEEN6\OBJ and
KEEN4-6\KEEN6C\OBJ directories, respectively) with K1n9_Duk3's Patching Utility
and let it patch the compressed executable for you.


Borland C++ 2.0 issues?
=======================

Any versions of Keen 4-6 prior to version 1.4 appear to have been compiled with
Borland C++ 2.0. Version 1.4 is where they switched from 2.0 to 3.0 (without
changing the Library directory to the one from 3.0 for some reason).

The code in this package can be built with Borland C++ 2.0 and the compiled
executables appear to be working perfectly fine. But when that version of the
compiler was used to compile the "Return to the Shadowlands" source code (which
is based on an earlier incarnation of this source code recreation), it caused
problems. The code compiled without errors, but the compiled executable would
always quit with the error message "Abnormal program termination".

The reason for this error was that the compiler appeared to have forced the
"grneeded" array (declared as "far" in ID_CA.C) into the main data segment
instead of giving it its own far data segment. With this additional data in the
main data segment, there was simply not enough space left for the stack and
that is why the program aborted with an error message.

It is unclear what caused this problem. The same source code compiles perfectly
fine with Borland C++ 3.1 and produces an executable that actually works. If
similar issues arise when working on mods based on this source code, try using
Borland C++ 3.1 instead of whatever version you were using before.


Special Thanks
==============

I would like to thank Jason Blochowiak, Adrian Carmack, John Carmack, Tom Hall,
John Romero and Robert Prince for creating Commander Keen 4-6 in the first
place.

Special thanks to John Carmack (and the rest of id Software) for releasing the
Wolfenstein 3-D and Spear of Destiny source code to the public in July 1995.

Extra special thanks to the late Richard Mandel of Flat Rock Software and
everybody else involved in the process of getting the source code of some of
the games id Software made for Softdisk (Catacomb series, Hovertank, Keen
Dreams) released to the public. I have learned a lot from the source code of
these games and this project certainly would not have been possible without it.

Thanks to PCKF user Multimania for supplying additional information regarding
the names of functions and variables for Keen 4 and Keen 5.

And last, but not least, I would like to thank NY00123 for figuring out how to
get the compiler to recreate Keen 6 v1.5 and also for sharing a lot of valuable
information about the "gamesrc-ver-recreation" project in various public posts
on the RGB Classic Games Forum (among other places). That's where I first heard
about the TDUMP utility, which is certainly a much better way to extract names
from the debugging information that some executables came with. And using IDA
to open executables and then make IDA generate ASM files that can be compared
more easily using tools like FC in Windows/DOS is pretty much the best way to
track down differences between those two executables without going insane.

[END OF FILE]