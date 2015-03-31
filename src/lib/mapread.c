#include "src/lib/mapread.h"

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

//this function is quite messy ^^; sorry! it is a quick and dirty fix~
static int dump(const char *js, jsmntok_t *t, size_t count, int indent, /*char *js_sv,*/ map_t *map, int q) {
	int i, j, k;
	bitmap_t bp;
	if (count == 0) {
		return 0;
	}
	/* We may want to do strtol() here to get numeric value */
	if (t->type == JSMN_PRIMITIVE) {
		if(js_sv == "data")
		{
			/*
				here we should recursivly call dump again here to skip over the array until we get the facking width of the map.
				so we can initiate the map which allocates the facking map->tiles->data->data properly and THEN we can return
				here to read the data.... That is my design for this... wwww

				FUCK well i am stuck.... wwww
			*/
//++++			map->data[q] = (byte)strtol(js+t->start, &(char *)t->end, 10);
//			printf("%d[%d]", q, map->data[q]);
		}
		else
		if(js_sv == "height")
		{
			//map->height = (int)malloc(sizeof(int));
			map->height = (int)strtol(js+t->start, (char **)js+t->end, 10);
			//printf("h:[%d]\n", map->height);
		}else if(js_sv == "width")
		{
			map->width = (int)strtol(js+t->start, (char **)js+t->end, 10);
//			printf("w:[%d]\n", map->width);
		}
		return 1;
		/* We may use strndup() to fetch string value */
	} else if (t->type == JSMN_STRING) {
		if(jsoneq(js, t, "data") == 0 )
		{
//			printf("[[[[%d|%d]]]]\n", &(t+1)->size, (t+1)->size);
//++++			printf("\n%.*s[xx[%d|%d]xx]\n", (t+1)->end - (t+1)->start, js+(t+1)->start, &(t+1)->size, (t+1)->size);
//++++			map->data = malloc(sizeof(byte) * (t+1)->size);
//++++			map->tiles = malloc(sizeof(tiles_t));
			//map->tiles->data = malloc(sizeof(bitmap_t));
//++++			bp = bitmapLoadPcx("data/ed.pcx");
			map->tiles->data = &bp;
			//map->tiles->data->data = malloc((16/**2*/)*16);
			//map->tiles->data->width = (16/**2*/);
			//map->tiles->data->height= 16;
			map->tiles->tileHeight = 16;
			map->tiles->tileWidth = 16;
			map->tiles->rows = 1;
			map->tiles->cols = 1;
			js_sv="data";//strdup(js+t->start);//, t->end - t->start);
		}
		else
		if (jsoneq(js, t, "height") == 0 && indent==1)
		{
			js_sv="height";//strdup(js+t->start);//, t->end - t->start);
		}else if (jsoneq(js, t, "width") == 0 && indent==1)
		{
			js_sv="width";//strdup(js+t->start);//, t->end - t->start);
		}else js_sv=NULL;
		return 1;
	} else if (t->type == JSMN_OBJECT) {
		//printf("\n");
		j = 0;
		for (i = 0; i < t->size; i++) {
			//for (k = 0; k < indent; k++) printf("\t");
			j += dump(js, t+1+j, count-j, indent+1, map, i);
			//printf(": ");
			j += dump(js, t+1+j, count-j, indent+1, map, i);
			//printf("\n");
		}
		return j+1;
	} else if (t->type == JSMN_ARRAY) {
		j = 0;
		//printf("==\n");
		for (i = 0; i < t->size; i++) {
			//for (k = 0; k < indent-1; k++) printf("\t");
			//printf("\t-");
			j += dump(js, t+1+j, count-j, indent+1, map, i);
			//printf("==\n");
		}
		return j+1;
	}
	return 0;
}

static int loadmap(char *mn, map_t *map)
{
	int r;
	int eof_expected = 0;
	char *js = NULL;
	size_t jslen = 0;
	char buf[BUFSIZ];

	jsmn_parser p;
	jsmntok_t *tok;
	size_t tokcount = 2;

	FILE *fh = fopen(mn, "r");

	/* Prepare parser */
	jsmn_init(&p);

	/* Allocate some tokens as a start */
	tok = malloc(sizeof(*tok) * tokcount);
	if (tok == NULL) {
		fprintf(stderr, "malloc(): errno=%d\n", errno);
		return 3;
	}

	for (;;) {
		/* Read another chunk */
		r = fread(buf, 1, sizeof(buf), fh);
		if (r < 0) {
			fprintf(stderr, "fread(): %d, errno=%d\n", r, errno);
			return 1;
		}
		if (r == 0) {
			if (eof_expected != 0) {
				return 0;
			} else {
				fprintf(stderr, "fread(): unexpected EOF\n");
				return 2;
			}
		}

		js = realloc(js, jslen + r + 1);
		if (js == NULL) {
			fprintf(stderr, "realloc(): errno = %d\n", errno);
			return 3;
		}
		strncpy(js + jslen, buf, r);
		jslen = jslen + r;

again:
		r = jsmn_parse(&p, js, jslen, tok, tokcount);
		if (r < 0) {
			if (r == JSMN_ERROR_NOMEM) {
				tokcount = tokcount * 2;
				tok = realloc(tok, sizeof(*tok) * tokcount);
				if (tok == NULL) {
					fprintf(stderr, "realloc(): errno=%d\n", errno);
					return 3;
				}
				goto again;
			}
		} else {
			dump(js, tok, p.toknext, 0, map, 0);
			eof_expected = 1;
		}
	}

	free(js);
	free(tok);
	fclose(fh);

	return 0;
}
