#ifndef _LIBFMAPREAD_H_
#define _LIBFMAPREAD_H_
#include <stdlib.h>	//huge wwww
#include <stdio.h>		//huge wwww
#include <string.h>
#include <errno.h>
#include <malloc.h>

#include "src/lib/jsmn/farjsmn.h"
//++++#include "src/lib/modex16.h"
//++++#include "src/lib/lib_head.h"
#include "src/lib/types.h"

//#define DEBUG_MAPDATA
//#define DEBUG_MAPVAR
//#define DEBUG_DUMPVARS
//#define DEBUG_JS

typedef struct {
//++++	bitmap_t *data;
	word tileHeight;
	word tileWidth;
	unsigned int rows;
	unsigned int cols;
} tiles_t;

typedef struct {
	byte huge *data;
	tiles_t *tiles;
	int width;	//this has to be signed!
	int height;	//this has to be signed!
} map_t;

int jsoneq(const char huge *json, jsmntok_t huge *tok, const char huge *s);
word dump(const char huge *js, jsmntok_t huge *t, size_t count, word indent, char *js_sv, map_t *map, dword q);
int loadmap(char *mn, map_t *map);

#endif/*_LIBFMAPREAD_H_*/
