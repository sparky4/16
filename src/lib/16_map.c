/* Project 16 Source Code~
 * Copyright (C) 2012-2016 sparky4 & pngwen & andrius4669 & joncampbell123 & yakui-lover
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

#include "16_map.h"

// Ideally, preprocess json during compilation and read serialized data

int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start &&
			strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

void extract_map(const char *js, jsmntok_t *t, size_t count, map_t *map) {
	int i, j, k, indent, inner_end;
	bitmap_t bp;
	#ifdef DEBUG_DUMPVARS
	fprintf(stderr, "t->size=[%d]	", t->size);
	fprintf(stderr, "q=[%d]	", q);
	fprintf(stderr, "indent= [%d]	", indent);
	fprintf(stderr, "js_sv= [%s]\n", js_sv);
	#endif
	map->tiles = malloc(sizeof(tiles_t));
	map->tiles->btdata = malloc(sizeof(bitmap_t));
	//fix this to be far~
	//And a pointer
	bp = bitmapLoadPcx("data/ed.pcx");
	map->tiles->btdata = &bp;
	map->tiles->tileHeight = 16;
	map->tiles->tileWidth = 16;
	map->tiles->rows = 1;
	map->tiles->cols = 1;
	map->tiles->debug_text=false;
	
	while(i<count) {
		if(jsoneq(js, t, "layers" == 0)) {
			i++;
			map->data = malloc(sizeof(*byte) * t[i].size);
			inner_end = t[i].end;
			k = 0;
			while(t[i].start < inner_end) {
				if(jsoneq(js, t, "data") == 0) {
					map->data[k] = malloc(sizeof(byte) * t[i+1].size);
					for(j = 0; j < t[i+1].size; j++) {
						map->data[k][j] = (byte)atoi(js + t[i+1+j].start);
					}
					i += j + 1;
					k++;
				}
			}
		}/*
		if(jsoneq(js, t, "tilesets" == 0)) {
			i++;
			map->tiles = malloc(sizeof(*tiles_t) * t[i].size);
			inner_end = t[i].end;
			k = 0;
			while(t[i].start < inner_end) {
				if(jsoneq(js, t, "data") == 0) {
					map->tiles[k] = malloc(sizeof(tiles_t));
					bp = bitmapLoadPcx("data/ed.pcx");
					map->tiles[k].btdata = &bp;
					k++;
				}
			}
		}*/

		if (jsoneq(js, t, "height") == 0 && indent<=1) {
			map->height = atoi(js + t[i+1].start);
			#ifdef DEBUG_MAPVAR
			fprintf(stderr, "indent= [%d]	", indent);
			fprintf(stderr, "h:[%d]\n", map->height);
			#endif
			i++;
		}
		else if(jsoneq(js, t, "width") == 0 && indent<=1) {
			map->width = atoi(js+t[i+1]->start);
			#ifdef DEBUG_MAPVAR
			fprintf(stderr, "indent= [%d]	", indent);
			fprintf(stderr, "w:[%d]\n", map->width);
			#endif
			i++;
		}
		i++;
	}
}

int loadmap(char *mn, map_t *map) {
	char *js;
	char js_ss[16];

	jsmn_parser p;
	jsmntok_t *tok = NULL;
	size_t tokcount, file_s;

	FILE *fh = fopen(mn, "r");

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
	jsmn_parse(&p, js, file_s, tok, tokcount);
	fprintf(stderr, "running dump~\n");
	printf("%d\n", p.toknext);
	extract_map(js, tok, tokcount, 0);

	free(js);
	free(tok);
	fclose(fh);
	
	return 0;
}
