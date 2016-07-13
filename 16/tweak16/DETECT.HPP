#ifndef _DETECT_HPP
#define _DETECT_HPP

#include "misc.hpp"
#include "regtable.hpp"
#include "vgalib.hpp"


struct ModeInfo
	{
	enum EmulType { EM_CONFLICT, HERC, CGA, VGA };
	enum ColorsType { COLOR2=2, COLOR4=4, COLOR16=16, COLOR256=256 };
	enum AGType { AG_CONFLICT, ALPHA, GRAPHICS };

	EmulType emulation;
	AGType alphaGraph;
	ColorsType colors;
	int hClocks, vClocks, xres, yres, lineClocks, spareClocks, adrOffset,
		hPixelsPerClock, vxres, vyres, vxresBytes;
	float xpages, ypages;
	Boolean countBy2, chain4;

	ModeInfo(RegisterTable &rt)			{ detectFrom(rt); }
	void detectFrom(RegisterTable &);
	GraphicsAPI *getGraphicsAPI();
	void show();
	};

#endif