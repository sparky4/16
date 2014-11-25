
VERGE version 2.01a, Copyright (C) 1998 BJ Eirich (aka vecna)
VERGE comes with ABSOLUTELY NO WARRANTY. This is free software,
and you are welcome to redistribute it under certain conditions.
For details refer to GNU.TXT.

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

>Jan 1, 2000<

Hey Guys,

This is the old source for V2 (DJGPP), along with the sources for the utils (mostly DJGPP), the VCC compiler (DJGPP; though it should compiler under Watcom), and MapEd 2 (Watcom). Just getting this out the door. The new source will be released and updated regularly a week or so from now, so you can futz around with this until that time.

I'm not sure the main engine even runs in this build, as all I got from compiling and running it was a fistful of page faults. Sorry, I don't feel like trying to make it work. It's old and stinky (enjoy! ^_^). It may be VC incompatibilities with newer stuff, it may be some minor oversight on my part from tweaking it at some point, it could by anything.

The utils and MapEd 2 seemed to be in working order, however. Didn't include all the util sources, because they're hidden somewhere on my hard drive. I'll find them later (like when I clean this cluttered mess of a hard drive).

I removed the CD audio code, because of GPL issues and it was a shoddy interface to begin with. I didn't take the FLI code out, though I can't remember where I got it from. It's likely I nabbed it up from X2FTP or some similar archive. If you can figure out where it's from and if I'm violating the GPL by including it (though I hacked it up a bit), just send me some proof of its origin and license and I'll remove it.

Otherwise, wham. There ya go. If you have any questions about it, just drop me a line and I'll see what I can do. Happy New Year, and have fun with it... Or possibly heaping gobs of frustration. :-)

- aen

-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

Pertaining to MikMod:

Parts of the library (in playercode/mdulaw.c) are derived from the files
libst.h and raw.c from an old version of the sox (SOund eXchange) package
written by Lance Norskog and Jef Poskanzer. The following copyright notice
applies to these parts:

Copyright (C) 1989 by Jef Poskanzer.
Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted, provided
that the above copyright notice appear in all copies and that both that
copyright notice and this permission notice appear in supporting
documentation.  This software is provided "as is" without express or
implied warranty.
