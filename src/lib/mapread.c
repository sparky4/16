#include "src/lib/mapread.h"

static int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

//this function is quite messy ^^; sorry! it is a quick and dirty fix~
static int dump(const char *js, jsmntok_t *t, size_t count, int indent, /*char *js_sv,*/ map_t *map) {
	int i, j, k;
	if (count == 0) {
		return 0;
	}
	/* We may want to do strtol() here to get numeric value */
	if (t->type == JSMN_PRIMITIVE) {
		if(js_sv == "height")
		{
			map->height = (int)strtol(js+t->start, (char **)js+t->end, 10);
			//printf("h:[%d]\n", map->height);
		}else if(js_sv == "width")
		{
			map->width = (int)strtol(js+t->start, (char **)js+t->end, 10);
			//printf("w:[%d]\n", map->width);
		}
		return 1;
		/* We may use strndup() to fetch string value */
	} else if (t->type == JSMN_STRING) {
		//printf("'%.*s'", t->end - t->start, js+t->start);
		if (jsoneq(js, t, "height") == 0 && indent==1)
		{
			js_sv="height";//strdup(js+t->start);//, t->end - t->start);
			//printf("%s\n", js_sv);
		}else if (jsoneq(js, t, "width") == 0 && indent==1)
		{
			js_sv="width";//strdup(js+t->start);//, t->end - t->start);
			//printf("%s\n", js_sv);
		}else js_sv=NULL;
		return 1;
	} else if (t->type == JSMN_OBJECT) {
		//printf("\n");
		j = 0;
		for (i = 0; i < t->size; i++) {
			//for (k = 0; k < indent; k++) printf("\t");
			j += dump(js, t+1+j, count-j, indent+1, map);
			//printf(": ");
			j += dump(js, t+1+j, count-j, indent+1, map);
			//printf("\n");
		}
		return j+1;
	} else if (t->type == JSMN_ARRAY) {
		j = 0;
		//printf("==\n");
		for (i = 0; i < t->size; i++) {
			//for (k = 0; k < indent-1; k++) printf("\t");
			//printf("\t-");
			j += dump(js, t+1+j, count-j, indent+1, map);
			//printf("==\n");
		}
		return j+1;
	}
	return 0;
}

int loadmap(char *mn, map_t *map)
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
			fprintf(stderr, "realloc(): errno=%d\n", errno);
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
			dump(js, tok, p.toknext, 0, map);
			//fprintf(stdout, "[[[[%d]]]]\n", sizeof(tok));
			//printf("[\n%d\n]", jslen);
			eof_expected = 1;
		}
	}

	return 0;
}

/*int main()
{
	map_t map;
	loadmap("../../../../data/test.map", &map);
	return 0;
}*/
