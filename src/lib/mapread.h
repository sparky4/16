#ifndef _LIBMAPREAD_H_
#define _LIBMAPREAD_H_
#include <string.h>
#include <errno.h>

//#include <stdlib.h>

#include "src/lib/jsmn/jsmn.h"
#include "src/lib/modex16.h"
#include "src/lib/lib_head.h"

//---- temp!
static char *js_sv;

typedef struct {
	bitmap_t *data;
	word tileHeight;
	word tileWidth;
	unsigned int rows;
	unsigned int cols;
} tiles_t;

typedef struct {
	byte	*data;
	tiles_t *tiles;
	int width;
	int height;
} map_t;

static int jsoneq(const char *json, jsmntok_t *tok, const char *s);
static int dump(const char *js, jsmntok_t *t, size_t count, int indent, map_t *map, int q);
static int loadmap(char *mn, map_t *map);

#endif/*_LIBMAPREAD_H_*/
