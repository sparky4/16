#ifndef XTYPES_HPP
    #define XTYPES_HPP

typedef unsigned char BYTE;
typedef signed   char SBYTE;
typedef unsigned char FLAG;
typedef unsigned short COORD;
typedef unsigned short DIST;

struct blitbuf {
    DIST xsize;
    DIST ysize;
    BYTE *image;
};

#endif

