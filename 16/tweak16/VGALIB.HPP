#ifndef _VGALIB_HPP
#define _VGALIB_HPP

class GraphicsAPI;
class VGAGraphicsAPI;
class Chained256;
class Unchained256;
class Planar16;

class GraphicsAPI
	{
	public:
		enum HJustify	{ LEFT, HCENTER, RIGHT };
		enum VJustify	{ TOP, VCENTER, BOTTOM };

	protected:
		int xres, yres, vxres, vyres, color, colors;
		unsigned char *font, *libID;
		int fontWidth, fontHeight;
		HJustify hJustify;
		VJustify vJustify;
	public:
		GraphicsAPI(int, int, int, int, int);
		virtual ~GraphicsAPI()				{}
		virtual char *getLibID()			{ return libID; }
		virtual int getWidth()				{ return xres; }
		virtual int getHeight()				{ return yres; }
		virtual int getVirtualWidth()		{ return vxres; }
		virtual int getVirtualHeight()		{ return vyres; }
		virtual long getPageSize()			{ return long(xres)*yres; }
		virtual int getColors()				{ return colors; }
		virtual void setColor(int c)		{ color = c; }
		virtual void setBase(int, int)		=0;
		virtual void syncWithRefresh()		=0;
		virtual int getColor()				{ return color; }
		virtual void putPixel(int x, int y)	{ putPixel(x, y, color); }
		virtual void putPixel(int x, int y, int c) =0;
		virtual int getPixel(int x, int y)	=0;
		virtual void hLine(int, int, int);
		virtual void vLine(int, int, int);
		virtual void line(int, int, int, int);
		virtual void rectangle(int, int, int, int);
		virtual void bar(int, int, int, int);
		virtual void wipe();
		virtual void putChar(int, int, int);
		virtual void putText(int, int, char*);
		virtual void setTextJustify(HJustify, VJustify);
	};

class VGAGraphicsAPI : public GraphicsAPI
	{
	protected:
		int xbytes;
		static unsigned char *videoBuf;
        virtual unsigned getOffset(int, int);
		virtual int getPelPan(int) =0;
	public:
		VGAGraphicsAPI(int, int, int, int, int);
		void syncWithRefresh();
		void setBase(int, int);
	};

class Chained256 : public VGAGraphicsAPI
	{
	protected:
		int getPelPan(int);
		unsigned getOffset(int, int);
	public:
		Chained256(int, int, int);
		void putPixel(int, int, int);
		int getPixel(int, int);
		virtual void hLine(int, int, int);
	};

class Unchained256 : public VGAGraphicsAPI
	{
	protected:
		int getPelPan(int);
	public:
		Unchained256(int, int, int);
		void putPixel(int, int, int);
		int getPixel(int, int);
		virtual void hLine(int, int, int);
	};

class Planar16 : public VGAGraphicsAPI
	{
	protected:
		int getPelPan(int);
	public:
		Planar16(int, int, int);
		void putPixel(int, int, int);
		int getPixel(int, int);
		void hLine(int, int, int);
	};

#endif