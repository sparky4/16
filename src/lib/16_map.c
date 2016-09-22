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
	char *s;
	i = 0;
	while(i<count) {
		if(jsoneq(js, &(t[i]), "layers") == 0) {
			i++;
			map->data = malloc(sizeof(byte*) * t[i].size);
			inner_end = t[i].end;
			k = 0;
			while(t[i].start < inner_end) {
				printf("%d, %d\n", t[i].start, inner_end);
				if(jsoneq(js, &(t[i]), "data") == 0) {
					#ifdef DEBUG_MAPVAR
					printf("Layer %d data: [", k);
					#endif
					map->data[k] = malloc(sizeof(byte) * t[i+1].size);
					for(j = 0; j < t[i+1].size; j++) {
						map->data[k][j] = (byte)atoi(js + t[i+2+j].start);
						#ifdef DEBUG_MAPVAR
						printf("%d, ", map->data[k][j]);
						#endif
					}
					i += j + 2;
					k++;
					#ifdef DEBUG_MAPVAR
					puts("]");
					#endif
				}
				else{
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
					//fix this to be far~
					map->tiles[k] = malloc(sizeof(tiles_t));
					map->tiles[k]->btdata = malloc(sizeof(bitmap_t));
					map->tiles[k]->tileHeight = 16;
					map->tiles[k]->tileWidth = 16;
					map->tiles[k]->rows = 1;
					map->tiles[k]->cols = 1;
					map->tiles[k]->debug_text=false;
					//Fix to load tileset specified.
					//And move to vrs, probably
					bp = bitmapLoadPcx("data/ed.pcx");
					map->tiles[k]->btdata = &bp;
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

int loadmap(char *mn, map_t *map) {
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
