/*
 *	Mode detection:
 *	---------------
 *	Currently incompatible with CGA and Hercules modes.
 *		200-line CGA modes are detected as 100-line.
 *		640x200x2 is detected as 16-color.
 *		Hercules not supported at all.
 *	Monochrome EGA/VGA modes are also *not* supported, due to use of
 *		different port addresses.
 *
 *	Variables of interest:
 *		hPixels, vPixels - the actual resolution of the mode, in characters
 *			in text modes, in pixels in graphics modes.
 *		adrOffset - the number of addressable bytes between two vertically
 *			adjacent pixels, or words between two vertically adjacent
 *			characters in text mode.
 *
 */

#include "detect.hpp"
#include "screen.hpp"
#include <dos.h>
#include <conio.h>



void ModeInfo::detectFrom(RegisterTable &regTable)
	{
	Register *GCMode = regTable.getRegister(GRACON_ADDR, 0x05);
	Register *GCMisc = regTable.getRegister(GRACON_ADDR, 0x06);
	Register *ACMode = regTable.getRegister(ATTRCON_ADDR, 0x10);
	Register *CRTChde = regTable.getRegister(CRTC_ADDR, 0x01);
	Register *CRTCoflo = regTable.getRegister(CRTC_ADDR, 0x07);
	Register *CRTCscan = regTable.getRegister(CRTC_ADDR, 0x09);
	Register *CRTCvde = regTable.getRegister(CRTC_ADDR, 0x12);
	Register *CRTCoffs = regTable.getRegister(CRTC_ADDR, 0x13);
	Register *CRTCmode = regTable.getRegister(CRTC_ADDR, 0x17);
	Register *SEQmmode = regTable.getRegister(SEQ_ADDR, 0x04);

	int temp;
	switch ((*GCMode)(5,2))
		{
		case 0:
			colors = COLOR16;		// might also be COLOR2 !!!
			hPixelsPerClock = 8;
			break;
		case 1:
			colors = COLOR4;
			hPixelsPerClock = 16;
			break;
		case 2:
			colors = COLOR256;
			hPixelsPerClock = 4;
			break;
		}

	temp = (**GCMisc & 1 != 0);
	if (temp == (**ACMode & 1 != 0))
		alphaGraph = temp ? GRAPHICS : ALPHA;
	else
		alphaGraph = AG_CONFLICT;

	if (alphaGraph == ALPHA)
		hPixelsPerClock = 1;

	chain4 = (*SEQmmode)(3,1);
	countBy2 = (*CRTCmode)(3,1);
	adrOffset = **CRTCoffs * 2 * (countBy2+1);

	hClocks = **CRTChde + 1;
	vClocks =
		(**CRTCvde | ((*CRTCoflo)(1,1) << 8) | ((*CRTCoflo)(6,1) << 9)) + 1;

	xres = hClocks * hPixelsPerClock;
	vxresBytes = vxres = adrOffset * hPixelsPerClock;

	if (alphaGraph == ALPHA)
		vxresBytes *= 2;
	else
		if (alphaGraph == GRAPHICS)
			if (colors == COLOR16)
				vxresBytes /= 8;
			else
				if (colors == COLOR256 && !chain4)
					vxresBytes /= 4;

	lineClocks = (*CRTCscan)(0,4) + 1;
	if ((*CRTCscan)(7,1) << 5)
		lineClocks *= 2;
	yres = vClocks / lineClocks;
	spareClocks = vClocks % lineClocks;
	vyres = (alphaGraph == GRAPHICS ? 65536L : 32768L) / vxresBytes;
    xpages = float(vxres)/xres;
	ypages = float(vyres)/yres;
	}


GraphicsAPI *ModeInfo::getGraphicsAPI()
	{
	if (alphaGraph == GRAPHICS)
		switch(colors)
			{
			case COLOR16:
				return new Planar16(xres, yres, vxres);
			case COLOR256:
				if (chain4)
					return new Chained256(xres, yres, vxres);
				else
					return new Unchained256(xres, yres, vxres);
			}
	return 0;
	}

void ModeInfo::show()
	{
	window(editWidth/2+2, editHeight-18, editWidth, editHeight-7);
	clrscr();
	textattr(INFO_COLOR);
	cprintf("This seems to be a %d-color\n\r"
			"%s mode.\n\r\n\r"
			"Physical resolution: %d x %d\n\r"
			"Virtual resolution:  %d x %d\n\r\n\r"
			"Page resolution:     %3.1f x %3.1f",
			colors,
			alphaGraph==ALPHA ? "text" :
				alphaGraph==GRAPHICS ? (chain4 ? "linear graphics" :
												 "planar graphics") :
									   "graph/text conflict",
			xres, yres,
			vxres, vyres,
			xpages, ypages
			);
	window(1,1,editWidth,editHeight);
	}

