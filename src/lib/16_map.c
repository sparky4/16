/* Project 16 Source Code~
 * Copyright (C) 2012-2017 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
 *
 * This file is part of Project 16.
 *
 * Project 16 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Project 16 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 * write to the Free Software Foundation, Inc., 51 Franklin Street,
 * Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "src/lib/16_map.h"

// Ideally, preprocess json during compilation and read serialized data

int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

//this function is quite messy ^^; sorry! it is a quick and dirty fix~
word dump(const char *js, jsmntok_t *t, size_t count, word indent, char *js_sv, map_t *map, dword q) {
	dword i;
	word j;//, k;
	bitmap_t bp;
	#ifdef DEBUG_JS
	if(indent==0)
	{
		fprintf(stdout, "%s\n", js);
		fprintf(stdout, "\n");
	}
	#endif
	#ifdef DEBUG_DUMPVARS
	fprintf(stdout, "t->size=[%d]	", t->size);
	fprintf(stdout, "q=[%d]	", q);
	fprintf(stdout, "indent= [%d]	", indent);
	fprintf(stdout, "js_sv= [%s]\n", js_sv);
	#endif
	if (count == 0) {
		return 0;
	}
	/* We may want to do strtol() here to get numeric value */
//0000fprintf(stderr, "t->type=%d\n", t->type);
	if (t->type == JSMN_PRIMITIVE) {
		if(strstr(js_sv, "data"))
		{
			/*
				here we should recursivly call dump again here to skip over the array until we get the facking width of the map.
				so we can initiate the map which allocates the facking map->tiles->data->data properly and THEN we can return
				here to read the data.... That is my design for this... wwww

				FUCK well i am stuck.... wwww
			*/
			map->data[q] = (byte)atoi(js+t->start);
			#ifdef DEBUG_MAPDATA
				fprintf(stdout, "%d[%d]", q, map->data[q]);
			#endif
		}
		else
		if(strstr(js_sv, "height"))
		{
			map->height = atoi(js+t->start);
			#ifdef DEBUG_MAPVAR
			fprintf(stdout, "indent= [%d]	", indent);
			fprintf(stdout, "h:[%d]\n", map->height);
			#endif
		}else if(strstr(js_sv, "width"))
		{
			map->width = atoi(js+t->start);
			#ifdef DEBUG_MAPVAR
			fprintf(stdout, "indent= [%d]	", indent);
			fprintf(stdout, "w:[%d]\n", map->width);
			#endif
		}
		return 1;
		/* We may use strndup() to fetch string value */
	} else if (t->type == JSMN_STRING) {
		if(jsoneq(js, t, "data") == 0)
		{
//			fprintf(stdout, "[[[[%d|%d]]]]\n", &(t+1)->size, (t+1)->size);
//			fprintf(stdout, "\n%.*s[xx[%d|%d]xx]\n", (t+1)->end - (t+1)->start, js+(t+1)->start, &(t+1)->size, (t+1)->size);
			map->data = malloc(sizeof(byte) * (t+1)->size);
			map->tiles = malloc(sizeof(tiles_t));
			map->tiles->btdata = malloc(sizeof(bitmap_t));
			//fix this to be far~
//0000			bp = bitmapLoadPcx("data/ed.pcx");
//			bp = bitmapLoadPcx("data/koishi^^.pcx");
			map->tiles->btdata = &bp;
//----			map->tiles->data = planar_buf_from_bitmap(&bp);
			//map->tiles->data->data = malloc((16/**2*/)*16);
			//map->tiles->data->width = (16/**2*/);
			//map->tiles->data->height= 16;
			map->tiles->tileHeight = 16;
			map->tiles->tileWidth = 16;
			map->tiles->rows = 1;
			map->tiles->cols = 1;
#ifdef __DEBUG_MAP__
			dbg_maptext=false;
#endif
			strcpy(js_sv, "data");//strdup(js+t->start);//, t->end - t->start);
		}
		else
		if (jsoneq(js, t, "height") == 0 && indent<=1)
		{
			strcpy(js_sv, "height");//strdup(js+t->start);//, t->end - t->start);
		}else
		if(jsoneq(js, t, "width") == 0 && indent<=1)
		{
			strcpy(js_sv, "width");//strdup(js+t->start);//, t->end - t->start);
		}else strcpy(js_sv, "\0");
		return 1;
	} else if (t->type == JSMN_OBJECT) {
		//fprintf(stdout, "\n");
		j = 0;
		for (i = 0; i < t->size; i++) {
			//for (k = 0; k < indent; k++) fprintf(stdout, "\t");
			j += dump(js, t+1+j, count-j, indent+1, js_sv, map, i);
			//fprintf(stdout, ": ");
			j += dump(js, t+1+j, count-j, indent+1, js_sv, map, i);
			//fprintf(stdout, "\n");
		}
		return j+1;
	} else if (t->type == JSMN_ARRAY) {
		j = 0;
		//fprintf(stdout, "==\n");
		for (i = 0; i < t->size; i++) {
			//for (k = 0; k < indent-1; k++) fprintf(stdout, "\t");
			//fprintf(stdout, "\t-");
			j += dump(js, t+1+j, count-j, indent+1, js_sv, map, i);
			//fprintf(stdout, "==\n");
		}
		return j+1;
	}
	return 0;
}

