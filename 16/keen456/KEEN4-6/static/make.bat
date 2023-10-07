@echo off
rem bcc makeobj.c

makeobj c AUDIODCT.CK4 ..\keen4\ck4adict.obj _audiodict
makeobj f AUDIOHED.CK4 ..\keen4\ck4ahead.obj _AudioHeader _audiohead
makeobj c EGADICT.CK4  ..\keen4\ck4edict.obj _EGAdict
makeobj f EGAHEAD.CK4  ..\keen4\ck4ehead.obj EGA_grafixheader _EGAhead
makeobj c CGADICT.CK4  ..\keen4\ck4cdict.obj _CGAdict
makeobj f CGAHEAD.CK4  ..\keen4\ck4chead.obj CGA_grafixheader _CGAhead
makeobj f MAPHEAD.CK4  ..\keen4\ck4mhead.obj MapHeader _maphead
makeobj f introscn.CK4 ..\keen4\ck4intro.obj _introscn

makeobj c AUDIODCT.CK5 ..\keen5\ck5adict.obj _audiodict
makeobj f AUDIOHED.CK5 ..\keen5\ck5ahead.obj _AudioHeader _audiohead
makeobj c EGADICT.CK5  ..\keen5\ck5edict.obj _EGAdict
makeobj f EGAHEAD.CK5  ..\keen5\ck5ehead.obj EGA_grafixheader _EGAhead
makeobj c CGADICT.CK5  ..\keen5\ck5cdict.obj _CGAdict
makeobj f CGAHEAD.CK5  ..\keen5\ck5chead.obj CGA_grafixheader _CGAhead
makeobj f MAPHEAD.CK5  ..\keen5\ck5mhead.obj MapHeader _maphead
makeobj f introscn.CK5 ..\keen5\ck5intro.obj _introscn

makeobj c AUDIODCT.CK6 ..\keen6\ck6adict.obj _audiodict
makeobj f AUDIOHED.CK6 ..\keen6\ck6ahead.obj _AudioHeader _audiohead
makeobj c EGADICT.CK6  ..\keen6\ck6edict.obj _EGAdict
makeobj f EGAHEAD.CK6  ..\keen6\ck6ehead.obj EGA_grafixheader _EGAhead
makeobj c CGADICT.CK6  ..\keen6\ck6cdict.obj _CGAdict
makeobj f CGAHEAD.CK6  ..\keen6\ck6chead.obj CGA_grafixheader _CGAhead
makeobj f MAPHEAD.CK6  ..\keen6\ck6mhead.obj MapHeader _maphead
makeobj f introscn.CK6 ..\keen6\ck6intro.obj _introscn
makeobj f orderscn.CK6 ..\keen6\ck6order.obj _orderscn