int loadmap(char *mn, map_t *map, global_game_variables_t *gvar)
{
	int r;
	static word incr=0;
	int eof_expected = 0;
	char *js = NULL;
	size_t jslen = 0;
	char buf[BUFSIZ];
	static char js_ss[16];

	jsmn_parser p;
	jsmntok_t *tok;
	size_t tokcount = 2;

	FILE *fh = fopen(mn, "r");

	/* Prepare parser */
	jsmn_init(&p);

	/* Allocate some tokens as a start */
//0000fprintf(stderr, "tok malloc\n");
	tok = malloc(sizeof(*tok) * tokcount);
	if (tok == NULL) {
		fprintf(stderr, "malloc(): errno=%d\n", errno);
		return 3;
	}

	for (;;) {
		/* Read another chunk */
//0000fprintf(stderr, "read\n");
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
//0000fprintf(stdout, "r=	[%d]	BUFSIZ=%d\n", r, BUFSIZ);
//0000fprintf(stderr, "js alloc~\n");
		js = realloc(js, jslen + r + 1);
		if (js == NULL) {
			fprintf(stderr, "*js=%Fp\n", *js);
			fprintf(stderr, "realloc(): errno = %d\n", errno);
			return 3;
		}
		strncpy(js + jslen, buf, r);
		jslen = jslen + r;

again:
//0000fprintf(stdout, "	parse~ tok=%zu	jslen=%zu	r=%d	_memavl()=%u	BUFSIZ=%d~\n", tokcount, jslen, r, _memavl(), BUFSIZ);
//0000fprintf(stdout, "p=[%u]	[%u]	[%d]\n", p.pos, p.toknext, p.toksuper);
/*
		I think it crashes on the line below when it tries to parse the data of huge maps... wwww this is a jsmn problem wwww
*/
		r = jsmn_parse(&p, js, jslen, tok, tokcount);
//0000fprintf(stdout, "r=	[%d]\n", r);
		if (r < 0) {
			if (r == JSMN_ERROR_NOMEM) {
				tokcount = tokcount * 2;
//0000fprintf(stderr, "tok realloc~ %zu\n", tokcount);
				tok = realloc(tok, sizeof(*tok) * tokcount);
				if (tok == NULL) {
					fprintf(stderr, "realloc(): errno=%d\n", errno);
					return 3;
				}
				goto again;
			}
		} else {
			//printf("js=%Fp\n", (js));
			//printf("*js=%Fp\n", (*(js)));
			//printf("&*js=%s\n", &(*(js)));
			//printf("&buf=[%Fp]\n", &buf);
			//printf("&buf_seg=[%x]\n", FP_SEG(&buf));
			//printf("&buf_off=[%x]\n", FP_OFF(&buf));
			//printf("&buf_fp=[%Fp]\n", MK_FP(FP_SEG(&buf), FP_OFF(&buf)));
			//printf("buf=[\n%s\n]\n", buf);
			//printf("buff=[%Fp]\n", buff);
			//printf("(*buff)=[%Fp]\n", (*buff));
			//printf("&(*buff)=[\n%s\n]\n", &(*buff));
			#ifdef DEBUG_DUMPVARS
			fprintf(stdout, "running dump~\n");
			#endif
			dump(js, tok, p.toknext, incr, &js_ss, map, 0);
			eof_expected = 1;
		}
	}

	//free(js);
	//free(tok);
	//fclose(fh);

	return 0;
}

void extract_map(const char *js, jsmntok_t *t, size_t count, map_t *map) {
	int i, j, k, indent=0, inner_end;
	//bitmap_t bp;

	i = 0;
	while(i<count) {
		if(jsoneq(js, &(t[i]), "layers") == 0) {
			i++;
			map->layerdata = malloc(sizeof(byte*) * t[i].size);
			inner_end = t[i].end;
			k = 0;
			while(t[i].start < inner_end) {
#ifdef DEBUG_DUMPVARS
				printf("t[%d].start=%d, %d\n", i, t[i].start, inner_end);
#endif
				if(jsoneq(js, &(t[i]), "data") == 0) {
#ifdef DEBUG_MAPDATA
					printf("Layer %d data: [\n", k);
#endif
					map->layerdata[k] = malloc(sizeof(byte) * t[i+1].size);
					map->data = map->layerdata[k];
					for(j = 0; j < t[i+1].size; j++) {
						map->layerdata[k][j] = (byte)atoi(js + t[i+2+j].start);
						//for backwards compatibility for rest of code
//						map->data[j] = map->layerdata[k][j];//(byte)atoi(js + t[i+2+j].start);//(byte)atoi(js+t->start);
#ifdef DEBUG_MAPDATA
						//printf("[%d,%d]%d", k, j, map->layerdata[k][j]);
						fprintf(stdout, "%c", map->data[j]+44);
#endif
					}
					i += j + 2;
					k++;
#ifdef DEBUG_MAPDATA
					puts("\n]");
#endif
				}else{
					i++;
				}
			}
		}
		if(jsoneq(js, &(t[i]), "tilesets") == 0) {
			i++;
			map->tiles = malloc(sizeof(tiles_t*) * t[i].size);
			inner_end = t[i].end;
			k = 0;
			while(t[i].start < inner_end) {
				if(jsoneq(js, &(t[i]), "image") == 0) {
					map->layertile[k] = malloc(sizeof(tiles_t));
					//Fix to load tileset specified.
					//And move to vrs, probably
//					bp = bitmapLoadPcx("data/ed.pcx");
//					map->layertile[k]->btdata = &bp;
					map->layertile[k]->btdata = malloc(sizeof(bitmap_t));
					map->layertile[k]->tileHeight = 16;
					map->layertile[k]->tileWidth = 16;
					map->layertile[k]->rows = 1;
					map->layertile[k]->cols = 1;
#ifdef __DEBUG_MAP__
					dbg_maptext=false;
#endif
					map->tiles->btdata = map->layertile[k]->btdata;
					map->tiles->tileHeight = 16;
					map->tiles->tileWidth = 16;
					map->tiles->rows = 1;
					map->tiles->cols = 1;
					k++;
				}
				i++;
			}
		}

		if (jsoneq(js, &(t[i]), "height") == 0 && indent<=1) {
			map->height = atoi(js + t[i+1].start);
#ifdef DEBUG_MAPVAR
			printf("Height: %d\n", map->height);
#endif
			i++;
		}
		else if(jsoneq(js, &(t[i]), "width") == 0 && indent<=1) {
			map->width = atoi(js + t[i+1].start);
#ifdef DEBUG_MAPVAR
			printf("Width: %d\n", map->width);
#endif
			i++;
		}
		i++;
	}
}

int newloadmap(char *mn, map_t *map) {
	char *js;

	jsmn_parser p;
	jsmntok_t *tok = NULL;
	size_t tokcount, file_s;

	FILE *fh = fopen(mn, "r");
	int status;

	/* Prepare parser */
	jsmn_init(&p);

	file_s = filesize(fh);
	js = malloc(file_s);
	if(js == NULL) {
		fprintf(stderr, "malloc(): errno = %d", 2);
		fclose(fh);
		return 3;
	}
	if(fread(js, 1, file_s, fh) != file_s) {
		fprintf(stderr, "Map read error");
		free(js);
		fclose(fh);
		return 1;
	}
	tokcount = jsmn_parse(&p, js, file_s, NULL, 0);
	tok = malloc(tokcount*sizeof(jsmntok_t));
	printf("Allocated %d tokens", tokcount);
	jsmn_init(&p);
	if((status = jsmn_parse(&p, js, file_s, tok, tokcount)) < 0)
	{
		printf("Error: %d\n", status);
		return status;
	}
	else if(status != tokcount) { printf("Warning: used %d tok\n", status);}
	extract_map(js, tok, tokcount, map);

	free(js);
	free(tok);
	fclose(fh);

	return 0;
}


//======


#define MAPBUFINLM (gvar->ca.camap.mapsegs)
int CA_loadmap(char *mn, map_t *map, global_game_variables_t *gvar)
{
	jsmn_parser p;
	jsmntok_t *tok = NULL;
	size_t tokcount, file_s;

	FILE *fh = fopen(mn, "r");
	int status;

	/* Prepare parser */
	jsmn_init(&p);

	file_s = filesize(fh);
	CA_LoadFile(mn, &MAPBUFINLM, gvar);
	tokcount = jsmn_parse(&p, MAPBUFINLM, file_s, NULL, 0);
	tok = malloc(tokcount*sizeof(jsmntok_t));
//	printf("Allocated %d tokens", tokcount);
	jsmn_init(&p);
	if((status = jsmn_parse(&p, MAPBUFINLM, file_s, tok, tokcount)) < 0)
	{
		printf("Error: %d\n", status);
		return status;
	}
	else if(status != tokcount) { printf("Warning: used %d tok\n", status);}
	extract_map(MAPBUFINLM, tok, tokcount, map);

	free(tok);
	fclose(fh);

	return 0;
}
